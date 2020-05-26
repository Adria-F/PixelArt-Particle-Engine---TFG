#include "Application.h"
#include "SpriteSheetExporter.h"

#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleParticles.h"
#include "ModuleGUI.h"
#include "ModuleCamera.h"

SpriteSheetExporter::SpriteSheetExporter(float duration, uint frames, float2 frameSize, int pixelSize, int rows, int columns): duration(duration), frameNum(frames), frameSize(frameSize), pixelSize(pixelSize), rows(rows), columns(columns)
{
}

SpriteSheetExporter::~SpriteSheetExporter()
{
	ClearFrameList();
	App->textures->DeleteImage(spritesheet);
}

void SpriteSheetExporter::CreateSpriteSheet()
{
	//Clear previous list
	ClearFrameList();
	App->textures->DeleteImage(spritesheet);

	//Restart particles
	App->particles->Stop();
	App->particles->Play();

	//Set frame and pixel size
	uint prevPixelSize = App->render->pixelSize;
	App->camera->OnResize(frameSize.x, frameSize.y);
	App->render->OnResize(frameSize.x, frameSize.y);
	App->render->pixelSize = pixelSize;

	//Collect all frames images
	float timeStep = duration / (frameNum-1);
	float dt = 1.0f / 60.0f; //60 fps dt
	float frameTime = 0.0f;
	frames.push_back(App->textures->CreateTextureImage(App->render->pixelartTexture, frameSize.x, frameSize.y)); //Attach empty texture for frame 0
	for (float time = 0; time <= duration; time += dt)
	{
		App->particles->Update(dt); //Update particles
		
		frameTime += dt;
		if (frameTime >= timeStep)
		{
			frameTime -= timeStep;
			//Render textures
			App->render->DrawScene(); //To draw the pixel art we first need the scene drawn
			App->render->DrawPixelArt(); //Draw final frame result

			frames.push_back(App->textures->CreateTextureImage(App->render->pixelartTexture, frameSize.x, frameSize.y));
		}
	}

	//Create spritesheet
	spritesheet = App->textures->GenerateImage(frameSize.x*columns, frameSize.y*rows); //TODO: Now spritesheet is a single row

	//Attach all the frames
	int count = 0;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			App->textures->InsertImage(spritesheet, frames[count++], j*frameSize.x, i*frameSize.y, frameSize.x, frameSize.y);
			if (count == frameNum)
				break;
		}
		if (count == frameNum)
			break;
	}

	//Restore previous values
	App->camera->OnResize(App->gui->sceneSize.x, App->gui->sceneSize.y);
	App->render->OnResize(App->gui->sceneSize.x, App->gui->sceneSize.y);
	App->render->pixelSize = prevPixelSize;
}

void SpriteSheetExporter::ExportSpriteSheet(const char* path)
{
	if (spritesheet != 0)
		App->textures->ExportImage(path, spritesheet);
}

void SpriteSheetExporter::ClearFrameList()
{
	for (int i = 0; i < frames.size(); ++i)
	{
		App->textures->DeleteImage(frames[i]);
	}
	frames.clear();
}

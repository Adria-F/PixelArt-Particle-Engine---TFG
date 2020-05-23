#include "Application.h"
#include "SpriteSheetExporter.h"

#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleParticles.h"
#include "ModuleGUI.h"

SpriteSheetExporter::SpriteSheetExporter(float duration, uint frames, float2 frameSize, int pixelSize): duration(duration), frameNum(frames), frameSize(frameSize), pixelSize(pixelSize)
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

	//Collect all frames images
	float timeStep = duration / frameNum;
	float dt = 1.0f / 60.0f; //60 fps dt
	float frameTime = 0.0f;
	for (float time = 0; time <= duration; time += dt)
	{
		App->particles->Update(dt); //Update particles
		
		frameTime += dt;
		if (frameTime >= timeStep)
		{
			frameTime = 0.0f;
			//Render textures
			App->render->DrawScene(); //To draw the pixel art we first need the scene drawn
			App->render->DrawPixelArt(); //Draw final frame result

			frames.push_back(App->textures->CreateTextureImage(App->render->pixelartTexture, App->gui->sceneSize.x, App->gui->sceneSize.y));
		}
	}

	//Create spritesheet
	spritesheet = App->textures->GenerateImage(App->gui->sceneSize.x*frameNum, App->gui->sceneSize.y); //TODO: Now spritesheet is a single row

	//Attach all the frames
	for (int i = 0; i < frameNum; ++i)
	{
		App->textures->InsertImage(spritesheet, frames[i], i*App->gui->sceneSize.x, 0, App->gui->sceneSize.x, App->gui->sceneSize.y);
	}
}

void SpriteSheetExporter::ExportSpriteSheet(const char* path)
{
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

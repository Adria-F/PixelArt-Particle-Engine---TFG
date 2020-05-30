#include "Application.h"
#include "SpriteSheetExporter.h"

#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleParticles.h"
#include "ModuleGUI.h"
#include "ModuleCamera.h"

SpriteSheetExporter::SpriteSheetExporter(float duration, uint frames, float2 frameSize, int pixelSize, int rows, int columns): duration(duration), frameNum(frames), frameSize(frameSize), pixelSize(pixelSize), rows(rows), columns(columns)
{
	spritesheetTexture = new Texture("spritesheet");
	spritesheetTexture->width = frameSize.x*columns;
	spritesheetTexture->height = frameSize.y*rows;

	camera = new ModuleCamera();
	camera->OnResize(256, 256);
}

SpriteSheetExporter::~SpriteSheetExporter()
{
	ClearFrameList();
	App->textures->DeleteImage(spritesheet);

	RELEASE(spritesheetTexture);
	RELEASE(camera);
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
	while (frames.size() < frameNum)
	{
		App->particles->Update(dt); //Update particles
		
		frameTime += dt;
		if (frameTime >= timeStep)
		{
			frameTime -= timeStep;
			//Render textures
			App->render->DrawScene(camera->getProjectionMatrix(), camera->getViewMatrix()); //To draw the pixel art we first need the scene drawn
			App->render->DrawPixelArt(frameSize, pixelSize); //Draw final frame result

			frames.push_back(App->textures->CreateTextureImage(App->render->exportPixelartTexture, frameSize.x, frameSize.y));
		}
	}

	//Create spritesheet
	float2 spritesheetSize = { frameSize.x*columns, frameSize.y*rows };
	if (forceSquared)
	{
		float maxValue = max(spritesheetSize.x, spritesheetSize.y);
		spritesheetSize = { maxValue, maxValue };
	}
	spritesheet = App->textures->GenerateImage(spritesheetSize.x, spritesheetSize.y); //TODO: Now spritesheet is a single row

	//Attach all the frames
	int count = 0;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			App->textures->InsertImage(spritesheet, frames[count++], j*frameSize.x, spritesheetSize.y - ((i+1)*frameSize.y), frameSize.x, frameSize.y);
			if (count == frameNum)
				break;
		}
		if (count == frameNum)
			break;
	}

	//Convert to OpenGL texture
	if (spritesheetTexture->GL_id == 0)
	{
		glGenTextures(1, &spritesheetTexture->GL_id);
		glBindTexture(GL_TEXTURE_2D, spritesheetTexture->GL_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	App->textures->GetTextureFromImage(spritesheet, spritesheetTexture);
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

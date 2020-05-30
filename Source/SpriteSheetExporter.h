#ifndef __SPRITESHEET_EXPORTER_H__
#define __SPRITESHEET_EXPORTER_H__

#include "Globals.h"

#include <vector>

class Texture;
class ModuleCamera;

class SpriteSheetExporter
{
public:

	SpriteSheetExporter(float duration, uint frames, float2 frameSize, int pixelSize, int rows, int columns);
	~SpriteSheetExporter();

	void CreateSpriteSheet();
	void ExportSpriteSheet(const char* path);

	void ClearFrameList();

public:

	uint spritesheet = 0;
	Texture* spritesheetTexture = nullptr;

	std::vector<uint> frames; //List of ilImages

	float duration = 0.0f;
	uint frameNum = 0;

	int rows = 0;
	int columns = 0;
	bool forceSquared = false;

	float2 frameSize;
	int pixelSize;

	ModuleCamera* camera = nullptr;

};

#endif // !__SPRITESHEET_EXPORTER_H__
#ifndef __PANEL_EXPORTSPRITESHEET_H__
#define __PANEL_EXPORTSPRITESHEET_H__

#include "Panel.h"

class SpriteSheetExporter;

enum spritesheetType
{
	SQUARE,
	SINGLE_ROW,
	SINGLE_COLUMN,
	CUSTOM
};

class PanelExportSpritesheet : public Panel
{
public:

	PanelExportSpritesheet(const char* name);
	~PanelExportSpritesheet();

	void SetFlags();
	void DrawContent();

public:

	SpriteSheetExporter* exporter = nullptr;

	std::string path;

	float duration = 1.0f;
	int frames = 2;

	spritesheetType disposition = SQUARE;
	std::string typeDisplay = "Square";
	int rows = 1;
	int columns = 2;

	float2 frameSize = { 32,32 };
	int pixelSize = 5;

	bool multipleOf4 = false;
};

#endif // !__PANEL_EXPORTSPRITESHEET_H__
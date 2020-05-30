#ifndef __PANEL_EXPORTSPRITESHEET_H__
#define __PANEL_EXPORTSPRITESHEET_H__

#include "Panel.h"

class SpriteSheetExporter;
class ModuleCamera;

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

	void RefreshSpriteSheet();

	void OnOpen();
	void OnClose();

public:

	SpriteSheetExporter* exporter = nullptr;

	std::string path;

	float duration = 1.0f;
	int frames = 2;

	spritesheetType disposition = SQUARE;
	std::string typeDisplay = "Square";
	int customRows = 3;
	int customColumns = 3;
	int rows = 3;
	int columns = 3;

	float2 frameSize = { 256, 256 };
	int pixelSize = 5;

	bool multipleOf4 = false;
};

#endif // !__PANEL_EXPORTSPRITESHEET_H__
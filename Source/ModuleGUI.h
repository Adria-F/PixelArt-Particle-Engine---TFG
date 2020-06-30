#ifndef __MODULEGUI_H__
#define __MODULEGUI_H__

#include "Module.h"
#include <map>

union SDL_Event;
class Panel;
class ImFont;

class PanelExportSpritesheet;
class PanelNodeCanvas;

class ModuleGUI : public Module
{
public:

	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	bool Init();
	bool Start();
	update_state PreUpdate(float dt);
	update_state Update(float dt);
	bool CleanUp();

	void SaveSettings(JSON_Value* settings);
	void LoadSettings(JSON_Value* settings);

	void Draw();

	void handleInput(SDL_Event* event);
	void AddFont(int fontPercent = 100, int fontSize = DEFAULT_FONT_SIZE);
	ImFont* GetFont(int fontPercent = 100, int fontSize = DEFAULT_FONT_SIZE) const;

	bool UsingMouse() const;
	bool UsingKeyboard() const;

	bool IsExportPanelActive() const;
	void ResetCanvas();

	void DrawPlayMenu();

	void DrawColorBox(Color& color);
	void DrawGradientBox(Gradient& gradient);

	bool DrawInputInt(const char* label, int* value, float indent = 0.0f);
	bool DrawInputFloat(const char* label, float* value, float indent = 0.0f, bool enabled = true);
	bool DrawInputFloatRandomOption(const char* label, float* value, float* rand1, float* rand2, bool* random, float indent = 0.0f);
	bool DrawInputFloat2(const char* label, float* value, float indent = 0.0f, bool enabled = true);
	bool DrawInputFloat2RandomOption(const char* label, float* value, float* rand1, float* rand2, bool* random, float indent = 0.0f);
	bool DrawInputFloat3(const char* label, float* value, float indent = 0.0f, bool enabled = true);
	bool DrawInputFloat3RandomOption(const char* label, float* value, float* rand1, float* rand2, bool* random, float indent = 0.0f);

public:

	std::list<Panel*> panels;

	bool mouseOnScene = false;
	bool mouseOnPixelScene = false;
	bool mouseOnExportScene = false;

	bool clickedScene = false;

	float2 scenePosition;
	float2 sceneSize;

private:

	ImFont* defaultFont;
	std::map<int, ImFont*> fonts;

	PanelExportSpritesheet* panelExport = nullptr;
	PanelNodeCanvas* panelCanvas = nullptr;
};

#endif // !__MODULEGUI_H__
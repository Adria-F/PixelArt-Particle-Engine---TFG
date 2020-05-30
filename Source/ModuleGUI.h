#ifndef __MODULEGUI_H__
#define __MODULEGUI_H__

#include "Module.h"
#include <map>

union SDL_Event;
class Panel;
class ImFont;

class PanelExportSpritesheet;

class ModuleGUI : public Module
{
public:

	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	bool Init();
	bool Start();
	update_state PreUpdate(float dt);
	update_state Update(float dt);
	update_state PostUpdate(float dt);
	bool CleanUp();

	void Draw();

	void handleInput(SDL_Event* event);
	void AddFont(int fontPercent = 100, int fontSize = DEFAULT_FONT_SIZE);
	ImFont* GetFont(int fontPercent = 100, int fontSize = DEFAULT_FONT_SIZE) const;

	bool UsingMouse() const;
	bool UsingKeyboard() const;

	bool IsExportPanelActive() const;

	bool DrawInputFloat(const char* label, const char* id, float* value, float step, bool enabled, float* alternativeValue = nullptr, bool condition = false);
	void DrawColorBox(Color& color);
	void DrawGradientBox(Gradient& gradient);

	bool DrawInputInt(const char* label, int* value, float indent);
	bool DrawInputFloat(const char* label, float* value, float indent);

public:

	std::list<Panel*> panels;

	bool mouseOnScene = false;
	bool mouseOnPixelScene = false;
	bool mouseOnExportScene = false;

	float2 scenePosition;
	float2 sceneSize;

private:

	ImFont* defaultFont;
	std::map<int, ImFont*> fonts;

	PanelExportSpritesheet* panelExport = nullptr;
};

#endif // !__MODULEGUI_H__
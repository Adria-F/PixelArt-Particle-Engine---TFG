#ifndef __MODULEGUI_H__
#define __MODULEGUI_H__

#include "Module.h"
#include <map>

union SDL_Event;
class Panel;
class ImFont;

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
	void AddFont(float fontPercent);
	ImFont* GetFont(int fontPercent);

	bool UsingMouse() const;
	bool UsingKeyboard() const;

public:

	std::list<Panel*> panels;

	bool mouseOnScene = false;

private:

	std::map<int, ImFont*> fonts;
};

#endif // !__MODULEGUI_H__
#ifndef __MODULEGUI_H__
#define __MODULEGUI_H__

#include "Module.h"

class ModuleGUI : public Module
{
public:
	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	bool Init();
	bool Start();
	update_state PreUpdate();
	update_state Update();
	update_state PostUpdate();
	bool CleanUp();

	void Draw();
};

#endif // !__MODULEGUI_H__
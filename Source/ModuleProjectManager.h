#ifndef __MODULE_PROJECTMANAGER_H__
#define __MODULE_PROJECTMANAGER_H__

#include "Module.h"

class ModuleProjectManager : public Module
{
public:

	ModuleProjectManager(bool stat_enabled = true);
	~ModuleProjectManager();

	bool Start();
	update_state Update(float dt);
	bool CleanUp();

	void SaveProject(const char* path);
	void LoadProject(const char* path);

	std::string OpenFileDialog(const char* extensionHint, const char* extension);
	std::string SaveFileDialog(const char* extensionHint, const char* extension);

public:

	std::string workingDir;

};

#endif // !__MODULE_PROJECTMANAGER_H__
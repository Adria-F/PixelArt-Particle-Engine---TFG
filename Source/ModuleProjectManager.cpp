#include "Application.h"
#include "ModuleProjectManager.h"

#include "JSONManager.h"
#include "ModuleNodeCanvas.h"
#include "CanvasNode.h"

#include "WinItemDialog.h" 

ModuleProjectManager::ModuleProjectManager(bool stat_enabled)
{
}

ModuleProjectManager::~ModuleProjectManager()
{
}

bool ModuleProjectManager::Start()
{
	return true;
}

update_state ModuleProjectManager::Update(float dt)
{
	return update_state::UPDATE_CONTINUE;
}

bool ModuleProjectManager::CleanUp()
{
	return true;
}

void ModuleProjectManager::SaveProject(const char* path)
{
	JSON_File* projectFile = App->JSON_Manager->openWriteFile(path);
	JSON_Value* project = projectFile->createValue();

	//Save sequence
	for (std::list<CanvasNode*>::iterator it_n = App->nodeCanvas->nodes.begin(); it_n != App->nodeCanvas->nodes.end(); ++it_n)
	{
		(*it_n)->Save(project);
	}

	projectFile->addValue("project", project);
	projectFile->Write();
	App->JSON_Manager->closeFile(projectFile);
}

void ModuleProjectManager::LoadProject(const char* path)
{
}

std::string ModuleProjectManager::SaveFileDialog(const char* extensionHint, const char* extension)
{
	std::string path = saveFileWID(extensionHint, extension);
	if (path.length() > 0)
		return path + extension;
	else
		return path;
}

std::string ModuleProjectManager::OpenFileDialog(const char* extensionHint, const char* extension)
{
	return openFileWID(extensionHint, extension);
}

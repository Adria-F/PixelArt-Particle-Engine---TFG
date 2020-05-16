#include "Application.h"
#include "ModuleProjectManager.h"

#include "JSONManager.h"
#include "ModuleNodeCanvas.h"
#include "CanvasNode.h"

#include "WinItemDialog.h" 
#include "NodeGroup.h"

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
	project->convertToArray();

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
	JSON_File* projectFile = App->JSON_Manager->openReadFile(path);
	JSON_Value* project = projectFile->getValue("project");

	//Load sequence
	std::map<uint, CanvasNode*> nodes;
	for (int i = 0; i < project->getRapidJSONValue()->Size(); ++i)
	{
		JSON_Value* nodeDef = project->getValueFromArray(i);
		CanvasNode* node = App->nodeCanvas->CreateNode(nodeDef->getString("name"), (nodeType)nodeDef->getUint("type"), nodeDef->getVector2("position"), true);
		node->Load(nodeDef);
		nodes.insert(std::pair<uint, CanvasNode*>(node->UID, node));
	}
	//Connect sequence
	for (std::map<uint, CanvasNode*>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
	{
		if ((*it_n).second->parentUID != 0) //It has a parent
		{
			CanvasNode* parent = nodes[(*it_n).second->parentUID];
			if (parent != nullptr)
			{
				parent->InsertNode((*it_n).second);
			}
		}
		else //If it has no parent, add it to the ModuleNodeCanavs
		{
			App->nodeCanvas->nodes.push_back((*it_n).second);
		}
	}

	//Update NodeBoxes with all added nodes
	for (std::list<CanvasNode*>::iterator it_n = App->nodeCanvas->nodes.begin(); it_n != App->nodeCanvas->nodes.end(); ++it_n)
	{
		if ((*it_n)->type == PARTICLE || (*it_n)->type == EMITTER)
		{
			((NodeGroup*)(*it_n))->AddNodes();
		}
	}

	App->JSON_Manager->closeFile(projectFile);
	workingDir = path;
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

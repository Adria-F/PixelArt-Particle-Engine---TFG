#include "Application.h"
#include "ModuleProjectManager.h"

#include "JSONManager.h"
#include "ModuleNodeCanvas.h"
#include "ModuleParticles.h"
#include "ModuleCamera.h"
#include "ModuleGUI.h"
#include "CanvasNode.h"
#include "NodeGroup.h"

#include "WinItemDialog.h" 

ModuleProjectManager::ModuleProjectManager(bool stat_enabled)
{
}

ModuleProjectManager::~ModuleProjectManager()
{
}

bool ModuleProjectManager::Start()
{
	CreateDirectory(ASSETS_FOLDER, NULL);
	CreateDirectory(PROJECTS_FOLDER, NULL);

	return true;
}

void ModuleProjectManager::NewProject()
{
	for (std::list<CanvasNode*>::iterator it_n = App->nodeCanvas->nodes.begin(); it_n != App->nodeCanvas->nodes.end(); ++it_n)
	{
		RELEASE((*it_n));
	}
	App->nodeCanvas->nodes.clear();

	App->nodeCanvas->hoveredNode = nullptr;
	App->nodeCanvas->newHoveredNode = nullptr;
	App->nodeCanvas->selectedNode = nullptr;
	App->nodeCanvas->selectedConnection = nullptr;

	App->particles->ClearAll();

	App->camera->ResetCamera();
	App->gui->ResetCanvas();

	workingDir.clear();
}

void ModuleProjectManager::SaveProject(const char* path)
{
	BROFILER_CATEGORY("Save Project", Profiler::Color::Cyan);

	JSON_File* projectFile = App->JSON_Manager->openWriteFile(path);

	JSON_Value* settings = projectFile->createValue();
	App->SaveSettings(settings);
	projectFile->addValue("settings", settings);

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
	BROFILER_CATEGORY("Load Project", Profiler::Color::Cyan);

	JSON_File* projectFile = App->JSON_Manager->openReadFile(path);
	if (projectFile)
	{
		NewProject(); //Clear current project
		JSON_Value* settings = projectFile->getValue("settings");
		if (settings) //TMP
			App->LoadSettings(settings);

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
		///Connect all nodes with their parent
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
		///Update NodeBoxes with all added nodes
		for (std::list<CanvasNode*>::iterator it_n = App->nodeCanvas->nodes.begin(); it_n != App->nodeCanvas->nodes.end(); ++it_n)
		{
			if ((*it_n)->type == PARTICLE || (*it_n)->type == EMITTER)
			{
				((NodeGroup*)(*it_n))->AddNodes();
			}
		}
		///Restore link connections
		for (std::map<uint, CanvasNode*>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
		{
			(*it_n).second->LoadConnections(nodes);
		}

		App->JSON_Manager->closeFile(projectFile);
		workingDir = path;
		App->particles->Play();
	}
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

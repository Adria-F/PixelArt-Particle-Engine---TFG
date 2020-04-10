#include "Application.h"
#include "ModuleNodeCanvas.h"

#include "CanvasNode.h"
#include "ModuleGUI.h"

ModuleNodeCanvas::ModuleNodeCanvas(bool start_enabled): Module(start_enabled)
{
}

ModuleNodeCanvas::~ModuleNodeCanvas()
{
}

bool ModuleNodeCanvas::Start()
{
	//Create fonts for canvas zooming
	for (float i = MIN_ZOOM*100; i <= MAX_ZOOM*100; i += ZOOM_STEPS*100)
	{
		App->gui->AddFont(i);
	}

	return true;
}

update_state ModuleNodeCanvas::Update(float dt)
{
	std::list<CanvasNode*>::iterator it_n = nodes.begin();
	while (it_n != nodes.end())
	{
		if ((*it_n)->toDelete)
		{
			RELEASE((*it_n));
			it_n = nodes.erase(it_n);
		}
		else
			++it_n;
	}

	return UPDATE_CONTINUE;
}

update_state ModuleNodeCanvas::PostUpdate(float dt)
{
	if (connectionEnded)
	{
		connectionEnded = false;
		if (connectionCallback != nullptr && connecting != nullptr)
		{
			connectionCallback->SetConnection(connecting);
			connecting->SetConnection(connectionCallback);
		}
		connecting = nullptr;
		connectionCallback = nullptr;
	}

	return UPDATE_CONTINUE;
}

bool ModuleNodeCanvas::CleanUp()
{
	for (std::list<CanvasNode*>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
	{
		RELEASE((*it_n));
	}
	nodes.clear();

	return true;
}

void ModuleNodeCanvas::StopConnection()
{
	connectionEnded = true;
}

void ModuleNodeCanvas::StartConnection(NodeConnection* connection)
{
	connecting = connection;
}

void ModuleNodeCanvas::RequestConnection(NodeConnection* node)
{
	connectionCallback = node;
}

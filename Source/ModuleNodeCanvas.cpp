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
	for (float i = MIN_ZOOM; i <= MAX_ZOOM; i += ZOOM_STEPS)
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

bool ModuleNodeCanvas::CleanUp()
{
	for (std::list<CanvasNode*>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
	{
		RELEASE((*it_n));
	}
	nodes.clear();

	return true;
}
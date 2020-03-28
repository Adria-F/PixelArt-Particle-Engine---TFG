#include "Application.h"

// Include all Modules
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleGUI.h"
#include "ModuleCamera.h"

#include "ModuleParticles.h"

using namespace std;

Application::Application()
{
	// Create all modules
	window = new ModuleWindow();
	render = new ModuleRender();
	input = new ModuleInput();
	gui = new ModuleGUI();
	camera = new ModuleCamera();

	particles = new ModuleParticles();

	// Add all modules to application
	AddModule(window);
	AddModule(input);
	AddModule(gui);
	AddModule(camera);

	AddModule(particles);

	//Render module last
	AddModule(render);
}

Application::~Application()
{
	for (list<Module*>::reverse_iterator module = modules.rbegin(); module != modules.rend() && (*module) != NULL; ++module)
	{
		RELEASE((*module));
	}
}

bool Application::Init()
{
	bool ret = true;
	// Initialize everything needed
	START_RNG_SEED(); //TO be able to use RNG in any file without worrying about initializing the seed

	// Initialize all modules
	for (list<Module*>::iterator module = modules.begin(); module != modules.end() && ret == true; ++module)
	{
		ret = (*module)->Init();
	}

	// Start all modules
	for (list<Module*>::iterator module = modules.begin(); module != modules.end() && ret == true; ++module)
	{
		ret = (*module)->Start();
	}

	timer.Start();

	return ret;
}

void Application::PrepareUpdate()
{
	dt = (float)timer.ReadTime() / 1000.0f;

	timer.Start();
}

update_state Application::Update()
{
	update_state ret = UPDATE_CONTINUE;
	PrepareUpdate();

	// PreUpdate all modules
	for (list<Module*>::iterator module = modules.begin(); module != modules.end() && ret == UPDATE_CONTINUE; ++module)
	{
		ret = (*module)->PreUpdate(dt);
	}
	// Update all modules
	for (list<Module*>::iterator module = modules.begin(); module != modules.end() && ret == UPDATE_CONTINUE; ++module)
	{
		ret = (*module)->Update(dt);
	}
	// PostUpdate all modules
	for (list<Module*>::iterator module = modules.begin(); module != modules.end() && ret == UPDATE_CONTINUE; ++module)
	{
		ret = (*module)->PostUpdate(dt);
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	// CleanUp all modules
	for (list<Module*>::iterator module = modules.begin(); module != modules.end() && ret == true; ++module)
	{
		ret = (*module)->CleanUp();
	}

	return ret;
}

void Application::AddModule(Module* module)
{
	modules.push_back(module);
}

#include "Application.h"

// Include all Modules

using namespace std;

Application::Application()
{
	// Create all modules


	// Add all modules to application

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

	return ret;
}

update_state Application::Update()
{
	update_state ret = UPDATE_CONTINUE;

	// PreUpdate all modules
	for (list<Module*>::iterator module = modules.begin(); module != modules.end() && ret == UPDATE_CONTINUE; ++module)
	{
		ret = (*module)->PreUpdate();
	}
	// Update all modules
	for (list<Module*>::iterator module = modules.begin(); module != modules.end() && ret == UPDATE_CONTINUE; ++module)
	{
		ret = (*module)->Update();
	}
	// PostUpdate all modules
	for (list<Module*>::iterator module = modules.begin(); module != modules.end() && ret == UPDATE_CONTINUE; ++module)
	{
		ret = (*module)->PostUpdate();
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

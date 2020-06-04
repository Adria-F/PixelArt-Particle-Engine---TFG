#include "Application.h"

// Include all Modules
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleGUI.h"
#include "ModuleCamera.h"

#include "ModuleParticles.h"
#include "ModuleNodeCanvas.h"
#include "ModuleProjectManager.h"
#include "ModuleTextures.h"

using namespace std;

Application::Application()
{
	JSON_Manager = new JSONManager();

	// Create all modules
	window = new ModuleWindow();
	render = new ModuleRender();
	input = new ModuleInput();
	gui = new ModuleGUI();
	camera = new ModuleCamera();

	particles = new ModuleParticles();
	nodeCanvas = new ModuleNodeCanvas();
	projectManager = new ModuleProjectManager();
	textures = new ModuleTextures();

	// Add all modules to application
	AddModule(window);
	AddModule(input);
	AddModule(gui);
	AddModule(camera);

	AddModule(particles);
	AddModule(nodeCanvas);
	AddModule(projectManager);
	AddModule(textures);

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

	//TMP
	//SDL_GL_SetSwapInterval(0);

	return ret;
}

void Application::PrepareUpdate()
{
	dt = (float)timer.ReadTime() / 1000.0f;

	if (dt > 0.0f)
	{
		if (FPS_COUNT >= 1000)
		{
			TOTAL_FPS = AVG_FPS;
			FPS_COUNT = 1;
		}
		TOTAL_FPS += 1.0 / dt;
		FPS_COUNT++;
		AVG_FPS = TOTAL_FPS / FPS_COUNT;

		std::string title = "Application Name | FPS:" + std::to_string((int)(1 / dt)) + " FPS_Avg:" + std::to_string(AVG_FPS);
		SDL_SetWindowTitle(App->window->window, title.c_str());
	}

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

void Application::SaveSettings(JSON_Value* settings)
{
	for (std::list<Module*>::iterator it_m = modules.begin(); it_m != modules.end(); ++it_m)
	{
		(*it_m)->SaveSettings(settings);
	}
}

void Application::LoadSettings(JSON_Value* settings)
{
	for (std::list<Module*>::iterator it_m = modules.begin(); it_m != modules.end(); ++it_m)
	{
		(*it_m)->LoadSettings(settings);
	}
}

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"
#include "Timer.h"

#include <list>

// Predefine all Modules
class Module;
class ModuleWindow;
class ModuleRender;
class ModuleInput;
class ModuleGUI;
class ModuleCamera;

class ModuleParticles;

class Application
{
public:

	//Declare All Module pointers
	ModuleWindow*		window = nullptr;
	ModuleRender*		render = nullptr;
	ModuleInput*		input = nullptr;
	ModuleGUI*			gui = nullptr;
	ModuleCamera*		camera = nullptr;

	ModuleParticles*	particles = nullptr;

private:

	std::list<Module*> modules;

	Timer timer;
	float dt;

	double TOTAL_FPS = 0.0;
	float AVG_FPS = 0.0f;
	int FPS_COUNT = 0;

public:

	Application();
	~Application();

	bool Init();
	void PrepareUpdate();
	update_state Update();
	bool CleanUp();

	void AddModule(Module* module);
};

extern Application* App;

#endif // !__APPLICATION_H__
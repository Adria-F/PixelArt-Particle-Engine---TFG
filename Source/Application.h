#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include <list>

// Predefine all Modules
class Module;
class ModuleWindow;
class ModuleRender;
class ModuleInput;
class ModuleGUI;

class Application
{
public:

	//Declare All Module pointers
	ModuleWindow*		window = nullptr;
	ModuleRender*		render = nullptr;
	ModuleInput*		input = nullptr;
	ModuleGUI*			gui = nullptr;

private:

	std::list<Module*> modules;

public:

	Application();
	~Application();

	bool Init();
	update_state Update();
	bool CleanUp();

	void AddModule(Module* module);
};

extern Application* App;

#endif // !__APPLICATION_H__
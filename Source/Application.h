#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"
#include "Module.h"

#include <list>

// Predefine all Modules
class Module;

class Application
{
public:

	//Declare All Module pointers


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
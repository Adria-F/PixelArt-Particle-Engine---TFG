#ifndef __MODULEINPUT_H__
#define __MODULEINPUT_H__

#include "Module.h"
#include "SDL/include/SDL.h"
#include "SDL\include\SDL_scancode.h"

class ModuleInput : public Module
{
public:

	ModuleInput(bool start_enabled = true);
	~ModuleInput();

	bool Init();
	update_state PreUpdate();
	bool CleanUp();

public:


};

#endif // !__MODULEINPUT_H__
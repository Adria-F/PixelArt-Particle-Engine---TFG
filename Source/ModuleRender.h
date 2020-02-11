#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"

class ModuleRender : public Module
{
public:

	ModuleRender(bool start_enabled = true);
	~ModuleRender();

	bool Init();
	update_state PreUpdate();
	update_state PostUpdate();
	bool CleanUp();

public:
	
	GLuint frameBuffer = 0;

	SDL_GLContext context;
};

#endif // !__MODULERENDER_H__
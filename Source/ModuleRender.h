#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"

#define MAX_LIGHTS 8

class Shader;

class ModuleRender : public Module
{
public:

	ModuleRender(bool start_enabled = true);
	~ModuleRender();

	bool Init();
	update_state PreUpdate();
	update_state PostUpdate();
	bool CleanUp();

	void OnResize(int width, int height);

public:
	
	GLuint frameBuffer = 0;

	SDL_GLContext context;

	Shader* defaultShader = nullptr;
};

#endif // !__MODULERENDER_H__
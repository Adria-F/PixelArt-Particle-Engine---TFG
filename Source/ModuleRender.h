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
	update_state PreUpdate(float dt);
	update_state PostUpdate(float dt);
	bool CleanUp();

	uint generateVAO(uint verticesSize, float* vertices, uint indicesSize, uint* indices);

	void OnResize(int width, int height);
	void generateFrameBuffer(int width, int height);

public:
	
	GLuint frameBuffer = 0;
	GLuint texture = 0;

	SDL_GLContext context;

	Shader* defaultShader = nullptr;

	uint VAO = 0;
};

#endif // !__MODULERENDER_H__
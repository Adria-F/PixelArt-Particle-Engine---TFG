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

	void SaveSettings(JSON_Value* settings);
	void LoadSettings(JSON_Value* settings);

	void DrawScene(float* projectionMatrix, float* viewMatrix);
	void DrawPixelArt(float2 viewportSize, uint pixelSize);

	uint generateVAO(uint verticesSize, float* vertices, uint indicesSize, uint* indices);

	void OnResize(int width, int height);
	void GenerateFrameBuffer(int width, int height);

	void GenerateExportFrameBuffer(int width, int height);

public:
	
	GLuint frameBuffer = 0;
	GLuint texture = 0;
	GLuint pixelartTexture = 0;

	GLuint exportFrameBuffer = 0;
	GLuint exportTexture = 0;
	GLuint exportPixelartTexture = 0;

	SDL_GLContext context;

	Shader* defaultShader = nullptr;
	Shader* pixelartShader = nullptr;

	uint VAO = 0;

	uint pixelSize = 10;
};

#endif // !__MODULERENDER_H__
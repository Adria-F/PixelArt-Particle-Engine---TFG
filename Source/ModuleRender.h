#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include <queue>
#include <vector>

#define MAX_LIGHTS 8

class Shader;
class Particle;

enum blendModeType
{
	BLEND_NORMAL,
	BLEND_ADDITIVE
};

struct closerToCamera
{
	bool operator()(Particle* Obj_1, Particle* Obj_2)const;
};
struct older
{
	bool operator()(Particle* Obj_1, Particle* Obj_2)const;
};
struct younger
{
	bool operator()(Particle* Obj_1, Particle* Obj_2)const;
};

struct SortingLayer
{
	void PushOlder(Particle* particle) { olderBuffer.push(particle); }
	void PushYounger(Particle* particle) { youngerBuffer.push(particle); }

	std::priority_queue<Particle*, std::vector<Particle*>, older> olderBuffer;
	std::priority_queue<Particle*, std::vector<Particle*>, younger> youngerBuffer;
};

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
	void DrawBuffer();

	uint generateVAO(uint verticesSize, float* vertices, uint indicesSize, uint* indices);

	void OnResize(int width, int height);
	void GenerateFrameBuffer(int width, int height);

	void GenerateExportFrameBuffer(int width, int height);

	void SetBlendMode(blendModeType mode);

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

	blendModeType blendMode = BLEND_ADDITIVE;

	std::priority_queue<Particle*, std::vector<Particle*>, closerToCamera> proximityBuffer;
	std::map<int, SortingLayer> layers;
};

#endif // !__MODULERENDER_H__
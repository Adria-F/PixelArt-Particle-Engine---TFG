#ifndef __MODULEPARTICLES_H__
#define __MODULEPARTICLES_H__

#include "Module.h"
#include "Globals.h"
#include <vector>

class Particle
{
public:

	Particle() {}
	~Particle() {}

	void Update();
	void Draw();

public:

	//TODO: Particle data structure
	float3 position = { 0.0f,0.0f,0.0f };
};

class ParticleEmitter
{
public:

	ParticleEmitter(Particle* templateParticle=nullptr):templateParticle(templateParticle)
	{}
	~ParticleEmitter();

	void UpdateParticles();
	void DrawParticles();

public:

	//TODO: Particle emitter data structure
	float3 position = { 0.0f,0.0f,0.0f };
	float frequency = 0.0f;

	Particle* templateParticle = nullptr;

	std::vector<Particle*> particles;
};

class ModuleParticles : public Module
{
public:
	ModuleParticles(bool start_enabled = true);
	~ModuleParticles();

	bool Start();
	update_state Update();
	bool CleanUp();

	void DrawParticles();

private:

	uint rectangleVAO;

	std::vector<ParticleEmitter*> emitters;
};

#endif // !__MODULEPARTICLES_H__
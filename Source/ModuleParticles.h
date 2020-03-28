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

	void LookCamera();
	void Update(float dt);
	void Draw();

public:

	//TODO: Particle data structure
	vec position = vec::zero;
	Quat rotation = Quat::identity;
	vec scale = vec::one;

	bool billboard = true;
};

class ParticleEmitter
{
public:

	ParticleEmitter(Particle* templateParticle=nullptr):templateParticle(templateParticle)
	{
		lastEmit = frequency;
	}
	~ParticleEmitter();

	void Play();
	void Pause();
	void Stop();

	void UpdateParticles(float dt);
	void DrawParticles();

public:

	//TODO: Particle emitter data structure
	vec position = { 0.0f,0.0f,0.0f };
	float frequency = 1.0f;

private:

	Particle* templateParticle = nullptr;
	std::vector<Particle*> particles;

	float lastEmit = 0.0f;
	bool playing = true;
	bool restarted = false;
};

class ModuleParticles : public Module
{
public:
	ModuleParticles(bool start_enabled = true);
	~ModuleParticles();

	bool Start();
	update_state Update(float dt);
	bool CleanUp();

	void Play();
	void Pause();
	void Stop();
	void DrawParticles();

private:

	uint rectangleVAO;

	std::vector<ParticleEmitter*> emitters;
};

#endif // !__MODULEPARTICLES_H__
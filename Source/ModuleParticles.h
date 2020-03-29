#ifndef __MODULEPARTICLES_H__
#define __MODULEPARTICLES_H__

#include "Module.h"
#include "Globals.h"
#include <list>

class Particle
{
public:

	Particle() {}
	Particle(Particle* templateParticle);
	~Particle() {}

	void LookCamera();
	void Update(float dt);
	void Draw();

public:

	//TODO: Particle data structure
	vec position = vec::zero;
	Quat rotation = Quat::identity;
	vec scale = vec::one;

	float4 color = float4::one;

	vec direction = vec::unitY;
	float speed = 2.0f;

	float lifeTime = 5.0f;

	bool billboard = true;

	bool toDestroy = false;

private:

	float timeAlife = 0.0f;
};

class ParticleEmitter
{
public:

	ParticleEmitter(Particle* templateParticle = nullptr);
	~ParticleEmitter();

	void Play();
	void Pause();
	void Stop();

	void UpdateParticles(float dt);
	void DrawParticles();

	vec randomDirectionInCone(float radius, float height) const;

public:

	//TODO: Particle emitter data structure
	vec position = { 0.0f,0.0f,0.0f };
	float frequency = 1.0f;

private:

	Particle* templateParticle = nullptr;
	std::list<Particle*> particles;

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

	std::list<ParticleEmitter*> emitters;
};

#endif // !__MODULEPARTICLES_H__
#ifndef __MODULEPARTICLES_H__
#define __MODULEPARTICLES_H__

#include "Module.h"
#include "Globals.h"
#include <list>
#include <unordered_map>

class Particle;
class ParticleEmitter;

class ParticleData;
class EmitterData;

class BaseTransformParticleNode;
class BaseMovementParticleNode;
class BaseColorParticleNode;
class BaseTransformEmitterNode;

class ColorParticleNode;

class Particle
{
public:

	Particle(ParticleEmitter* emitter);
	Particle(ParticleEmitter* emitter, Particle* templateParticle);
	~Particle();

	void Update(float dt);
	void Draw();

	void SetRandomLifeTime(bool random);
	float GetLifePercent() const;

public:

	union
	{
		struct
		{
			BaseTransformParticleNode* baseTransform;
			BaseMovementParticleNode* baseMovement;		
			BaseColorParticleNode* baseColor;

			ColorParticleNode* color;
		};
		ParticleData* data[MAX_PARTICLE_DATA];
	};

	ParticleEmitter* emitter = nullptr;

	float lifeTime = 5.0f;

	float randLifeTime1 = 3.0f;
	float randLifeTime2 = 5.0f;

	bool toDestroy = false;

private:

	float timeAlive = 0.0f;
	bool randomizeLifeTime = false;
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

	union
	{
		struct
		{
			BaseTransformEmitterNode* baseTransform;
		};
		EmitterData* data[MAX_PARTICLE_DATA];
	};

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
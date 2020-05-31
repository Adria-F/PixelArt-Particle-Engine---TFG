#ifndef __ENTITYDATA_H__
#define __ENTITYDATA_H__

#include "Globals.h"

class Particle;
class ParticleEmitter;

class EntityData
{
public:

	EntityData(Particle* particle): particle(particle)
	{}
	EntityData(ParticleEmitter* emitter) : emitter(emitter)
	{}
	virtual ~EntityData() {}

	virtual void Init() {}
	virtual void Execute(float dt) {}
	virtual void PrepareRender() {}

	virtual EntityData* Copy(Particle* particle) const { return new EntityData(particle); }

public:

	union
	{
		Particle* particle = nullptr;
		ParticleEmitter* emitter;
	};

	bool update = false;
};

#endif // !__ENTITYDATA_H__
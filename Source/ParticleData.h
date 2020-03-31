#ifndef __PARTICLEDATA_H__
#define __PARTICLEDATA_H__

#include "Globals.h"

class Particle;

class ParticleData
{
public:

	ParticleData(Particle* particle): particle(particle)
	{}
	~ParticleData() {}

	virtual void Execute(float dt) {}
	virtual void PrepareRender() {}

	virtual ParticleData* Copy(Particle* particle) const { return new ParticleData(particle); }

public:

	Particle* particle = nullptr;
};

#endif // !__PARTICLEDATA_H__
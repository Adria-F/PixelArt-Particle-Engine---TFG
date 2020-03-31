#ifndef __PARTICLE_BASEMOVEMENT_H__
#define __PARTICLE_BASEMOVEMENT_H__

#include "ParticleData.h"

class BaseMovement : public ParticleData
{
public:

	BaseMovement(Particle* particle): ParticleData(particle)
	{}
	~BaseMovement() {}

	void Execute(float dt);

	ParticleData* Copy(Particle* particle) const;

public:

	vec direction = vec::unitY;
	float speed = 2.0f;
};

#endif // !__PARTICLE_BASEMOVEMENT_H__
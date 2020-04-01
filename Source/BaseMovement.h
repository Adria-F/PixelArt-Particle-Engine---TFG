#ifndef __PARTICLE_BASEMOVEMENT_H__
#define __PARTICLE_BASEMOVEMENT_H__

#include "ParticleData.h"

class BaseMovement : public ParticleData
{
public:

	BaseMovement(Particle* particle);
	~BaseMovement() {}

	void Execute(float dt);

	ParticleData* Copy(Particle* particle) const;

	void SetRandomSpeed(bool random);
	void RandomizeSpeed();

public:

	vec direction = vec::unitY;
	float speed = 2.0f;

	float randSpeed1 = 0.5f;
	float randSpeed2 = 10.0f;

private:

	bool randomizeSpeed = false;
};

#endif // !__PARTICLE_BASEMOVEMENT_H__
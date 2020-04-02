#ifndef __BASEMOVEMENT_PARTICLENODE_H__
#define __BASEMOVEMENT_PARTICLENODE_H__

#include "ParticleData.h"

class BaseMovementParticleNode : public ParticleData
{
public:

	BaseMovementParticleNode(Particle* particle);
	~BaseMovementParticleNode() {}

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

#endif // !__BASEMOVEMENT_PARTICLENODE_H__
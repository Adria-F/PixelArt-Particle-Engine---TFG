#ifndef __BASEMOVEMENT_PARTICLENODE_H__
#define __BASEMOVEMENT_PARTICLENODE_H__

#include "EntityData.h"

class BaseMovementParticleNode : public EntityData
{
public:

	BaseMovementParticleNode(Particle* particle);
	~BaseMovementParticleNode() {}

	void Execute(float dt);

	EntityData* Copy(Particle* particle) const;

	void SetRandomSpeed(bool random);
	void RandomizeSpeed();

public:

	vec direction = vec::unitY;
	float speed = 2.0f;

	float randSpeed1 = 0.5f;
	float randSpeed2 = 10.0f;

	bool randomizeSpeed = false;
};

#endif // !__BASEMOVEMENT_PARTICLENODE_H__
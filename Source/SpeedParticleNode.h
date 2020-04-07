#ifndef __SPEED_PARTICLENODE_H__
#define __SPEED_PARTICLENODE_H__

#include "EntityData.h"

class SpeedParticleNode : public EntityData
{
public:

	SpeedParticleNode(Particle* particle);
	~SpeedParticleNode() {}

	void Execute(float dt);

	EntityData* Copy(Particle* particle) const;

public:

	float speed = 0.0f;
};

#endif // !__SPEED_PARTICLENODE_H__
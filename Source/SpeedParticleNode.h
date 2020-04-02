#ifndef __SPEED_PARTICLENODE_H__
#define __SPEED_PARTICLENODE_H__

#include "ParticleData.h"

class SpeedParticleNode : public ParticleData
{
public:

	SpeedParticleNode(Particle* particle);
	~SpeedParticleNode() {}

	void Execute(float dt);

	ParticleData* Copy(Particle* particle) const;

public:

	float speed = 0.0f;
};

#endif // !__SPEED_PARTICLENODE_H__
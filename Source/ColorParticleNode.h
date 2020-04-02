#ifndef __COLORPARTICLE_NODE_H__
#define __COLORPARTICLE_NODE_H__

#include "ParticleData.h"
#include "Color.h"

class ColorParticleNode : public ParticleData
{
public:

	ColorParticleNode(Particle* particle);
	~ColorParticleNode() {}

	void Execute(float dt);

	ParticleData* Copy(Particle* particle) const;

public:

	Color fixColor;
	Gradient overLifetimeColor;

	bool overLifetime = false;
};

#endif // !__COLORPARTICLE_NODE_H__
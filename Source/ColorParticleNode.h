#ifndef __COLORPARTICLE_NODE_H__
#define __COLORPARTICLE_NODE_H__

#include "EntityData.h"
#include "Color.h"

class ColorParticleNode : public EntityData
{
public:

	ColorParticleNode(Particle* particle);
	~ColorParticleNode() {}

	void Execute(float dt);

	EntityData* Copy(Particle* particle) const;

public:

	Color fixColor;
	Gradient overLifetimeColor;

	bool overLifetime = false;
};

#endif // !__COLORPARTICLE_NODE_H__
#include "MakeGlobalParticleNode.h"

MakeGlobalParticleNode::MakeGlobalParticleNode(Particle* particle): EntityData(particle)
{
}

EntityData* MakeGlobalParticleNode::Copy(Particle* particle) const
{
	MakeGlobalParticleNode* ret = new MakeGlobalParticleNode(particle);

	ret->active = active;

	return ret;
}

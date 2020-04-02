#include "MakeGlobalParticleNode.h"

MakeGlobalParticleNode::MakeGlobalParticleNode(Particle* particle): ParticleData(particle)
{
}

ParticleData* MakeGlobalParticleNode::Copy(Particle* particle) const
{
	MakeGlobalParticleNode* ret = new MakeGlobalParticleNode(particle);

	ret->active = active;

	return ret;
}

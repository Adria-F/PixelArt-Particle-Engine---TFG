#include "MakeGlobalParticleNode.h"

MakeGlobalParticleNode::MakeGlobalParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, position, size)
{
}

EntityData* MakeGlobalParticleNode::Copy(Particle* particle) const
{
	MakeGlobalParticleNode* ret = new MakeGlobalParticleNode(particle, name.c_str(), position, size);

	ret->active = active;

	return ret;
}

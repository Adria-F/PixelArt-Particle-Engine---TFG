#include "SpeedParticleNode.h"

#include "ModuleParticles.h"
#include "BaseMovementParticleNode.h"

SpeedParticleNode::SpeedParticleNode(Particle* particle): EntityData(particle)
{
}

void SpeedParticleNode::Execute(float dt)
{
	particle->baseMovement->speed = speed;
}

EntityData* SpeedParticleNode::Copy(Particle* particle) const
{
	SpeedParticleNode* ret = new SpeedParticleNode(particle);

	ret->speed = speed;

	return ret;
}
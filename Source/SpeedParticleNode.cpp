#include "SpeedParticleNode.h"

#include "ModuleParticles.h"
#include "BaseMovementParticleNode.h"

SpeedParticleNode::SpeedParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_SPEED, position, size)
{
}

void SpeedParticleNode::Execute(float dt)
{
	particle->baseMovement->speed = speed;
}

EntityData* SpeedParticleNode::Copy(Particle* particle) const
{
	SpeedParticleNode* ret = new SpeedParticleNode(particle, name.c_str(), position, size);

	ret->speed = speed;

	return ret;
}
#include "BaseMovement.h"

#include "ModuleParticles.h"
#include "BaseTransform.h"

void BaseMovement::Execute(float dt)
{
	//Change Base Transform
	particle->baseTransform->position += direction * speed*dt;
}

ParticleData* BaseMovement::Copy(Particle* particle) const
{
	BaseMovement* ret = new BaseMovement(particle);

	ret->direction = direction;
	ret->speed = speed;

	return ret;
}
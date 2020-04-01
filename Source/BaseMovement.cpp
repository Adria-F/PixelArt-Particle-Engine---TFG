#include "BaseMovement.h"

#include "ModuleParticles.h"
#include "BaseTransform.h"

BaseMovement::BaseMovement(Particle* particle) : ParticleData(particle)
{
	
}

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

	ret->randomizeSpeed = randomizeSpeed;

	ret->randSpeed1 = randSpeed1;
	ret->randSpeed2 = randSpeed2;

	if (randomizeSpeed)
		ret->RandomizeSpeed();

	return ret;
}

void BaseMovement::SetRandomSpeed(bool random)
{
	randomizeSpeed = random;
}

void BaseMovement::RandomizeSpeed()
{
	speed = Lerp(randSpeed1, randSpeed2, GET_RANDOM());
}

#include "BaseMovementParticleNode.h"

#include "ModuleParticles.h"
#include "BaseTransformParticleNode.h"

BaseMovementParticleNode::BaseMovementParticleNode(Particle* particle) : ParticleData(particle)
{
	
}

void BaseMovementParticleNode::Execute(float dt)
{
	//Change Base Transform
	particle->baseTransform->position += direction * speed*dt;
}

ParticleData* BaseMovementParticleNode::Copy(Particle* particle) const
{
	BaseMovementParticleNode* ret = new BaseMovementParticleNode(particle);

	ret->direction = direction;
	ret->speed = speed;

	ret->randomizeSpeed = randomizeSpeed;

	ret->randSpeed1 = randSpeed1;
	ret->randSpeed2 = randSpeed2;

	if (randomizeSpeed)
		ret->RandomizeSpeed();

	return ret;
}

void BaseMovementParticleNode::SetRandomSpeed(bool random)
{
	randomizeSpeed = random;
}

void BaseMovementParticleNode::RandomizeSpeed()
{
	speed = Lerp(randSpeed1, randSpeed2, GET_RANDOM());
}

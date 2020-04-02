#include "Application.h"
#include "BaseColorParticleNode.h"

#include "ModuleRender.h"
#include "Shader.h"

BaseColorParticleNode::BaseColorParticleNode(Particle* particle) : ParticleData(particle)
{
	color = White;
}

void BaseColorParticleNode::PrepareRender()
{
	App->render->defaultShader->sendColor("objectColor", color.Get());
}

ParticleData* BaseColorParticleNode::Copy(Particle* particle) const
{
	BaseColorParticleNode* ret = new BaseColorParticleNode(particle);

	ret->randomize = randomize;

	ret->color = color;
	ret->random = random;

	if (randomize)
		ret->Randomize();

	return ret;
}

void BaseColorParticleNode::SetRandom(bool random)
{
	randomize = random;
}

void BaseColorParticleNode::Randomize()
{
	color = random.GetColor(GET_RANDOM());
}
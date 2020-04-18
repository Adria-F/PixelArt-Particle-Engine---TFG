#include "Application.h"
#include "BaseColorParticleNode.h"

#include "ModuleRender.h"
#include "Shader.h"

BaseColorParticleNode::BaseColorParticleNode(Particle* particle) : EntityData(particle)
{
	color = White;

	random.SetColor(White.rgb, 0.0f);
	random.SetColor(Red.rgb, 0.5f);
	random.SetColor(Black.rgb, 1.0f);

	random.SetAlpha(0.0f, 0.0f);
	random.SetAlpha(1.0f, 1.0f);
}

void BaseColorParticleNode::PrepareRender()
{
	App->render->defaultShader->sendColor("objectColor", color.Get());
}

EntityData* BaseColorParticleNode::Copy(Particle* particle) const
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
#include "Application.h"
#include "BaseColor.h"

#include "ModuleRender.h"
#include "Shader.h"

BaseColor::BaseColor(Particle * particle) : ParticleData(particle)
{
	color = White;
}

void BaseColor::PrepareRender()
{
	App->render->defaultShader->sendColor("objectColor", color.Get());
}

ParticleData* BaseColor::Copy(Particle* particle) const
{
	BaseColor* ret = new BaseColor(particle);

	ret->color = color;
	ret->random = random;

	if (randomize)
		ret->Randomize();

	return ret;
}

void BaseColor::SetRandom(bool random)
{
	randomize = random;
}

void BaseColor::Randomize()
{
	color = random.GetColor(GET_RANDOM());
}
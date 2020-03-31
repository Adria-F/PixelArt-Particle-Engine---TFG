#include "Application.h"
#include "BaseColor.h"

#include "ModuleRender.h"
#include "Shader.h"

void BaseColor::PrepareRender()
{
	App->render->defaultShader->sendColor("objectColor", color);
}

ParticleData* BaseColor::Copy(Particle* particle) const
{
	BaseColor* ret = new BaseColor(particle);

	ret->color = color;

	return ret;
}
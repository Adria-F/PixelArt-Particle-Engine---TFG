#include "ColorParticleNode.h"

#include "ModuleParticles.h"
#include "BaseColorParticleNode.h"

ColorParticleNode::ColorParticleNode(Particle* particle): ParticleData(particle)
{
}

void ColorParticleNode::Execute(float dt)
{
	if (overLifetime)
	{
		particle->baseColor->color = overLifetimeColor.GetColor(particle->GetLifePercent());
	}
	else
	{
		particle->baseColor->color = fixColor;
	}
}

ParticleData* ColorParticleNode::Copy(Particle* particle) const
{
	ColorParticleNode* ret = new ColorParticleNode(particle);

	ret->fixColor = fixColor;
	ret->overLifetimeColor = overLifetimeColor;

	ret->overLifetime = overLifetime;

	return ret;
}

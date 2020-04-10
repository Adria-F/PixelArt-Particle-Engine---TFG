#include "ColorParticleNode.h"

#include "ModuleParticles.h"
#include "BaseColorParticleNode.h"

ColorParticleNode::ColorParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_COLOR, position, size)
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

EntityData* ColorParticleNode::Copy(Particle* particle) const
{
	ColorParticleNode* ret = new ColorParticleNode(particle, name.c_str(), position, size);

	ret->fixColor = fixColor;
	ret->overLifetimeColor = overLifetimeColor;

	ret->overLifetime = overLifetime;

	return ret;
}
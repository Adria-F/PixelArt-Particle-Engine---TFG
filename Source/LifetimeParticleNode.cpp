#include "Application.h"
#include "LifetimeParticleNode.h"

#include "ModuleParticles.h"
#include "ModuleGUI.h"

LifetimeParticleNode::LifetimeParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_LIFETIME, position, size)
{
}

EntityData* LifetimeParticleNode::Copy(Particle* particle) const
{
	LifetimeParticleNode* ret = new LifetimeParticleNode(particle, name.c_str(), position, size);

	ret->lifetime = lifetime;

	ret->randLifetime1 = randLifetime1;
	ret->randLifetime2 = randLifetime2;
	ret->random = random;

	if (random)
	{
		ret->lifetime = Lerp(randLifetime1, randLifetime2, GET_RANDOM());
	}

	particle->lifeTime = ret->lifetime;

	return ret;
}

void LifetimeParticleNode::DisplayConfig()
{
	App->gui->DrawInputFloatRandomOption("Lifetime", &lifetime, &randLifetime1, &randLifetime2, &random);
}

void LifetimeParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addFloat("lifetime", lifetime);

	node->addFloat("randLifetime1", randLifetime1);
	node->addFloat("randLifetime2", randLifetime2);
	node->addBool("random", random);
}

void LifetimeParticleNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	lifetime = nodeDef->getFloat("lifetime");

	randLifetime1 = nodeDef->getFloat("randLifetime1");
	randLifetime2 = nodeDef->getFloat("randLifetime2");
	random = nodeDef->getBool("random");
}

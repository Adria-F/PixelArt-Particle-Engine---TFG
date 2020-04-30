#include "Application.h"
#include "DeathInstantiationParticleNode.h"

#include "ModuleParticles.h"
#include "BaseTransformParticleNode.h"

DeathInstantiationParticleNode::DeathInstantiationParticleNode(Particle * particle, const char * name, float2 position, float2 size) : EntityData(particle), CanvasNode(name, PARTICLE_DEATHINSTANTIATION, position, size)
{
	NodeConnection* dataOut = new NodeConnection(this, NODE_OUTPUT, { size.x / 2.0f, 0.0f }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(dataOut);

	NodeConnection* dataIn = new NodeConnection(this, NODE_INPUT, { size.x / 2.0f, size.y }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(dataIn);

	NodeConnection* dataIn2 = new NodeConnection(this, NODE_INPUT, { size.x - NODE_PADDING * 0.25f, size.y / 2.0f }, TRIANGLE, ImGuiDir_Left);
	connections.push_back(dataIn2);
}

void DeathInstantiationParticleNode::Execute(float dt)
{
	if (particle->toDestroy && instantiateParticle != nullptr) //If toDestroy is updated then it must be connected to an emitter
	{
		Particle* instance = particle->emitter->SpawnParticle(instantiateParticle);
		//Change particle direction...
		instance->baseTransform->position = particle->baseTransform->position;
	}
}

EntityData* DeathInstantiationParticleNode::Copy(Particle* particle) const
{
	DeathInstantiationParticleNode* ret = new DeathInstantiationParticleNode(particle, name.c_str(), position, size);

	ret->instantiateParticle = instantiateParticle;

	return ret;
}
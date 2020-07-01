#include "Application.h"
#include "DeathInstantiationParticleNode.h"

#include "ModuleParticles.h"
#include "BaseTransformParticleNode.h"

DeathInstantiationParticleNode::DeathInstantiationParticleNode(Particle * particle, const char * name, float2 position, float2 size) : EntityData(particle), CanvasNode(name, PARTICLE_DEATHINSTANTIATION, position, size)
{
	update = true;

	NodeConnection* particleIn = new NodeConnection(this, NODE_INPUT, { size.x - CONNECTIONTRIANGLE_SIZE*0.5f, size.y / 2.0f }, TRIANGLE, ImGuiDir_Left);
	connections.push_back(particleIn);
}

void DeathInstantiationParticleNode::Execute(float dt)
{
	if (particle->toDestroy && instantiateParticle != nullptr) //If toDestroy is updated then it must be connected to an emitter
	{
		Particle* instance = particle->emitter->SpawnParticle(instantiateParticle);
		//Change particle direction...
		if (instance != nullptr) //It will be null if particle limit has been reach
			instance->baseTransform->position = particle->baseTransform->position;
	}
}

EntityData* DeathInstantiationParticleNode::Copy(Particle* particle) const
{
	DeathInstantiationParticleNode* ret = new DeathInstantiationParticleNode(particle, name.c_str(), position, size);

	ret->instantiateParticle = instantiateParticle;

	return ret;
}

bool DeathInstantiationParticleNode::OnConnection(NodeConnection* connection)
{
	bool ret = false;

	if (connection->type == NODE_OUTPUT && connection->node->type == PARTICLE && connection->node != particle)
	{
		instantiateParticle = (Particle*)connection->node;
		ret = true;
	}

	return ret;
}

void DeathInstantiationParticleNode::OnDisconnection(NodeConnection * connection)
{
	instantiateParticle = nullptr;
}

void DeathInstantiationParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addUint("particle", (instantiateParticle != nullptr) ? instantiateParticle->UID : 0);
}

void DeathInstantiationParticleNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	//TODO
}

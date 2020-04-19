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

	NodeConnection* dataIn2 = new NodeConnection(this, NODE_INPUT, { size.x - GRAPH_NODE_WINDOW_PADDING * 0.25f, size.y / 2.0f }, TRIANGLE, ImGuiDir_Left);
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

bool DeathInstantiationParticleNode::OnConnection(NodeConnection* connection)
{
	bool ret = false;

	CanvasNode* node = connection->node;
	if (node->type < MAX_PARTICLE_NODE)
	{
		if (node->type == PARTICLE)
		{
			Particle* part = (Particle*)node;
			if (part->particleOut == connection) //We are inputing a particle into the node
			{
				instantiateParticle = part;
			}
			else
			{
				particle = part;
				//Check connected nodes below and connect the particle
				for (std::list<NodeConnection*>::iterator it_c = connections.begin(); it_c != connections.end(); ++it_c)
				{
					if ((*it_c)->type == NODE_INPUT && (*it_c)->connected != nullptr)
					{
						(*it_c)->connected->node->OnConnection(connection);
					}
				}
			}
		}
		else if (particle != nullptr) //It is connecting to a node below
		{
			particle->OnConnection(connection);
			node->OnConnection(particle->GetDataConnection());
		}

		ret = true;
	}

	return ret;
}

void DeathInstantiationParticleNode::OnDisconnection(NodeConnection* connection)
{
	if (connection->type == NODE_INPUT) //If the other is an input, this is an output :)
	{
		particle = nullptr;
		for (std::list<NodeConnection*>::iterator it_c = connections.begin(); it_c != connections.end(); ++it_c)
		{
			if ((*it_c)->type == NODE_INPUT && (*it_c)->connected != nullptr)
			{
				(*it_c)->connected->node->OnDisconnection((*it_c));
			}
		}
	}
	else if (connection->node->type == PARTICLE)
	{
		Particle* part = (Particle*)connection->node;

		if (instantiateParticle == part)
		{
			instantiateParticle = nullptr;
		}
		else if (particle != nullptr)
		{
			particle->OnDisconnection(connection);
		}
	}
}

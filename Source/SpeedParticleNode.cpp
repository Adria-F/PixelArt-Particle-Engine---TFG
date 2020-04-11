#include "SpeedParticleNode.h"

#include "ModuleParticles.h"
#include "BaseMovementParticleNode.h"

#include "MakeGlobalParticleNode.h"

SpeedParticleNode::SpeedParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_SPEED, position, size)
{
	NodeConnection* dataOut = new NodeConnection(this, NODE_OUTPUT, { size.x / 2.0f, 0.0f }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(dataOut);

	NodeConnection* dataIn = new NodeConnection(this, NODE_INPUT, { size.x / 2.0f, size.y }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(dataIn);
}

void SpeedParticleNode::Execute(float dt)
{
	particle->baseMovement->speed = speed;
}

EntityData* SpeedParticleNode::Copy(Particle* particle) const
{
	SpeedParticleNode* ret = new SpeedParticleNode(particle, name.c_str(), position, size);

	ret->speed = speed;

	return ret;
}

bool SpeedParticleNode::OnConnection(CanvasNode* node)
{
	bool ret = false;

	if (node->type < MAX_PARTICLE_NODE)
	{
		if (node->type == PARTICLE)
		{
			particle = (Particle*)node;
			//TODO: Check connected nodes below and connect the particle
			for (std::list<NodeConnection*>::iterator it_c = connections.begin(); it_c != connections.end(); ++it_c)
			{
				if ((*it_c)->type == NODE_INPUT && (*it_c)->connected != nullptr)
				{
					(*it_c)->connected->node->OnConnection(node);
				}
			}
		}
		else if (particle != nullptr) //It is connecting to a node below
		{
			node->OnConnection(particle);
		}

		ret = true;
	}

	return ret;
}

void SpeedParticleNode::OnDisconnection(NodeConnection* connection)
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
	else if (particle != nullptr)
	{
		particle->OnDisconnection(connection);
	}
}
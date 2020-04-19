#include "Application.h"
#include "ColorParticleNode.h"

#include "ModuleParticles.h"
#include "BaseColorParticleNode.h"
#include "ModuleGUI.h"

ColorParticleNode::ColorParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_COLOR, position, size)
{
	NodeConnection* dataOut = new NodeConnection(this, NODE_OUTPUT, { size.x / 2.0f, 0.0f }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(dataOut);

	NodeConnection* dataIn = new NodeConnection(this, NODE_INPUT, { size.x / 2.0f, size.y }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(dataIn);

	fixColor = White;
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

bool ColorParticleNode::OnConnection(NodeConnection* connection)
{
	bool ret = false;

	CanvasNode* node = connection->node;
	if (node->type < MAX_PARTICLE_NODE)
	{
		if (node->type == PARTICLE)
		{
			particle = (Particle*)node;
			
			//Check connected nodes below and connect the particle
			for (std::list<NodeConnection*>::iterator it_c = connections.begin(); it_c != connections.end(); ++it_c)
			{
				if ((*it_c)->type == NODE_INPUT && (*it_c)->connected != nullptr)
				{
					(*it_c)->connected->node->OnConnection(connection);
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

void ColorParticleNode::OnDisconnection(NodeConnection* connection)
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

void ColorParticleNode::DisplayConfig()
{
	if (ImGui::RadioButton("Fix", !overLifetime))
		overLifetime = false;
	ImGui::SameLine();
	if (ImGui::RadioButton("Over Lifetime", overLifetime))
		overLifetime = true;

	if (!overLifetime)
		App->gui->DrawColorBox(fixColor);
	else
		App->gui->DrawGradientBox(overLifetimeColor);
}

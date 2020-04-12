#include "Application.h"
#include "EmissionEmitterNode.h"

#include "ModuleParticles.h"
#include "ModuleGUI.h"

EmissionEmitterNode::EmissionEmitterNode(ParticleEmitter* emitter, const char* name, float2 position, float2 size): EntityData(emitter), CanvasNode(name, EMITTER_EMISSION, position, size)
{
	lastEmit = frequency;

	NodeConnection* dataOut = new NodeConnection(this, NODE_OUTPUT, { size.x / 2.0f, 0.0f }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(dataOut);

	NodeConnection* dataIn = new NodeConnection(this, NODE_INPUT, { size.x / 2.0f, size.y }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(dataIn);
}

void EmissionEmitterNode::Execute(float dt)
{
	lastEmit += dt;

	if (lastEmit >= frequency)
	{
		lastEmit = 0.0f;
		switch (type)
		{
		case EmissionEmitterNode::CONSTANT:
			emitter->SpawnParticle();
			break;
		case EmissionEmitterNode::BURST:
			if (repeatBurst)
			{
				for (int i = 0; i < burst; ++i)
				{
					emitter->SpawnParticle();
				}
			}
			break;
		}
	}
}

void EmissionEmitterNode::Play()
{
	if (type == BURST && !repeatBurst)
	{
		for (int i = 0; i < burst; ++i)
		{
			emitter->SpawnParticle();
		}
	}
}

bool EmissionEmitterNode::OnConnection(CanvasNode* node)
{
	bool ret = false;

	if (node->type > MAX_PARTICLE_NODE && node->type < MAX_EMITTER_NODE)
	{
		if (node->type == EMITTER)
		{
			emitter = (ParticleEmitter*)node;

			//Check connected nodes below and connect the emitter
			for (std::list<NodeConnection*>::iterator it_c = connections.begin(); it_c != connections.end(); ++it_c)
			{
				if ((*it_c)->type == NODE_INPUT && (*it_c)->connected != nullptr)
				{
					(*it_c)->connected->node->OnConnection(node);
				}
			}
		}
		else if (emitter != nullptr) //It is connecting to a node below
		{
			node->OnConnection(emitter);
		}

		ret = true;
	}

	return ret;
}

void EmissionEmitterNode::OnDisconnection(NodeConnection* connection)
{
	if (connection->type == NODE_INPUT) //If the other is an input, this is an output :)
	{
		emitter = nullptr;
		for (std::list<NodeConnection*>::iterator it_c = connections.begin(); it_c != connections.end(); ++it_c)
		{
			if ((*it_c)->type == NODE_INPUT && (*it_c)->connected != nullptr)
			{
				(*it_c)->connected->node->OnDisconnection((*it_c));
			}
		}
	}
	else if (emitter != nullptr)
	{
		emitter->OnDisconnection(connection);
	}
}

void EmissionEmitterNode::DisplayConfig()
{
	ImGui::Text("Burst");
	ImGui::NewLine();

	ImGui::Text("Burst"); ImGui::SameLine(75.0f);
	App->gui->DrawInputFloat("", "##burst", &burst, 1.0f, true);
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 70.0f);  ImGui::Checkbox("Repeat", &repeatBurst);

	if (repeatBurst)
		ImGui::Text("Frequency");
	else
		ImGui::TextDisabled("Frequency");
	ImGui::SameLine(75.0f);
	
	App->gui->DrawInputFloat("", "##frequency", &frequency, 0.1f, repeatBurst);
}

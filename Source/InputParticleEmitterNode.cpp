#include "InputParticleEmitterNode.h"

#include "ModuleParticles.h"
#include "NodeGroup.h"

InputParticleEmitterNode::InputParticleEmitterNode(ParticleEmitter* emitter, const char * name, float2 position, float2 size) : EntityData(emitter), CanvasNode(name, EMITTER_INPUTPARTICLE, position, size)
{
	particleIn = new NodeConnection(this, NODE_INPUT, { size.x-CONNECTIONTRIANGLE_SIZE*0.5f, size.y/2.0f }, TRIANGLE, ImGuiDir_Left);
	connections.push_back(particleIn);
}

void InputParticleEmitterNode::Execute(float dt)
{
	if (emitter->GetTemplate() != inputParticle)
	{
		emitter->SetTemplate(inputParticle);
	}
}

bool InputParticleEmitterNode::OnConnection(NodeConnection* connection)
{
	bool ret = false;

	if (connection->type == NODE_OUTPUT && connection->node->type == PARTICLE)
	{
		inputParticle = (Particle*)connection->node;
		inputParticle->emitter = emitter;
		emitter->SetTemplate(inputParticle);
		ret = true;
	}

	return ret;
}

void InputParticleEmitterNode::OnDisconnection(NodeConnection* connection)
{
	inputParticle->emitter = nullptr;
	inputParticle = nullptr;
	emitter->SetTemplate(nullptr);
}

void InputParticleEmitterNode::SaveExtraInfo(JSON_Value * node)
{
	node->addUint("particle", (inputParticle != nullptr) ? inputParticle->UID : 0);
}

void InputParticleEmitterNode::LoadExtraInfo(JSON_Value * nodeDef)
{
	//TODO
}

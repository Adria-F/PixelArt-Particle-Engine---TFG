#include "MakeGlobalParticleNode.h"

#include "ModuleParticles.h"

MakeGlobalParticleNode::MakeGlobalParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_MAKEGLOBAL, position, size)
{
	NodeConnection* dataOut = new NodeConnection(this, NODE_OUTPUT, { size.x / 2.0f, 0.0f }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(dataOut);

	NodeConnection* dataIn = new NodeConnection(this, NODE_INPUT, { size.x / 2.0f, size.y }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(dataIn);
}

EntityData* MakeGlobalParticleNode::Copy(Particle* particle) const
{
	MakeGlobalParticleNode* ret = new MakeGlobalParticleNode(particle, name.c_str(), position, size);

	ret->active = active;

	return ret;
}

void MakeGlobalParticleNode::DisplayConfig()
{
	ImGui::Text("Active"); ImGui::SameLine();
	ImGui::Checkbox("##active", &active);
}

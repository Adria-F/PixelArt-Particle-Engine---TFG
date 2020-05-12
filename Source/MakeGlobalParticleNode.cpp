#include "MakeGlobalParticleNode.h"

#include "ModuleParticles.h"

MakeGlobalParticleNode::MakeGlobalParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_MAKEGLOBAL, position, size)
{
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

void MakeGlobalParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addBool("active", active);
}

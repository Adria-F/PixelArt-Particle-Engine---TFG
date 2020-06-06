#include "Application.h"
#include "BlendModeParticleNode.h"

#include "ModuleRender.h"

BlendModeParticleNode::BlendModeParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_BLENDMODE, position, size)
{
	blendMode = BLEND_ADDITIVE;
}

void BlendModeParticleNode::PrepareRender()
{
	App->render->SetBlendMode(blendMode);
}

EntityData* BlendModeParticleNode::Copy(Particle* particle) const
{
	BlendModeParticleNode* ret = new BlendModeParticleNode(particle, name.c_str(), position, size);

	ret->blendMode = blendMode;

	return ret;
}

void BlendModeParticleNode::DisplayConfig()
{
	static const char* types[] = { "Normal", "Additive" };
	if (ImGui::BeginCombo("Blend Mode", types[blendMode]))
	{
		if (ImGui::Selectable("Normal", blendMode == BLEND_NORMAL))
		{
			blendMode = BLEND_NORMAL;
		}
		if (ImGui::Selectable("Additive", blendMode == BLEND_ADDITIVE))
		{
			blendMode = BLEND_ADDITIVE;
		}

		ImGui::EndCombo();
	}
}

void BlendModeParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addUint("blendMode", blendMode);
}

void BlendModeParticleNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	blendMode = (blendModeType)nodeDef->getUint("blendMode");
}

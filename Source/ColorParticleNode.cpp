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

#include "Application.h"
#include "SpeedParticleNode.h"

#include "ModuleParticles.h"
#include "BaseMovementParticleNode.h"
#include "ModuleGUI.h"

SpeedParticleNode::SpeedParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_SPEED, position, size)
{
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

void SpeedParticleNode::DisplayConfig()
{
	if (ImGui::RadioButton("Fix", !overLifetime))
		overLifetime = false;
	ImGui::SameLine();
	if (ImGui::RadioButton("Over Lifetime", overLifetime))
		overLifetime = true;

	if (!overLifetime)
	{
		ImGui::Text("Speed"); ImGui::SameLine();
		App->gui->DrawInputFloat("", "##speed", &speed, 0.1f, true);
	}
	else
	{
		ImGui::Text("Curve system is work in progress");
	}
}

void SpeedParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addFloat("speed", speed);
	node->addBool("overLifetime", overLifetime);
}

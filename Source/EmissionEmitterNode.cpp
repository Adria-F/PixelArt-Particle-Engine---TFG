#include "Application.h"
#include "EmissionEmitterNode.h"

#include "ModuleParticles.h"
#include "ModuleGUI.h"

EmissionEmitterNode::EmissionEmitterNode(ParticleEmitter* emitter, const char* name, float2 position, float2 size): EntityData(emitter), CanvasNode(name, EMITTER_EMISSION, position, size)
{
	lastEmit = frequency;
}

void EmissionEmitterNode::Execute(float dt)
{
	lastEmit += dt;

	while (lastEmit >= frequency)
	{
		lastEmit -= frequency;
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
	if (emitter && emitter->GetTemplate() != nullptr && type == BURST && !repeatBurst)
	{
		for (int i = 0; i < burst; ++i)
		{
			emitter->SpawnParticle();
		}
	}
}

void EmissionEmitterNode::DisplayConfig()
{
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

void EmissionEmitterNode::SaveExtraInfo(JSON_Value* node)
{
	node->addFloat("frequency", frequency);
	node->addFloat("burst", burst);
	node->addBool("repeat", repeatBurst);
}

void EmissionEmitterNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	frequency = nodeDef->getFloat("frequency");
	burst = nodeDef->getFloat("burst");
	repeatBurst = nodeDef->getBool("repeat");
}

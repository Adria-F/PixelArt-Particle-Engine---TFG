#include "Application.h"
#include "EmissionEmitterNode.h"

#include "ModuleParticles.h"
#include "ModuleGUI.h"

EmissionEmitterNode::EmissionEmitterNode(ParticleEmitter* emitter, const char* name, float2 position, float2 size): EntityData(emitter), CanvasNode(name, EMITTER_EMISSION, position, size)
{
	update = true;
	lastEmit = frequency;
}

EmissionEmitterNode::~EmissionEmitterNode()
{
	for (std::list<Burst*>::iterator it_b = bursts.begin(); it_b != bursts.end(); ++it_b)
	{
		RELEASE((*it_b));
	}
	bursts.clear();
}

void EmissionEmitterNode::Execute(float dt)
{
	BROFILER_CATEGORY("EmissionEmitterNode", Profiler::Color::DarkViolet);

	time += dt;

	if (time >= duration)
	{
		if (loop)
			Stop(); //To reset all timers
		else
		{
			emitter->Finish();
			return;
		}			
	}

	lastEmit += dt;

	while (lastEmit >= frequency && frequency > 0.0f)
	{
		lastEmit -= frequency;

		emitter->SpawnParticle();
	}

	for (std::list<Burst*>::iterator it_b = bursts.begin(); it_b != bursts.end(); ++it_b)
	{
		if (time >= (*it_b)->start)
		{
			if ((*it_b)->repeat > 0.0f)
				(*it_b)->lastEmit += dt;

			if (((*it_b)->repeat == 0 && (*it_b)->lastEmit != -1.0f) || ((*it_b)->repeat > 0.0f && (*it_b)->lastEmit >= (*it_b)->repeat))
			{
				if ((*it_b)->repeat == 0)
					(*it_b)->lastEmit = -1.0f;
				else
					(*it_b)->lastEmit -= (*it_b)->repeat;

				for (int i = 0; i < (*it_b)->amount; i++)
					emitter->SpawnParticle();
			}
		}
	}
}

void EmissionEmitterNode::Stop()
{
	time = 0.0f;
	lastEmit = frequency;

	for (std::list<Burst*>::iterator it_b = bursts.begin(); it_b != bursts.end(); ++it_b)
	{
		(*it_b)->lastEmit = (*it_b)->repeat;
	}
}

void EmissionEmitterNode::DisplayConfig()
{
	App->gui->DrawInputFloat("Duration", &duration);
	ImGui::Checkbox("Loop", &loop);
	App->gui->DrawInputFloat("Frequency", &frequency);

	ImGui::NewLine();
	if (ImGui::CollapsingHeader("Bursts", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int count = 0;
		for (std::list<Burst*>::iterator it_b = bursts.begin(); it_b != bursts.end(); ++it_b)
		{
			if (ImGui::Selectable(("X##" + std::to_string(count)).c_str(), false, 0, { 7.0f,15.0f }))
			{
				it_b = bursts.erase(it_b);
				if (it_b == bursts.end())
					break;
			}
			ImGui::SameLine();
			if (ImGui::TreeNode(("Burst " + std::to_string(count)).c_str()))
			{
				App->gui->DrawInputFloat("Start", &(*it_b)->start);
				ImGui::DragInt("Amount", &(*it_b)->amount);
				App->gui->DrawInputFloat("Repeat", &(*it_b)->repeat);

				ImGui::TreePop();
			}
			ImGui::Separator();
			count++;
		}
		if (ImGui::Button("Add Burst"))
		{
			bursts.push_back(new Burst());
		}
	}
}

void EmissionEmitterNode::SaveExtraInfo(JSON_Value* node)
{
	node->addFloat("frequency", frequency);
	node->addFloat("duration", duration);
	node->addBool("loop", loop);

	JSON_Value* burstList = node->createValue();
	burstList->convertToArray();

	for (std::list<Burst*>::iterator it_b = bursts.begin(); it_b != bursts.end(); ++it_b)
	{
		JSON_Value* burst = burstList->createValue();
		burst->addFloat("start", (*it_b)->start);
		burst->addInt("amount", (*it_b)->amount);
		burst->addFloat("repeat", (*it_b)->repeat);

		burstList->addValue("", burst);
	}

	node->addValue("bursts", burstList);
}

void EmissionEmitterNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	frequency = nodeDef->getFloat("frequency");
	duration = nodeDef->getFloat("duration");
	loop = nodeDef->getBool("loop");

	JSON_Value* burstList = nodeDef->getValue("bursts");
	for (int i = 0; i < burstList->getRapidJSONValue()->Size(); ++i)
	{
		JSON_Value* burstDef = burstList->getValueFromArray(i);
		Burst* burst = new Burst();
		burst->start = burstDef->getFloat("start");
		burst->amount = burstDef->getInt("amount");
		burst->repeat = burstDef->getFloat("repeat");
		bursts.push_back(burst);
	}
}

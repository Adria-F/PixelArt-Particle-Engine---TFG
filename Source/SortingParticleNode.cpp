#include "Application.h"
#include "SortingParticleNode.h"

#include "ModuleCamera.h"

SortingParticleNode::SortingParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_SORTING, position, size)
{
}

EntityData* SortingParticleNode::Copy(Particle* particle) const
{
	SortingParticleNode* ret = new SortingParticleNode(particle, name.c_str(), position, size);

	ret->type = type;
	ret->priority = priority;

	return ret;
}

void SortingParticleNode::DrawExtraInfo(float2 offset, int zoom)
{
	if (App->camera->type == CAMERA_3D)
	{
		scaledSize = size * (zoom / 100.0f);
		ImGui::SetCursorScreenPos({ gridPosition.x + NODE_PADDING * (zoom / 100.0f), gridPosition.y + scaledSize.y - 17.0f*(zoom / 100.0f) });
		ImGui::TextColored({ 120.0f,0.0f,0.0f,1.0f }, "Inactive in 3D mode");
	}
}

void SortingParticleNode::DisplayConfig()
{
	if (App->camera->type == CAMERA_3D)
	{
		ImGui::Text("In 3D mode the sorting method is always by proximity");
		return;
	}

	static const char* sortTypes[] = { "Older in front", "Younger in front"};
	if (ImGui::BeginCombo("Order", sortTypes[type]))
	{
		if (ImGui::Selectable("Older in front", type == SORT_OLDER))
		{
			type = SORT_OLDER;
		}
		if (ImGui::Selectable("Younger in front", type == SORT_YOUNGER))
		{
			type = SORT_YOUNGER;
		}

		ImGui::EndCombo();
	}
	ImGui::InputInt("Priority", &priority);
}

void SortingParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addUint("sortType", type);
	node->addInt("priority", priority);
}

void SortingParticleNode::LoadeExtraInfo(JSON_Value* nodeDef)
{
	type = (sortingType)nodeDef->getUint("sortType");
	priority = nodeDef->getInt("priority");
}

#include "Application.h"
#include "CanvasNode.h"

#include "ModuleInput.h"

bool CanvasNode::Draw(float2 offset, bool hovered, bool selected)
{
	bool isHovered = false;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImGui::PushID(UID);

	//Node box
	ImU32 borderColor = IM_COL32(100, 100, 100, 255);
	ImU32 backgroundColor = IM_COL32(45, 45, 45, 255);
	float thickness = 1.0f;

	if (hovered)
	{
		borderColor = IM_COL32(150, 150, 100, 255);
	}
	if (selected)
	{
		borderColor = IM_COL32(255, 255, 75, 255);
		backgroundColor = IM_COL32(55, 55, 55, 255);
		thickness = 2.0f;
	}

	gridPosition = position + offset;

	draw_list->AddRectFilled({ gridPosition.x, gridPosition.y }, { gridPosition.x + size.x, gridPosition.y + size.y }, backgroundColor, 4.0f);
	draw_list->AddRect({ gridPosition.x, gridPosition.y }, { gridPosition.x + size.x, gridPosition.y + size.y }, borderColor, 4.0f, 15, thickness);

	ImGui::SetCursorScreenPos({ gridPosition.x + GRAPH_NODE_WINDOW_PADDING, gridPosition.y + GRAPH_NODE_WINDOW_PADDING });
	ImGui::BeginGroup();
	ImGui::Text(name.c_str());

	//Node selection & linking
	ImGui::SetCursorScreenPos({ gridPosition.x, gridPosition.y });
	ImGui::InvisibleButton("node", { size.x, size.y });
	if (ImGui::IsItemHovered() /*&& !hoveringConfigMenu*/)
	{
		isHovered = true;

		if (ImGui::IsMouseClicked(1))
		{
			ImGui::OpenPopup("##node info");
		}
	}
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 5,5 });
	if (ImGui::BeginPopup("##node info"))
	{
		if (ImGui::Selectable("Remove"))
		{
			toDelete = true;
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	if (selected && App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		toDelete = true;
	}
	
	//Dragging
	if (ImGui::IsItemClicked() /*&& !hoveringConfigMenu*/)
	{		
		clickOffset = { ImGui::GetMousePos().x - position.x, ImGui::GetMousePos().y - position.y };
	}
	if (selected && ImGui::IsMouseDragging(0))
	{
		position = { ImGui::GetMousePos().x - clickOffset.x, ImGui::GetMousePos().y - clickOffset.y };
	}

	ImGui::EndGroup();
	ImGui::PopID();

	return isHovered;
}

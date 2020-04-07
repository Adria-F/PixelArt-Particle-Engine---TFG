#include "CanvasNode.h"

bool CanvasNode::Draw(bool hovered, bool selected)
{
	bool isHovered = false;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

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

	draw_list->AddRectFilled({ position.x, position.y }, { position.x + size.x, position.y + size.y }, backgroundColor, 4.0f);
	draw_list->AddRect({ position.x, position.y }, { position.x + size.x, position.y + size.y }, borderColor, 4.0f, 15, thickness);

	//Node content
	//ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 5,5 });
	//ImGui::SetCursorScreenPos({ node->gridPos.x + node->size.x - 70, node->gridPos.y + GRAPH_NODE_WINDOW_PADDING });
	//ImGui::Text(name.c_str());
	//ImGui::PopStyleVar();

	ImGui::SetCursorScreenPos({ position.x + GRAPH_NODE_WINDOW_PADDING, position.y + GRAPH_NODE_WINDOW_PADDING });
	ImGui::BeginGroup();
	ImGui::Text(name.c_str());

	//Node selection & linking
	ImGui::SetCursorScreenPos({ position.x, position.y });
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
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
	if (ImGui::IsItemClicked() /*&& !hoveringConfigMenu*/)
	{
		//Dragging
		/*dragging_node = node;
		clickOffset = { ImGui::GetMousePos().x - offset.x - node->pos.x, ImGui::GetMousePos().y - offset.y - node->pos.y };*/
	}

	ImGui::EndGroup();

	return isHovered;
}

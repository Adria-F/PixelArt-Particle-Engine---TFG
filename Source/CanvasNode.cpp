#include "Application.h"
#include "CanvasNode.h"

#include "ModuleInput.h"
#include "ModuleGUI.h"

void CanvasNode::Draw(float2 offset, float zoom, bool hovered, bool selected)
{
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

	gridPosition = position*(zoom/100.0f) + offset;
	float2 scaledSize = size * (zoom/100.0f);

	draw_list->AddRectFilled({ gridPosition.x, gridPosition.y }, { gridPosition.x + scaledSize.x, gridPosition.y + scaledSize.y }, backgroundColor, 4.0f);
	draw_list->AddRect({ gridPosition.x, gridPosition.y }, { gridPosition.x + scaledSize.x, gridPosition.y + scaledSize.y }, borderColor, 4.0f, 15, thickness);

	//Draw scalable text
	ImFont* scaledFont = App->gui->GetFont(zoom);
	if (scaledFont != nullptr)
		ImGui::PushFont(scaledFont);

	ImGui::SetCursorScreenPos({ gridPosition.x + GRAPH_NODE_WINDOW_PADDING*(zoom / 100.0f), gridPosition.y + GRAPH_NODE_WINDOW_PADDING*(zoom / 100.0f) });
	ImGui::BeginGroup();
	ImGui::Text(name.c_str());

	if (scaledFont != nullptr)
		ImGui::PopFont();

	ImGui::EndGroup();
}

bool CanvasNode::Logic(float2 offset, float zoom, bool selected)
{
	bool isHovered = false;

	ImGui::PushID(UID);
	float2 scaledSize = size * (zoom / 100.0f);

	//Node hovering //& linking
	ImGui::SetCursorScreenPos({ gridPosition.x, gridPosition.y });
	ImGui::BeginGroup();
	ImGui::InvisibleButton("node", { scaledSize.x, scaledSize.y });
	if (ImGui::IsItemHovered() /*&& !hoveringConfigMenu*/)
	{
		isHovered = true;

		if (ImGui::IsMouseClicked(1))
		{
			ImGui::OpenPopup("##node info");
		}
	}

	//Right click pop up
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

	//Delete selected node with 'Supr'
	if (selected && App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		toDelete = true;
	}

	//Dragging
	if (ImGui::IsItemClicked() /*&& !hoveringConfigMenu*/)
	{
		clickOffset = { ImGui::GetMousePos().x - position.x*(zoom / 100.0f), ImGui::GetMousePos().y - position.y*(zoom / 100.0f) };
	}
	if (selected && ImGui::IsMouseDragging(0))
	{
		position = { ImGui::GetMousePos().x - clickOffset.x, ImGui::GetMousePos().y - clickOffset.y };
		position /= (zoom / 100.0f);
	}

	ImGui::EndGroup();
	ImGui::PopID();

	return isHovered;
}

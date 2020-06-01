#include "Application.h"
#include "PanelNodeCanvas.h"

#include "CanvasNode.h"
#include "ModuleNodeCanvas.h"
#include "ModuleInput.h"
#include "ModuleGUI.h"

//Include ndoes
#include "ModuleParticles.h"

PanelNodeCanvas::PanelNodeCanvas(const char* name): Panel(name)
{
	showGrid = false;
}

void PanelNodeCanvas::SetFlags()
{
	flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
}

void PanelNodeCanvas::DrawContent()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(45, 45, 50, 255));

	ImVec2 cursorPos = ImGui::GetCursorScreenPos();

	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushItemWidth(120.0f);

	//Display current zoom
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f);
	ImGui::SetCursorScreenPos(cursorPos);
	ImGui::Text("Zoom x %.1f", zoom / 100.0f);
	ImGui::PopStyleVar();

	App->nodeCanvas->canvasFocused = ImGui::IsWindowFocused();

	//Manage zoom
	float wheelDelta = ImGui::GetIO().MouseWheel;
	if (wheelDelta != 0.0f && ImGui::IsWindowHovered()) //There is scroll
	{
		int newZoom = zoom + wheelDelta * 100 * ZOOM_STEPS;
		newZoom = Clamp(newZoom, (int)(MIN_ZOOM*100), (int)(MAX_ZOOM*100));
		float2 mousePos = { ImGui::GetMousePos().x- ImGui::GetWindowPos().x, ImGui::GetMousePos().y- ImGui::GetWindowPos().y };
		float2 gridPoint = mousePos - scrolling;
		gridPoint *= (float)newZoom/(float)zoom;
		scrolling = mousePos - gridPoint;
		
		zoom = newZoom;
	}

	float2 offset = { ImGui::GetCursorScreenPos().x + scrolling.x, ImGui::GetCursorScreenPos().y + scrolling.y };
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	// Display grid
	if (showGrid)
	{
		ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
		float GRID_SZ = 64.0f*(zoom/100.0f);
		ImVec2 win_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetWindowSize();
		for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
			draw_list->AddLine(ImVec2(x + win_pos.x, win_pos.y), ImVec2(x + win_pos.x, canvas_sz.y + win_pos.y), GRID_COLOR);
		for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
			draw_list->AddLine(ImVec2(win_pos.x, y + win_pos.y), ImVec2(canvas_sz.x + win_pos.x, y + win_pos.y), GRID_COLOR);
	}	

	//Set cursor
	if (ImGui::IsWindowHovered() && (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT || ImGui::IsMouseDown(2)))
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
	}
	else if (App->nodeCanvas->hoveredNode != nullptr)
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}

	//Drag window
	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(2))
			midClicked = true;
	}
	if (!ImGui::IsMouseDown(2))
		midClicked = false;
	if ((midClicked && ImGui::IsMouseDragging(2, 0.0f) || (ImGui::IsMouseDown(0) && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)))
		scrolling = { scrolling.x + ImGui::GetIO().MouseDelta.x, scrolling.y + ImGui::GetIO().MouseDelta.y };
	
	//Draw content
	CanvasNode* newHoveredNode = nullptr;
	CanvasNode* newSelectedNode = nullptr;
	for (std::list<CanvasNode*>::iterator it_n = App->nodeCanvas->nodes.begin(); it_n != App->nodeCanvas->nodes.end(); ++it_n) //Draw in ascending order
	{
		(*it_n)->Draw(offset, zoom);
	}
	for (std::list<CanvasNode*>::reverse_iterator it_n = App->nodeCanvas->nodes.rbegin(); it_n != App->nodeCanvas->nodes.rend(); ++it_n) //Calculate interaction logic in reverse (because ImGui takes first drawn as top)
	{
		if ((*it_n)->Logic(offset, zoom))
		{
			newHoveredNode = (*it_n);
			if (ImGui::IsMouseClicked(0))
			{
				newSelectedNode = (*it_n);
			}
		}
	}
	if (newHoveredNode != nullptr)
		App->nodeCanvas->newHoveredNode = newHoveredNode;
	if (newSelectedNode && newSelectedNode != App->nodeCanvas->selectedNode)
	{
		//Move the selected node to the end of the list to draw it in front
		App->nodeCanvas->nodes.remove(newSelectedNode);
		App->nodeCanvas->nodes.push_back(newSelectedNode);
		App->nodeCanvas->selectedNode = newSelectedNode;
		App->nodeCanvas->selectedConnection = nullptr; //Deselect connection
	}

	//Click to add nodes
	static float2 mousePos = { 0.0f,0.0f };
	if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			App->nodeCanvas->selectedNode = nullptr; //Deselect node
		}
		else if (ImGui::IsMouseClicked(1))
		{
			mousePos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y }; //Store mousePosition at that moment
			ImGui::OpenPopup("##addNode");
		}
	}
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 5,5 });
	if (ImGui::BeginPopup("##addNode"))
	{
		CanvasNode* createdNode = App->nodeCanvas->DrawNodeList((mousePos - offset) / (zoom / 100.0f), CANVAS);
		if (createdNode != nullptr)
			App->nodeCanvas->nodes.push_back(createdNode);

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}
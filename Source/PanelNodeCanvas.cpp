#include "Application.h"
#include "PanelNodeCanvas.h"

#include "CanvasNode.h"
#include "ModuleNodeCanvas.h"
#include "ModuleInput.h"
#include "ModuleGUI.h"

//Include ndoes
#include "ModuleParticles.h"
#include "ColorParticleNode.h"
#include "SpeedParticleNode.h"
#include "MakeGlobalParticleNode.h"
#include "EmissionEmitterNode.h"

PanelNodeCanvas::PanelNodeCanvas(const char* name): Panel(name)
{

}

void PanelNodeCanvas::SetFlags()
{
	flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
}

void PanelNodeCanvas::DrawContent()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 55, 200));

	ImVec2 cursorPos = ImGui::GetCursorPos();
	ImGui::Text("Zoom x %.1f", zoom/100.0f);
	ImGui::SetCursorPos(cursorPos);

	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushItemWidth(120.0f);

	//Manage zoom
	float wheelDelta = ImGui::GetIO().MouseWheel;
	if (wheelDelta != 0.0f) //There is scroll
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
	else if (hoveredNode != nullptr)
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}

	//Drag window
	if (ImGui::IsWindowHovered() && (ImGui::IsMouseDragging(2, 0.0f) || (ImGui::IsMouseDown(0) && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)))
		scrolling = { scrolling.x + ImGui::GetIO().MouseDelta.x, scrolling.y + ImGui::GetIO().MouseDelta.y };

	//Draw content
	CanvasNode* newHoveredNode = nullptr;
	CanvasNode* newSelectedNode = nullptr;
	for (std::list<CanvasNode*>::iterator it_n = App->nodeCanvas->nodes.begin(); it_n != App->nodeCanvas->nodes.end(); ++it_n) //Draw in ascending order
	{
		(*it_n)->Draw(offset, zoom, hoveredNode == (*it_n), selectedNode == (*it_n));
	}
	for (std::list<CanvasNode*>::reverse_iterator it_n = App->nodeCanvas->nodes.rbegin(); it_n != App->nodeCanvas->nodes.rend(); ++it_n) //Calculate interaction logic in reverse (because ImGui takes first drawn as top)
	{
		if ((*it_n)->Logic(offset, zoom,selectedNode == (*it_n)) && newHoveredNode == nullptr)
		{
			newHoveredNode = (*it_n);
			if (ImGui::IsMouseClicked(0))
			{
				newSelectedNode = (*it_n);
			}
		}
	}
	hoveredNode = newHoveredNode;
	if (newSelectedNode && newSelectedNode != selectedNode)
	{
		//Move the selected node to the end of the list to draw it in front
		App->nodeCanvas->nodes.remove(newSelectedNode);
		App->nodeCanvas->nodes.push_back(newSelectedNode);
		selectedNode = newSelectedNode;
	}

	//Click to add nodes
	static float2 mousePos = { 0.0f,0.0f };
	if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			selectedNode = nullptr;
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
		DrawNodeList((mousePos - offset) / (zoom / 100.0f), zoom);

		ImGui::EndPopup();
	}
	else if (((std::string)node_filter).size() > 0)
	{
		memcpy(&node_filter, "\0", 1);
	}
	ImGui::PopStyleVar();

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

void PanelNodeCanvas::DrawNodeList(float2 spawnPos, int zoom)
{
	static std::vector<std::string> nodes = { "Particle", "Emitter", "Color", "Speed", "Make Global", "Emission" };
	
	//Filter
	ImGui::PushItemWidth(100.0f);
	ImGui::InputText("##filter", node_filter, 64);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Selectable("X", false, ImGuiSelectableFlags_DontClosePopups, { 8,13 }))
	{
		memcpy(&node_filter, "\0", 1);
	}
	std::string filter = node_filter;

	int choice = -1;
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (filter.size() == 0 || nodes[i].find(filter) != std::string::npos)
		{
			if (ImGui::Selectable(nodes[i].c_str()))
			{
				choice = i;
				break;
			}
		}
	}

	if (choice != -1)
	{
		ImFont* scaledFont = App->gui->GetFont(zoom);
		if (scaledFont != nullptr)
			ImGui::PushFont(scaledFont);

		float textSize = ImGui::CalcTextSize(nodes[choice].c_str()).x + GRAPH_NODE_WINDOW_PADDING * (zoom / 100.0f)*2.0f;

		if (scaledFont != nullptr)
			ImGui::PopFont();

		CanvasNode* node = nullptr;
		switch (choice)
		{
		case 0: //Particle
			node = new Particle(nodes[choice].c_str(), spawnPos, { textSize,45 });
			break;
		case 1: //Emitter
			node = new ParticleEmitter(nodes[choice].c_str(), spawnPos, { textSize,45 });
			break;
		case 2: //Color
			node = new ColorParticleNode(nullptr, nodes[choice].c_str(), spawnPos, { textSize,45 });
			break;
		case 3: //Speed
			node = new SpeedParticleNode(nullptr, nodes[choice].c_str(), spawnPos, { textSize,45 });
			break;
		case 4: //Make Global
			node = new MakeGlobalParticleNode(nullptr, nodes[choice].c_str(), spawnPos, { textSize,45 });
			break;
		case 5: //Emission
			node = new EmissionEmitterNode(nullptr, nodes[choice].c_str(), spawnPos, { textSize,45 });
			break;
		}

		if (node != nullptr)
		{
			App->nodeCanvas->nodes.push_back(node);
		}
	}
}
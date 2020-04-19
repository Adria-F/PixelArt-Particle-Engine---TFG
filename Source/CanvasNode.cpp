#include "Application.h"
#include "CanvasNode.h"

#include "ModuleInput.h"
#include "ModuleGUI.h"
#include "ModuleNodeCanvas.h"

inline float DistanceToLine(float2 v, float2 w, float2 p);
inline bool PointInsideRect(float2 A, float2 B, float2 point);

CanvasNode::~CanvasNode()
{
	for (std::list<NodeConnection*>::iterator it_c = connections.begin(); it_c != connections.end(); ++it_c)
	{
		RELEASE((*it_c));
	}
	connections.clear();
}

void CanvasNode::Draw(float2 offset, int zoom, bool hovered, bool selected)
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

	for (std::list<NodeConnection*>::iterator it_c = connections.begin(); it_c != connections.end(); ++it_c)
	{
		(*it_c)->Draw(zoom);
	}

	ImGui::EndGroup();
}

bool CanvasNode::Logic(float2 offset, int zoom, bool selected)
{
	bool isHovered = false;

	bool hoveringConnection = false;
	for (std::list<NodeConnection*>::iterator it_c = connections.begin(); it_c != connections.end(); ++it_c)
	{
		if ((*it_c)->Logic(zoom))
		{
			hoveringConnection = true;
			break; //We can be hovering only one at once (they must not overlap)
		}
	}

	ImGui::PushID(UID);
	float2 scaledSize = size * (zoom / 100.0f);

	//Node hovering //& linking
	ImGui::SetCursorScreenPos({ gridPosition.x, gridPosition.y });
	ImGui::BeginGroup();
	ImGui::InvisibleButton("node", { scaledSize.x, scaledSize.y });
	if (ImGui::IsItemHovered() /*&& !hoveringConfigMenu*/ && !hoveringConnection)
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
	if (selected && App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && ImGui::IsWindowFocused())
	{
		toDelete = true;
	}

	//Dragging
	static bool dragging = false;
	if (ImGui::IsItemClicked() /*&& !hoveringConfigMenu*/)
	{
		clickOffset = { ImGui::GetMousePos().x - position.x*(zoom / 100.0f), ImGui::GetMousePos().y - position.y*(zoom / 100.0f) };
		dragging = true;
	}
	if (dragging && ImGui::IsMouseReleased(0))
	{
		dragging = false;
	}
	if (selected && dragging && ImGui::IsMouseDragging(0))
	{
		position = { ImGui::GetMousePos().x - clickOffset.x, ImGui::GetMousePos().y - clickOffset.y };
		position /= (zoom / 100.0f);
	}

	ImGui::EndGroup();
	ImGui::PopID();

	return isHovered;
}

NodeConnection::NodeConnection(CanvasNode* node, connectionType type, float2 position, shapeType shape, ImGuiDir_ direction): node(node), type(type), localPosition(position), shape(shape), direction(direction)
{
}

NodeConnection::~NodeConnection()
{
	if (connected != nullptr)
	{
		if (App->nodeCanvas->selectedConnection == this || App->nodeCanvas->selectedConnection == connected)
			App->nodeCanvas->selectedConnection = nullptr;
		connected->Disconnect();
	}
}

void NodeConnection::Draw(int zoom)
{
	gridPosition = node->gridPosition + localPosition * (zoom / 100.0f);
	switch (shape)
	{
	case TRIANGLE:		
		DrawTriangle(zoom / 100.0f);
		break;
	case CIRCLE:
		break;
	}


	//Draw connection lines
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	if (connecting)
	{
		ImU32 color = IM_COL32(200, 200, 100, 255);
		ImVec2 pointA = { gridPosition.x, gridPosition.y };
		ImVec2 pointB = ImGui::GetMousePos();

		draw_list->AddLine(pointA, pointB, color, 3.0f);
	}
	if (connected != nullptr && type == NODE_OUTPUT)
	{
		ImU32 color = IM_COL32(200, 200, 100, 255);
		float thickness = 3.0f;
		ImVec2 pointA = { gridPosition.x, gridPosition.y };
		ImVec2 pointB = { connected->gridPosition.x, connected->gridPosition.y };

		if (App->nodeCanvas->selectedConnection == this)
		{
			color = IM_COL32(240, 240, 175, 255);
			thickness = 5.0f;
		}

		draw_list->AddLine(pointA, pointB, color, thickness);
	}
}

bool NodeConnection::Logic(int zoom)
{
	ImGui::SetCursorScreenPos({ gridPosition.x - CONNECTIONTRIANGLE_SIZE / 2.0f, gridPosition.y - CONNECTIONTRIANGLE_SIZE / 2.0f });
	ImGui::InvisibleButton("##particleIN", { CONNECTIONTRIANGLE_SIZE, CONNECTIONTRIANGLE_SIZE });

	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseDown(0))
			state = CLICKED;
		else
			state = HOVERED;
	}
	else
		state = IDLE;

	if (ImGui::IsItemClicked(0) && connected == nullptr)
	{
		connecting = true;
		App->nodeCanvas->StartConnection(this);
	}

	if (ImGui::IsMouseReleased(0) && connected == nullptr)
	{
		if (connecting)
		{
			connecting = false;
			App->nodeCanvas->StopConnection();
		}
		else if (ImGui::IsItemHovered())
		{
			App->nodeCanvas->RequestConnection(this);
		}
	}

	//Handle connection line logic
	if (ImGui::IsMouseClicked(0) && connected != nullptr && type == NODE_OUTPUT)
	{
		float2 mousePos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
		float2 pointA = { gridPosition.x, gridPosition.y };
		float2 pointB = { connected->gridPosition.x, connected->gridPosition.y };

		bool clicked = false;
		if (!ImGui::IsAnyItemHovered() && PointInsideRect(pointA, pointB, mousePos))
		{
			float distance = DistanceToLine(pointA, pointB, mousePos);
			if (distance <= 15.0f)
			{
				App->nodeCanvas->selectedConnection = this;
				clicked = true;
			}
		}
		if (!clicked && App->nodeCanvas->selectedConnection == this)
		{
			App->nodeCanvas->selectedConnection = nullptr;
		}
	}
	if (App->nodeCanvas->selectedConnection == this && App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && ImGui::IsWindowFocused())
	{
		connected->Disconnect();
		Disconnect();
		App->nodeCanvas->selectedConnection = nullptr;
	}

	return (state == HOVERED || state == CLICKED);
}

void NodeConnection::DrawTriangle(float scale)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 pointA = { gridPosition.x + ((direction == ImGuiDir_Left) ? -(CONNECTIONTRIANGLE_SIZE*scale) / 2.0f : ((direction == ImGuiDir_Right) ? (CONNECTIONTRIANGLE_SIZE*scale) / 2.0f : 0.0f)), gridPosition.y + ((direction == ImGuiDir_Up) ? -(CONNECTIONTRIANGLE_SIZE*scale) / 2.0f : ((direction == ImGuiDir_Down) ? (CONNECTIONTRIANGLE_SIZE*scale) / 2.0f : 0.0f)) };
	ImVec2 pointB = { gridPosition.x + ((direction == ImGuiDir_Left || direction == ImGuiDir_Down) ? (CONNECTIONTRIANGLE_SIZE*scale) / 2.0f : -(CONNECTIONTRIANGLE_SIZE*scale) / 2.0f), gridPosition.y + ((direction == ImGuiDir_Left || direction == ImGuiDir_Up) ? (CONNECTIONTRIANGLE_SIZE*scale) / 2.0f : -(CONNECTIONTRIANGLE_SIZE*scale) / 2.0f) };
	ImVec2 pointC = { gridPosition.x + ((direction == ImGuiDir_Left || direction == ImGuiDir_Up) ? (CONNECTIONTRIANGLE_SIZE*scale) / 2.0f : -(CONNECTIONTRIANGLE_SIZE*scale) / 2.0f), gridPosition.y + ((direction == ImGuiDir_Left || direction == ImGuiDir_Down) ? -(CONNECTIONTRIANGLE_SIZE*scale) / 2.0f : (CONNECTIONTRIANGLE_SIZE*scale) / 2.0f) };

	float thickness = CONNECTION_OUTLINE_THICKNESS * scale;

	ImU32 fillColor = IM_COL32(100, 100, 105, 255);
	ImU32 outlineColor = IM_COL32(200, 200, 205, 255);
	switch (state)
	{
	case HOVERED:
		outlineColor = IM_COL32(255, 255, 75, 255);
		break;
	case CLICKED:
		outlineColor = IM_COL32(255, 255, 75, 255);
		thickness *= 2.0f;
		break;
	}

	if (connecting || connected != nullptr)
	{
		fillColor = IM_COL32(200, 200, 100, 255);
	}

	if (App->nodeCanvas->selectedConnection != nullptr && (App->nodeCanvas->selectedConnection == this || App->nodeCanvas->selectedConnection->connected == this))
		fillColor = IM_COL32(240, 240, 175, 255);

	draw_list->AddTriangleFilled(pointA, pointB, pointC, fillColor);
	draw_list->AddTriangle(pointA, pointB, pointC, outlineColor, thickness);
}

void NodeConnection::SetConnection(NodeConnection* node)
{
	if (this->node->OnConnection(node))
		connected = node;	
}

void NodeConnection::Disconnect()
{
	this->node->OnDisconnection(connected);
	connected = nullptr;
}

inline float DistanceToLine(float2 v, float2 w, float2 p)
{
	const float l2 = w.DistanceSq(v);
	if (l2 == 0.0) return p.DistanceSq(v);   // v == w case

	float2 v1 = p - v;
	float2 v2 = w - v;
	const float t = max(0, min(1, v1.Dot(v2) / l2));
	const float2 projection = v + t * (w - v);
	return p.DistanceSq(projection);
}

inline bool PointInsideRect(float2 A, float2 B, float2 point)
{
	float2 bottomLeft = { Min(A.x, B.x), Max(A.y, B.y) };
	float2 topRight = { Max(A.x, B.x), Min(A.y, B.y) };

	return point.x > bottomLeft.x && point.x < topRight.x && point.y < bottomLeft.y && point.y > topRight.y;
}
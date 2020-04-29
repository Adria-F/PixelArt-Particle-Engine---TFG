#include "Application.h"
#include "NodeGroup.h"

#include "ModuleGUI.h"
#include "ModuleNodeCanvas.h"

NodeGroup::NodeGroup(const char* name, float2 position) : CanvasNode(name, NODE_HOLDER, position)
{
	interactable = false;
}

NodeGroup::~NodeGroup()
{
	for (std::list<NodeBox*>::iterator it_b = boxes.begin(); it_b != boxes.end(); ++it_b)
	{
		RELEASE((*it_b));
	}
	boxes.clear();
}

void NodeGroup::Draw(float2 offset, int zoom)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	//Node box
	ImU32 borderColor = IM_COL32(100, 100, 100, 255);
	ImU32 backgroundColor = IM_COL32(45, 45, 50, 255);
	float thickness = 5.0f;

	CalcRect();
	gridPosition = position * (zoom / 100.0f) + offset;
	float2 scaledSize = size * (zoom / 100.0f);

	draw_list->AddRectFilled({ gridPosition.x, gridPosition.y }, { gridPosition.x + scaledSize.x, gridPosition.y + scaledSize.y }, backgroundColor, 4.0f);
	draw_list->AddRect({ gridPosition.x, gridPosition.y }, { gridPosition.x + scaledSize.x, gridPosition.y + scaledSize.y }, borderColor, 4.0f, 15, thickness);

	//Draw scalable text
	ImFont* scaledFont = App->gui->GetFont(zoom, CANVAS_FONT_SIZE);
	if (scaledFont != nullptr)
		ImGui::PushFont(scaledFont);

	ImGui::SetCursorScreenPos({ gridPosition.x + NODE_PADDING * (zoom / 100.0f), gridPosition.y + NODE_PADDING * (zoom / 100.0f) });
	ImGui::BeginGroup();
	ImGui::Text(name.c_str());

	if (scaledFont != nullptr)
		ImGui::PopFont();

	//Draw boxes
	for (std::list<NodeBox*>::iterator it_b = boxes.begin(); it_b != boxes.end(); ++it_b)
	{
		(*it_b)->Draw(offset, zoom);
	}

	ImGui::EndGroup();
}

bool NodeGroup::ElementLogic(float2 offset, int zoom)
{
	CanvasNode* newHoveredNode = nullptr;
	CanvasNode* newSelectedNode = nullptr;
	for (std::list<NodeBox*>::iterator it_b = boxes.begin(); it_b != boxes.end(); ++it_b)
	{
		if ((*it_b)->toDelete)
		{
			if (App->nodeCanvas->hoveredNode == (*it_b))
				App->nodeCanvas->hoveredNode = nullptr;
			if (App->nodeCanvas->selectedNode == (*it_b))
				App->nodeCanvas->selectedNode = nullptr;

			RELEASE((*it_b));
			it_b = boxes.erase(it_b);

			if (it_b == boxes.end())
				break;
		}

		if ((*it_b)->Logic(offset, zoom))
		{
			newHoveredNode = (*it_b);
			if (ImGui::IsMouseClicked(0))
			{
				newSelectedNode = (*it_b);
			}
		}
	}
	App->nodeCanvas->hoveredNode = newHoveredNode;

	if (newSelectedNode && newSelectedNode != App->nodeCanvas->selectedNode)
	{
		//Move the selected node to the end of the list to draw it in front
		boxes.remove((NodeBox*)newSelectedNode);
		boxes.push_back((NodeBox*)newSelectedNode);
		App->nodeCanvas->selectedNode = newSelectedNode;
		App->nodeCanvas->selectedConnection = nullptr; //Deselect connection
	}

	return newHoveredNode != nullptr;
}

NodeBox* NodeGroup::AddNodeBox(const char* name, nodeType type)
{
	float2 boxPos = position;
	boxPos.x += BOX_CONTAINER_MARGIN;
	boxPos.y += size.y - BOX_CONTAINER_MARGIN + NODE_BOX_SEPARATION;
	
	NodeBox* ret = new NodeBox(name, type, boxPos, { NODE_DEFAULT_WIDTH + NODE_BOX_PADDING * 2.0f, NODE_BOX_MIN_HEIGHT }, this);
	if (boxes.size() > 0)
	{
		boxes.back()->bottomConnection->SetConnection(ret->topConnection);
		ret->topConnection->SetConnection(boxes.back()->bottomConnection); //This will already insert the new node
	}
	else
	{
		InsertBox(ret);
	}
	
	CalcRect();
	return ret;
}

void NodeGroup::InsertBox(NodeBox* box)
{
	boxes.push_back(box);

	for (std::list<CanvasNode*>::iterator it_n = box->nodes.begin(); it_n != box->nodes.end(); ++it_n)
	{
		OnNodeAdded((*it_n));
	}
}

void NodeGroup::RemoveBox(NodeBox* box, bool keepInList)
{
	if (!keepInList)
		boxes.remove(box);

	for (std::list<CanvasNode*>::iterator it_n = box->nodes.begin(); it_n != box->nodes.end(); ++it_n)
	{
		OnNodeRemoved((*it_n));
	}
}

void NodeGroup::CalcRect()
{
	float2 topLeft = float2::inf;
	float2 bottomRight = -float2::inf;
	for (std::list<NodeBox*>::iterator it_b = boxes.begin(); it_b != boxes.end(); ++it_b)
	{
		if ((*it_b)->position.x < topLeft.x)
			topLeft.x = (*it_b)->position.x;
		if ((*it_b)->position.y < topLeft.y)
			topLeft.y = (*it_b)->position.y;
		if ((*it_b)->position.x + (*it_b)->size.x > bottomRight.x)
			bottomRight.x = (*it_b)->position.x + (*it_b)->size.x;
		if ((*it_b)->position.y + (*it_b)->size.y > bottomRight.y)
			bottomRight.y = (*it_b)->position.y + (*it_b)->size.y;
	}

	topLeft -= {BOX_CONTAINER_MARGIN, BOX_CONTAINER_MARGIN};
	bottomRight += {BOX_CONTAINER_MARGIN, BOX_CONTAINER_MARGIN};

	position = topLeft;
	size = bottomRight - topLeft;
}

NodeBox::NodeBox(const char* name, nodeType type, float2 position, float2 size, NodeGroup* parentGroup) : CanvasNode(name, type, position, size), parentGroup(parentGroup)
{
	topConnection = new NodeConnection(this, NODE_INPUT, { size.x/2.0f, 0.0f }, CIRCLE);
	connections.push_back(topConnection);

	bottomConnection = new NodeConnection(this, NODE_OUTPUT, { size.x / 2.0f, size.y }, CIRCLE);
	connections.push_back(bottomConnection);
}

NodeBox::~NodeBox()
{
	if (parentGroup)
		parentGroup->RemoveBox(this, true);

	for (std::list<CanvasNode*>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
	{
		RELEASE((*it_n));
	}
	nodes.clear();
}

void NodeBox::Draw(float2 offset, int zoom)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	//Node box
	ImU32 borderColor = IM_COL32(100, 100, 100, 255);
	ImU32 backgroundColor = IM_COL32(45, 45, 45, 255);
	float thickness = 1.0f;

	if (App->nodeCanvas->hoveredNode == this || hoveringElement)
	{
		borderColor = IM_COL32(150, 150, 100, 255);
	}
	if (App->nodeCanvas->selectedNode == this)
	{
		borderColor = IM_COL32(255, 255, 75, 255);
		backgroundColor = IM_COL32(55, 55, 55, 255);
		thickness = 2.0f;
	}

	gridPosition = position* (zoom / 100.0f) + offset;
	float2 scaledSize = size * (zoom / 100.0f);

	draw_list->AddRectFilled({ gridPosition.x, gridPosition.y }, { gridPosition.x + scaledSize.x, gridPosition.y + scaledSize.y }, backgroundColor, 4.0f);
	draw_list->AddRect({ gridPosition.x, gridPosition.y }, { gridPosition.x + scaledSize.x, gridPosition.y + scaledSize.y }, borderColor, 4.0f, 15, thickness);

	//Draw scalable text
	ImFont* scaledFont = App->gui->GetFont(zoom);
	if (scaledFont != nullptr)
		ImGui::PushFont(scaledFont);

	ImGui::SetCursorScreenPos({ gridPosition.x + NODE_BOX_PADDING * (zoom / 100.0f), gridPosition.y + NODE_BOX_PADDING * (zoom / 100.0f) });
	ImGui::BeginGroup();
	ImGui::Text(name.c_str());

	if (scaledFont != nullptr)
		ImGui::PopFont();

	ImGui::Separator(scaledSize.x- NODE_BOX_PADDING * (zoom / 100.0f)*2.0f);

	//Draw nodes
	for (std::list<CanvasNode*>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
	{
		(*it_n)->Draw(offset, zoom);
	}

	//Draw conections
	for (std::list<NodeConnection*>::iterator it_c = connections.begin(); it_c != connections.end(); ++it_c)
	{
		(*it_c)->Draw(zoom);
	}

	ImGui::EndGroup();
}

float2 NodeBox::calcSize()
{
	if (nodes.size() == 0)
	{
		size.y = NODE_BOX_MIN_HEIGHT;
	}
	else
	{
		size.y = ImGui::CalcTextSize(name.c_str()).y;
		size.y += NODE_BOX_PADDING*2.0f;
		for (std::list<CanvasNode*>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
		{
			size.y += (*it_n)->size.y + NODE_BOX_PADDING;
		}
	}

	return size;
}

bool NodeBox::OnConnection(NodeConnection* connection)
{
	if (connection->node->type == type)
	{
		if (connection->type == NODE_OUTPUT)
		{
			NodeBox* connectedBox = (NodeBox*)connection->node;
			if (connectedBox->parentGroup)
			{
				if (parentGroup)
					parentGroup->RemoveBox(this);
				else
					App->nodeCanvas->nodes.remove(this);

				parentGroup = connectedBox->parentGroup;
				parentGroup->InsertBox(this);

				if (bottomConnection->connected)
				{
					bottomConnection->connected->node->OnConnection(bottomConnection);
				}
			}
		}

		return true;
	}

	return false;
}

void NodeBox::OnDisconnection(NodeConnection* connection)
{
	if (connection->type == NODE_OUTPUT && parentGroup)
	{
		parentGroup->boxes.remove(this);
		App->nodeCanvas->nodes.push_back(this);

		parentGroup = nullptr;

		if (bottomConnection->connected)
		{
			bottomConnection->connected->node->OnDisconnection(bottomConnection);
		}
	}
}

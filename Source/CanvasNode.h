#ifndef __CANVASNODE_H__
#define __CANVASNODE_H__

#include "Globals.h"
#include <list>

class CanvasNode
{
public:

	CanvasNode() {}
	CanvasNode(const char* name, ImVec2 position, ImVec2 size = { STANDARD_NODE_HEIGHT, STANDARD_NODE_HEIGHT }) : name(name), position(position), size(size)
	{}
	~CanvasNode() {}

	bool Draw(bool hovered = false, bool selected = false);

	virtual void DisplayConfig() {}

	virtual void DrawInputs() {}
	virtual void DrawOutputs() {}

public:

	std::string name;

	ImVec2 position;
	ImVec2 size;
	std::list<uint> outputs; //Index of nodes connected to the output
};

#endif // !__CANVASNODE_H__

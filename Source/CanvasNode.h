#ifndef __CANVASNODE_H__
#define __CANVASNODE_H__

#include "Globals.h"
#include <list>

class CanvasNode
{
public:

	CanvasNode() {}
	CanvasNode(const char* name, float2 position, float2 size = { STANDARD_NODE_WIDTH, STANDARD_NODE_HEIGHT }) : name(name), position(position), size(size)
	{
		UID = GENERATE_UID();
	}
	~CanvasNode() {}

	bool Draw(float2 offset, float zoom, bool hovered = false, bool selected = false);

	virtual void DisplayConfig() {}

	virtual void DrawInputs() {}
	virtual void DrawOutputs() {}

public:

	std::string name;
	uint UID;

	float2 position;
	float2 size;
	std::list<uint> outputs; //Index of nodes connected to the output

	float2 gridPosition;
	float2 clickOffset;

	bool toDelete = false;
};

#endif // !__CANVASNODE_H__

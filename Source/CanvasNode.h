#ifndef __CANVASNODE_H__
#define __CANVASNODE_H__

#include "Globals.h"
#include <list>

class CanvasNode;

enum connectionState
{
	IDLE,
	HOVERED,
	CLICKED,
	CONNECTED
};

enum connectionType
{
	NODE_INPUT,
	NODE_OUTPUT
};

enum shapeType
{
	TRIANGLE,
	CIRCLE
};

class NodeConnection
{
public:

	NodeConnection(CanvasNode* node, connectionType type, float2 position, shapeType shape, ImGuiDir_ direction = ImGuiDir_Down);

	void Draw(int zoom);
	bool Logic(int zoom);

	void DrawTriangle(float scale);

	void SetConnection(NodeConnection* node);

public:

	CanvasNode* node = nullptr;

	shapeType shape;
	ImGuiDir_ direction = ImGuiDir_Down;
	connectionType type;

	float2 localPosition;
	float2 gridPosition;

private:

	connectionState state;
	bool connecting = false;
	NodeConnection* connected = nullptr;
};

enum nodeType
{
	PARTICLE,
	EMITTER,
	PARTICLE_COLOR,
	PARTICLE_SPEED,
	PARTICLE_MAKEGLOBAL,
	EMITTER_EMISSION
};

class CanvasNode
{
public:

	CanvasNode() {}
	CanvasNode(const char* name, nodeType type, float2 position, float2 size = { STANDARD_NODE_WIDTH, STANDARD_NODE_HEIGHT }) : name(name), type(type), position(position), size(size)
	{
		UID = GENERATE_UID();
	}
	virtual ~CanvasNode() {}

	void Draw(float2 offset, int zoom, bool hovered = false, bool selected = false);
	bool Logic(float2 offset, int zoom, bool selected = false);

	virtual void OnConnection(CanvasNode* node) {}

	virtual void DisplayConfig() {}

public:

	std::string name;
	uint UID;
	nodeType type;

	float2 position;
	float2 size;
	std::list<NodeConnection*> connections;

	float2 gridPosition;
	float2 clickOffset;

	bool toDelete = false;
};

#endif // !__CANVASNODE_H__

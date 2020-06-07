#ifndef __CANVASNODE_H__
#define __CANVASNODE_H__

#include "Globals.h"
#include <list>

class CanvasNode;

enum connectionState
{
	IDLE,
	HOVERED,
	CLICKED
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
	~NodeConnection();

	void Draw(int zoom);
	bool Logic(int zoom);

	void DrawTriangle(float scale);
	void DrawCircle(float scale);

	void SetConnection(NodeConnection* node);
	void Disconnect();

public:

	CanvasNode* node = nullptr;

	shapeType shape;
	ImGuiDir_ direction = ImGuiDir_Down;
	connectionType type;

	float2 localPosition;
	float2 gridPosition;

	NodeConnection* connected = nullptr;
	uint connectedNodeUID = 0; //Used only for loading projects

private:

	connectionState state;
	bool connecting = false;
};

enum nodeType
{
	CANVAS = 0,

	PARTICLE,
	PARTICLE_COLOR,
	PARTICLE_SPEED,
	PARTICLE_MAKEGLOBAL,
	PARTICLE_DEATHINSTANTIATION,
	PARTICLE_SPRITE,
	PARTICLE_LIFETIME,
	PARTICLE_TRANSFORM,
	PARTICLE_BLENDMODE,
	MAX_PARTICLE_NODE = 15,
	
	EMITTER,
	EMITTER_EMISSION,
	EMITTER_SHAPE,
	EMITTER_TRANSFORM,
	EMITTER_INPUTPARTICLE,
	MAX_EMITTER_NODE = 30,
	
	NODE_BOXES_START,
	NODE_BOXES_PARTICLES,
	PARTICLE_NODE_BOX_INIT,
	PARTICLE_NODE_BOX_UPDATE,
	PARTICLE_NODE_BOX_RENDER,
	
	NODE_BOXES_EMITTERS = 40,
	EMITTER_NODE_BOX_INIT,
	EMITTER_NODE_BOX_INPUT,
	EMITTER_NODE_BOX_UPDATE
};

class CanvasNode
{
public:

	CanvasNode() {}
	CanvasNode(const char* name, nodeType type, float2 position, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	virtual ~CanvasNode();

	virtual void Draw(float2 offset, int zoom);
	virtual void DrawExtraInfo(float2 offset, int zoom) {}
	bool Logic(float2 offset, int zoom);
	virtual bool ElementLogic(float2 offset, int zoom) { return false; }

	virtual bool OnConnection(NodeConnection* connection) { return false; }
	virtual void OnDisconnection(NodeConnection* connection) {}

	virtual void DisplayConfig() {}

	virtual void InsertNode(CanvasNode* node) {}

	void Save(JSON_Value* project, uint parent = 0);
	virtual void SaveExtraInfo(JSON_Value* node) {}
	virtual void SaveChildNodes(JSON_Value* project) {}
	void Load(JSON_Value* nodeDef);
	virtual void LoadExtraInfo(JSON_Value* nodeDef) {}
	void LoadConnections(std::map<uint, CanvasNode*> nodes);

public:

	std::string name;
	uint UID;
	nodeType type;

	uint parentUID = 0; //Used for loading and connecting nodes

	float2 position;
	float2 size;
	std::list<NodeConnection*> connections;

	float2 gridPosition;
	float2 clickOffset;

	float2 titleSize;

	//Canvas Node
	bool toDelete = false;
	bool hoveringElement = false;
	bool interactable = true;
	bool movable = true;
};

#endif // !__CANVASNODE_H__

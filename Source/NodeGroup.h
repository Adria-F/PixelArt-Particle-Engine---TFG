#ifndef __NODEGROUP_H__
#define __NODEGROUP_H__

#include "CanvasNode.h"

class NodeGroup;
class NodeConnection;

class NodeBox : public CanvasNode
{
public:

	NodeBox(const char* name, nodeType type, float2 position, float2 size, NodeGroup* parentGroup);
	~NodeBox();

	void Draw(float2 offset, int zoom);

	float2 calcSize();

	bool OnConnection(NodeConnection* connection);
	void OnDisconnection(NodeConnection* connection);

public:

	NodeGroup* parentGroup = nullptr;

	NodeConnection* topConnection = nullptr;
	NodeConnection* bottomConnection = nullptr;

	std::list<CanvasNode*> nodes;
};

class NodeGroup : public CanvasNode
{
public:
	NodeGroup() {}
	NodeGroup(const char* name, float2 position);
	~NodeGroup();

	void Draw(float2 offset, int zoom);
	bool ElementLogic(float2 offset, int zoom);

	NodeBox* AddNodeBox(const char* name, nodeType type);
	void InsertBox(NodeBox* box);
	void RemoveBox(NodeBox* box, bool keepInList = false);

	virtual void OnNodeAdded(CanvasNode* node) {}
	virtual void OnNodeRemoved(CanvasNode* node) {}
	
	void CalcRect();

public:

	std::list<NodeBox*> boxes;
};

#endif // !__NODEGROUP_H__
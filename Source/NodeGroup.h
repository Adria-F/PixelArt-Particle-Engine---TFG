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
	bool ElementLogic(float2 offset, int zoom);

	float2 calcSize();

	void InsertNode(CanvasNode* node);

	bool OnConnection(NodeConnection* connection);
	void OnDisconnection(NodeConnection* connection);

	void SaveChildNodes(JSON_Value* project);

public:

	NodeGroup* parentGroup = nullptr;

	NodeConnection* topConnection = nullptr;
	NodeConnection* bottomConnection = nullptr;

	std::list<CanvasNode*> nodes;

private:

	ImVec2 nodeStartPosition;
	bool addBlockHovered = false;
};

class NodeGroup : public CanvasNode
{
public:
	NodeGroup() {}
	NodeGroup(const char* name, float2 position, nodeType type);
	~NodeGroup();

	void Draw(float2 offset, int zoom);
	bool ElementLogic(float2 offset, int zoom);

	NodeBox* AddNodeBox(const char* name, nodeType type);
	void InsertNode(CanvasNode* node);
	void RemoveBox(NodeBox* box, bool keepInList = false);
	bool HasBox(nodeType type) const;

	void RepositionBoxes(NodeBox* resizedBox, float prevBottom);

	virtual void OnNodeAdded(CanvasNode* node, bool update) {}
	virtual void OnNodeRemoved(CanvasNode* node) {}
	void AddNodes();
	
	void MoveChildren(float2 amount);
	void CalcRect();

	void SaveChildNodes(JSON_Value* project);

public:

	std::list<NodeBox*> boxes;

	float2 prevPos;
};

#endif // !__NODEGROUP_H__
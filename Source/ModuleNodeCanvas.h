#ifndef __MODULENODECANVAS_H__
#define __MODULENODECANVAS_H__

#include "Module.h"
#include <list>

class CanvasNode;
class NodeConnection;
class NodeBox;

enum connectionState;
enum nodeType;

class ModuleNodeCanvas : public Module
{
public:

	ModuleNodeCanvas(bool start_enabled = true);
	~ModuleNodeCanvas();

	bool Start();
	update_state Update(float dt);
	update_state PostUpdate(float dt);
	bool CleanUp();

	void DrawGuizmo();

	std::vector<nodeType> GetAllowedNodes(CanvasNode* nodeContainer) const;
	CanvasNode* DrawNodeList(float2 spawnPos, CanvasNode* nodeContainer);
	std::map<std::string, int> RequestNodeList(CanvasNode* nodeContainer) const;
	CanvasNode* CreateNode(const char* name, nodeType type, float2 spawnPos, bool empty = false) const;

	void StartConnection(NodeConnection* connection);
	void RequestConnection(NodeConnection* node);
	void StopConnection();

public:

	std::list<CanvasNode*> nodes;

	CanvasNode* hoveredNode = nullptr;
	CanvasNode* newHoveredNode = nullptr;
	CanvasNode* selectedNode = nullptr;
	NodeConnection* selectedConnection = nullptr;

	bool canvasFocused = false;

private:

	NodeConnection* connecting = nullptr;
	NodeConnection* connectionCallback = nullptr;
	bool connectionEnded = false;

	ImGuizmo::OPERATION guizmoOperation = ImGuizmo::TRANSLATE;

	char node_filter[64];
	bool drawingNodeList = false;
};

#endif // !__MODULENODECANVAS_H__
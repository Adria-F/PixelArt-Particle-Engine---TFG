#ifndef __MAKEGLOBAL_PARTICLENODE_H__
#define __MAKEGLOBAL_PARTICLENODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class MakeGlobalParticleNode : public EntityData, public CanvasNode
{
public:

	MakeGlobalParticleNode(Particle* particle, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { STANDARD_NODE_WIDTH, STANDARD_NODE_HEIGHT });
	~MakeGlobalParticleNode() {}

	EntityData* Copy(Particle* particle) const;

	bool OnConnection(CanvasNode* node);
	void OnDisconnection(NodeConnection* connection);

	void DisplayConfig();

public:

	bool active = true;
};

#endif // !__MAKEGLOBAL_PARTICLENODE_H__
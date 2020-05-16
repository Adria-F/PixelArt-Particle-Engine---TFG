#ifndef __MAKEGLOBAL_PARTICLENODE_H__
#define __MAKEGLOBAL_PARTICLENODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class MakeGlobalParticleNode : public EntityData, public CanvasNode
{
public:

	MakeGlobalParticleNode(Particle* particle, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~MakeGlobalParticleNode() {}

	EntityData* Copy(Particle* particle) const;

	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	bool active = true;
};

#endif // !__MAKEGLOBAL_PARTICLENODE_H__
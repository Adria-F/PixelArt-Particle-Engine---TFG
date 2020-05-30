#ifndef __LIFETIME_PARTICLENODE_H__
#define __LIFETIME_PARTICLENODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class LifetimeParticleNode : public EntityData, public CanvasNode
{
public:

	LifetimeParticleNode(Particle* particle, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~LifetimeParticleNode() {}

	EntityData* Copy(Particle* particle) const;

	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	float lifetime = 5.0f;

	float randLifetime1 = 3.0f;
	float randLifetime2 = 5.0f;

	bool random = false;
};

#endif // !__LIFETIME_PARTICLENODE_H__
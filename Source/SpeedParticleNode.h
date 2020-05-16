#ifndef __SPEED_PARTICLENODE_H__
#define __SPEED_PARTICLENODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class SpeedParticleNode : public EntityData, public CanvasNode
{
public:

	SpeedParticleNode(Particle* particle, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~SpeedParticleNode() {}

	void Execute(float dt);

	EntityData* Copy(Particle* particle) const;

	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	float speed = 0.0f;

	bool overLifetime = false;
};

#endif // !__SPEED_PARTICLENODE_H__
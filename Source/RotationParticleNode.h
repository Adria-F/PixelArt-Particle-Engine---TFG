#ifndef __ROTATION_PARTICLENODE_H__
#define __ROTATION_PARTICLENODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class RotationParticleNode : public EntityData, public CanvasNode
{
public:

	RotationParticleNode(Particle* particle, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~RotationParticleNode() {}

	void Init();
	void Execute(float dt);

	EntityData* Copy(Particle* particle) const;

	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	float angle = 0.0f;

};

#endif // !__ROTATION_PARTICLENODE_H__
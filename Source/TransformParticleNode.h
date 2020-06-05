#ifndef __TRANSFORM_PARTICLENODE_H__
#define __TRANSFORM_PARTICLENODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class TransformParticleNode : public EntityData, public CanvasNode
{
public:

	TransformParticleNode(Particle* particle, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~TransformParticleNode() {}

	void Init();
	void Execute(float dt);

	EntityData* Copy(Particle* particle) const;

	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	vec partPosition = vec::zero;
	vec randPosition1 = vec::zero;
	vec randPosition2 = vec::zero;
	bool randomPos = false;

	vec rotation = vec::zero;
	vec randRotation1 = vec::zero;
	vec randRotation2 = vec::zero;
	bool randomRot = false;

	float2 scale = float2::one;
	float2 randScale1 = float2::one;
	float2 randScale2 = float2::one;
	bool randomScale = false;

	bool billboard = true;
};

#endif // !__TRANSFORM_PARTICLENODE_H__
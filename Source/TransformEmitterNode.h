#ifndef __TRANSFORM_EMITTERNODE_H__
#define __TRANSFORM_EMITTERNODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class TransformEmitterNode : public EntityData, public CanvasNode
{
public:

	TransformEmitterNode(ParticleEmitter* emitter, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~TransformEmitterNode() {}

	void Execute(float dt);

	void DrawExtraInfo(float2 offset, int zoom);
	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	vec position = vec::zero;
	Quat rotation = Quat::identity;
	vec rotationEuler = Quat::identity.ToEulerXYZ();
	vec scale = vec::one;

	float4x4 matrix;

	bool changed = false;
};

#endif // !__TRANSFORM_EMITTERNODE_H__
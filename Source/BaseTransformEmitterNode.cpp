#include "BaseTransformEmitterNode.h"

BaseTransformEmitterNode::BaseTransformEmitterNode(ParticleEmitter* emitter): EntityData(emitter)
{
	CalculateMatrix();
}

void BaseTransformEmitterNode::CalculateMatrix()
{
	rotation = Quat::FromEulerXYZ(rotationEuler.x, rotationEuler.y, rotationEuler.z);
	matrix.Set(float4x4::FromTRS(position, rotation, scale));
}
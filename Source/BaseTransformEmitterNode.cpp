#include "BaseTransformEmitterNode.h"

BaseTransformEmitterNode::BaseTransformEmitterNode(ParticleEmitter* emitter): EmitterData(emitter)
{
	CalculateMatrix();
}

void BaseTransformEmitterNode::CalculateMatrix()
{
	matrix.Set(float4x4::FromTRS(position, rotation, scale));
}
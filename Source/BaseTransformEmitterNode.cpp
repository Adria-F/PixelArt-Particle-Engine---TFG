#include "BaseTransformEmitterNode.h"

BaseTransformEmitterNode::BaseTransformEmitterNode(ParticleEmitter* emitter): EntityData(emitter)
{
	CalculateMatrix();
}

void BaseTransformEmitterNode::CalculateMatrix()
{	
	matrix.Set(float4x4::FromTRS(position, rotation, scale));
}
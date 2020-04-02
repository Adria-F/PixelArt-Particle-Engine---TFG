#ifndef __BASETRANSFORM_EMITTERNODE_H__
#define __BASETRANSFORM_EMITTERNODE_H__

#include "EmitterData.h"

class BaseTransformEmitterNode : public EmitterData
{
public:

	BaseTransformEmitterNode(ParticleEmitter* emitter);
	~BaseTransformEmitterNode() {}

	void CalculateMatrix();

public:

	vec position = vec::zero;
	Quat rotation = Quat::identity;
	vec scale = vec::one;

	float4x4 matrix;
};

#endif // !__BASETRANSFORM_EMITTERNODE_H__
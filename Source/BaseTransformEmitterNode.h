#ifndef __BASETRANSFORM_EMITTERNODE_H__
#define __BASETRANSFORM_EMITTERNODE_H__

#include "EntityData.h"

class BaseTransformEmitterNode : public EntityData
{
public:

	BaseTransformEmitterNode(ParticleEmitter* emitter);
	~BaseTransformEmitterNode() {}

	void CalculateMatrix();

public:

	vec position = vec::zero;
	Quat rotation = Quat::identity;
	vec rotationEuler = Quat::identity.ToEulerXYZ();
	vec scale = vec::one;

	float4x4 matrix;
};

#endif // !__BASETRANSFORM_EMITTERNODE_H__
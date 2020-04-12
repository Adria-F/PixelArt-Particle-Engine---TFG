#ifndef __BASETRANSFORM_PARTICLENODE_H__
#define __BASETRANSFORM_PARTICLENODE_H__

#include "EntityData.h"

class BaseTransformParticleNode : public EntityData
{
public:

	BaseTransformParticleNode(Particle* particle);
	~BaseTransformParticleNode() {}

	void PrepareRender();
	
	void LookCamera();
	EntityData* Copy(Particle* particle) const;

	void SetRandomScale(bool random);
	void SetRandomRotation(bool random);

	void RandomizeScale();
	void RandomizeRotation();

public:
	
	bool billboard = true;

	vec position = vec::zero;
	Quat rotation = Quat::identity;
	vec scale = vec::one;

	float angleZ = 0.0f;

	float randRotation1 = 0.0f;
	float randRotation2 = 360.0f;

	float randScaleX1 = 0.5f;
	float randScaleX2 = 1.5f;
	float randScaleY1 = 0.5f;
	float randScaleY2 = 1.5f;

	bool randomizeScale = false;
	bool randomizeRotation = false;

private:

	vec eulerAngles;
};

#endif // !__BASETRANSFORM_PARTICLENODE_H__
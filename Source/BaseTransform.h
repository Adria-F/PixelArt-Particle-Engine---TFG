#ifndef __PARTICLE_BASETRANSFORM_H__
#define __PARTICLE_BASETRANSFORM_H__

#include "ParticleData.h"

class BaseTransform : public ParticleData
{
public:

	BaseTransform(Particle* particle);
	~BaseTransform() {}

	void PrepareRender();
	
	void LookCamera();
	ParticleData* Copy(Particle* particle) const;

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

private:

	vec eulerAngles;

	bool randomizeScale = false;
	bool randomizeRotation = false;
};

#endif // !__PARTICLE_BASETRANSFORM_H__
#ifndef __PARTICLE_BASETRANSFORM_H__
#define __PARTICLE_BASETRANSFORM_H__

#include "ParticleData.h"

class BaseTransform : public ParticleData
{
public:

	BaseTransform(Particle* particle): ParticleData(particle)
	{}
	~BaseTransform() {}

	void Execute(float dt);
	void PrepareRender();
	
	ParticleData* Copy(Particle* particle) const;

public:
	
	bool billboard = true;

	vec position = vec::zero;
	Quat rotation = Quat::identity;
	vec scale = vec::one;

private:

	vec eulerAngles;
};

#endif // !__PARTICLE_BASETRANSFORM_H__
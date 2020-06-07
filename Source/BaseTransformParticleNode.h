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

public:
	
	bool billboard = true;

	vec globalPostion = vec::zero;
	vec position = vec::zero;
	Quat rotation = Quat::identity;
	vec scale = vec::one;
};

#endif // !__BASETRANSFORM_PARTICLENODE_H__
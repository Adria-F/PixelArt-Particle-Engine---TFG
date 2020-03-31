#ifndef __PARTICLE_BASECOLOR_H__
#define __PARTICLE_BASECOLOR_H__

#include "ParticleData.h"

class BaseColor : public ParticleData
{
public:

	BaseColor(Particle* particle): ParticleData(particle)
	{}
	~BaseColor() {}

	void PrepareRender();

	ParticleData* Copy(Particle* particle) const;

public:

	float4 color = float4::one;
};

#endif // !__PARTICLE_BASECOLOR_H__
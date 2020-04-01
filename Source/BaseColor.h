#ifndef __PARTICLE_BASECOLOR_H__
#define __PARTICLE_BASECOLOR_H__

#include "ParticleData.h"
#include "Color.h"

class BaseColor : public ParticleData
{
public:

	BaseColor(Particle* particle);
	~BaseColor() {}

	void PrepareRender();

	ParticleData* Copy(Particle* particle) const;

	void SetRandom(bool random);
	void Randomize();

public:

	Color color = White;
	Gradient random;

private:

	bool randomize = false;

};

#endif // !__PARTICLE_BASECOLOR_H__
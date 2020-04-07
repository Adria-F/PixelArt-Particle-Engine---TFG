#ifndef __BASECOLOR_PARTICLENODE_H__
#define __BASECOLOR_PARTICLENODE_H__

#include "EntityData.h"
#include "Color.h"

class BaseColorParticleNode : public EntityData
{
public:

	BaseColorParticleNode(Particle* particle);
	~BaseColorParticleNode() {}

	void PrepareRender();

	EntityData* Copy(Particle* particle) const;

	void SetRandom(bool random);
	void Randomize();

public:

	Color color = White;
	Gradient random;

private:

	bool randomize = false;

};

#endif // !__BASECOLOR_PARTICLENODE_H__
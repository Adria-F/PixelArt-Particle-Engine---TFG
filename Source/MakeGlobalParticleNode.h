#ifndef __MAKEGLOBAL_PARTICLENODE_H__
#define __MAKEGLOBAL_PARTICLENODE_H__

#include "ParticleData.h"

class MakeGlobalParticleNode : public ParticleData
{
public:

	MakeGlobalParticleNode(Particle* particle);
	~MakeGlobalParticleNode() {}

	ParticleData* Copy(Particle* particle) const;

public:

	bool active = true;
};

#endif // !__MAKEGLOBAL_PARTICLENODE_H__
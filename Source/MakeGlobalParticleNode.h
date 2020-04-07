#ifndef __MAKEGLOBAL_PARTICLENODE_H__
#define __MAKEGLOBAL_PARTICLENODE_H__

#include "EntityData.h"

class MakeGlobalParticleNode : public EntityData
{
public:

	MakeGlobalParticleNode(Particle* particle);
	~MakeGlobalParticleNode() {}

	EntityData* Copy(Particle* particle) const;

public:

	bool active = true;
};

#endif // !__MAKEGLOBAL_PARTICLENODE_H__
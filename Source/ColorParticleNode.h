#ifndef __COLORPARTICLE_NODE_H__
#define __COLORPARTICLE_NODE_H__

#include "EntityData.h"
#include "CanvasNode.h"
#include "Color.h"

class ColorParticleNode : public EntityData, public CanvasNode
{
public:

	ColorParticleNode(Particle* particle, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { STANDARD_NODE_WIDTH, STANDARD_NODE_HEIGHT });
	~ColorParticleNode() {}

	//Entity Data
	void Execute(float dt);

	EntityData* Copy(Particle* particle) const;

	bool OnConnection(CanvasNode* node);
	void OnDisconnection(NodeConnection* connection);

public:

	Color fixColor;
	Gradient overLifetimeColor;

	bool overLifetime = false;
};

#endif // !__COLORPARTICLE_NODE_H__
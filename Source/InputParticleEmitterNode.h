#ifndef __INPUTPARTICLE_EMITTERNODE_H__
#define __INPUTPARTICLE_EMITTERNODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class InputParticleEmitterNode : public EntityData, public CanvasNode
{
public:

	InputParticleEmitterNode(ParticleEmitter* emitter, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~InputParticleEmitterNode() {}

	void Execute(float dt);

	bool OnConnection(NodeConnection* connection);
	void OnDisconnection(NodeConnection* connection);

public:

	NodeConnection* particleIn = nullptr;
	Particle* inputParticle = nullptr;
};

#endif // !__INPUTPARTICLE_EMITTERNODE_H__
#ifndef __DEATHINSTANTIATION_PARTICLENODE_H__
#define __DEATHINSTANTIATION_PARTICLENODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class DeathInstantiationParticleNode : public EntityData, public CanvasNode
{
public:

	DeathInstantiationParticleNode(Particle* particle, const char* name, float2 position = { 0.0f, 0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~DeathInstantiationParticleNode() {}

	//Entity Data
	void Execute(float dt);

	EntityData* Copy(Particle* particle) const;

	bool OnConnection(NodeConnection* connection);
	void OnDisconnection(NodeConnection* connection);

	//void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	Particle* instantiateParticle = nullptr;
};

#endif // !__DEATHINSTANTIATION_PARTICLENODE_H__
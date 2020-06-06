#ifndef __BLENDMODE_PARTICLENODE_H__
#define __BLENDMODE_PARTICLENODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

enum blendModeType;

class BlendModeParticleNode : public EntityData, public CanvasNode
{
public:

	BlendModeParticleNode(Particle* particle, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~BlendModeParticleNode() {}

	void PrepareRender();

	EntityData* Copy(Particle* particle) const;

	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	blendModeType blendMode;
};

#endif // !__BLENDMODE_PARTICLENODE_H__
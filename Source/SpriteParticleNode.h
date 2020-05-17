#ifndef __SPRITE_PARTICLENODE_H__
#define __SPRITE_PARTICLENODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class SpriteParticleNode : public EntityData, public CanvasNode
{
public:

	SpriteParticleNode(Particle* particle, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~SpriteParticleNode();

	void PrepareRender();

	EntityData* Copy(Particle* particle) const;

	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	uint spriteUID = 0;
	uint GL_id = 0;

	std::string currentSprite = "None";
};

#endif // !__SPRITE_PARTICLENODE_H__
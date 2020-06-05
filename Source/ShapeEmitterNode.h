#ifndef __SHAPE_EMITTERNODE_H__
#define __SHAPE_EMITTERNODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

enum emissionShape
{
	//3D shapes
	CONE_SHAPE,
	SPHERE_SHAPE,
	BOX_SHAPE,
	LAST_3D_SHAPE,
	MAX_3D_SHAPES = 15,

	//2D shapes
	CIRCLE_SHAPE,
	QUAD_SHAPE,
	LAST_2D_SHAPE,
	MAX_2D_SHAPES = 30
};

enum emissionDirection
{
	RANDOM_DIRECTION,
	LOOP_DIRECTION,
	PING_PONG
};

class ShapeEmitterNode : public EntityData, public CanvasNode
{
public:

	ShapeEmitterNode(ParticleEmitter* emitter, const char* name, float2 position = { 0.0f, 0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~ShapeEmitterNode() {}

	void DisplayConfig();

	vec GetDirection() const;

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

private:

	vec GetDirectionInCone() const;
	vec GetDirectionInSphere() const;
	vec GetDirectionInBox() const;

	vec GetDirectionInCircle() const;
	vec GetDirectionInQuad() const;

public:

	emissionShape shape = CIRCLE_SHAPE;
	emissionDirection direction = RANDOM_DIRECTION;
	float speed = 1.0f;
	int phase = 0.0f;

	//CONE
	float radius = 3.0f;
	float height = 5.0f;

	//CIRCLE - SPHERE
	int angle = 360;

	//BOX - QUAD
	vec boxSize = vec::one;

	std::string currentShape = "Circle";
	std::string currentDirection = "Random";
};

#endif // !__SHAPE_EMITTERNODE_H__
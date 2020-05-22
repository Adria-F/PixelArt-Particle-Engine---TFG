#include "Application.h"
#include "ShapeEmitterNode.h"

ShapeEmitterNode::ShapeEmitterNode(ParticleEmitter* emitter, const char* name, float2 position, float2 size) : EntityData(emitter), CanvasNode(name, EMITTER_SHAPE, position, size)
{
}

void ShapeEmitterNode::DisplayConfig()
{
	static const char* shapes3D[] = { "Cone", "Sphere", "Box" };
	static const char* shapes2D[] = { "Circle", "Quad" };

	if (ImGui::BeginCombo("Shape", currentShape.c_str()))
	{
		for (int i = 0; i < LAST_3D_SHAPE; ++i)
		{
			if (ImGui::Selectable(shapes3D[i], shape == i))
			{
				shape = (emissionShape)i;
				currentShape = shapes3D[i];
			}
		}
		for (int i = 0; i < LAST_2D_SHAPE-MAX_3D_SHAPES-1; ++i)
		{
			if (ImGui::Selectable(shapes2D[i], shape == (i + MAX_3D_SHAPES + 1)))
			{
				shape = (emissionShape)(i+MAX_3D_SHAPES+1);
				currentShape = shapes2D[i];
			}
		}

		ImGui::EndCombo();
	}
}

vec ShapeEmitterNode::GetDirection() const
{
	vec ret;

	switch (shape)
	{
	case CONE_SHAPE:
		ret = GetDirectionInCone();
		break;
	case SPHERE_SHAPE:
		ret = GetDirectionInSphere();
		break;
	case BOX_SHAPE:
		ret = GetDirectionInBox();
		break;
	case CIRCLE_SHAPE:
		ret = GetDirectionInCircle();
		break;
	case QUAD_SHAPE:
		ret = GetDirectionInQuad();
		break;
	}

	return ret;
}

vec ShapeEmitterNode::GetDirectionInCone() const
{
	float angle = 2.0*PI * GET_RANDOM();
	float x = Cos(angle)* radius*GET_RANDOM();
	float z = Sin(angle)* radius*GET_RANDOM();

	vec point = vec(x, height, z);

	return point.Normalized();
}

vec ShapeEmitterNode::GetDirectionInSphere() const
{
	return vec(0.0f,1.0f,0.0f);
}

vec ShapeEmitterNode::GetDirectionInBox() const
{
	return vec(0.0f,1.0f,0.0f);
}

vec ShapeEmitterNode::GetDirectionInCircle() const
{
	float angle = 2.0*PI * GET_RANDOM();
	float x = Cos(angle)* radius*GET_RANDOM();
	float y = Sin(angle)* radius*GET_RANDOM();

	vec point = vec(x, y, 0.0f);

	return point.Normalized();
}

vec ShapeEmitterNode::GetDirectionInQuad() const
{
	return vec(0.0f,1.0f,0.0f);
}

void ShapeEmitterNode::SaveExtraInfo(JSON_Value* node)
{
	node->addUint("shape", shape);

	node->addFloat("radius", radius);
	node->addFloat("height", height);

	node->addVector3("boxSize", boxSize);
}

void ShapeEmitterNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	shape = (emissionShape)nodeDef->getUint("shape");

	radius = nodeDef->getFloat("radius");
	height = nodeDef->getFloat("height");

	boxSize = nodeDef->getVector3("boxSize");
}
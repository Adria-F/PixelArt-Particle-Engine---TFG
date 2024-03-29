#include "Application.h"
#include "ShapeEmitterNode.h"

#include "ModuleParticles.h"
#include "BaseTransformEmitterNode.h"
#include "ModuleCamera.h"

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

	ImGui::NewLine();
	ImGui::Text("Shape properties:");
	switch (shape)
	{
	case CONE_SHAPE:
		ImGui::InputFloat("Radius", &radius, 0.1f);
		ImGui::InputFloat("Height", &height, 0.1f);
		DrawDirectionOptions();
		break;
	case SPHERE_SHAPE:
		ImGui::DragInt("Angle", &angle, 1.0f, 1, 360);
		ImGui::InputFloat("Radius", &radius, 0.1f);		
		break;
	case BOX_SHAPE:
		ImGui::InputFloat3("Box Size", boxSize.ptr());
		break;
	case CIRCLE_SHAPE:
		ImGui::DragInt("Angle", &angle, 1.0f, 1, 360);		
		ImGui::InputFloat("Radius", &radius, 0.1f);
		DrawDirectionOptions();
		break;
	case QUAD_SHAPE:
		ImGui::InputFloat2("Quad Size", boxSize.ptr());
		break;
	}
}

void ShapeEmitterNode::DrawDirectionOptions()
{
	ImGui::Text("Direction Options:");
	if (ImGui::BeginCombo("", currentDirection.c_str()))
	{
		if (ImGui::Selectable("Random", direction == RANDOM_DIRECTION))
		{
			direction = RANDOM_DIRECTION;
			currentDirection = "Random";
		}
		if (ImGui::Selectable("Loop", direction == LOOP_DIRECTION))
		{
			direction = LOOP_DIRECTION;
			currentDirection = "Loop";
		}
		if (ImGui::Selectable("Ping-Pong", direction == PING_PONG))
		{
			direction = PING_PONG;
			currentDirection = "Ping-Pong";
		}

		ImGui::EndCombo();
	}
	if (direction == LOOP_DIRECTION || direction == PING_PONG)
	{
		ImGui::InputFloat("Speed", &speed);
		ImGui::DragInt("Phase", &phase, 1.0f, 0, angle);
		if (phase > angle)
			phase = angle;
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
		ret = GetDirectionInCircle(angle, radius);
		break;
	case QUAD_SHAPE:
		ret = GetDirectionInQuad();
		break;
	}

	if (App->camera->type == CAMERA_3D)
	{
		ret = emitter->baseTransform->rotation * ret;
		ret = { ret.x*emitter->baseTransform->scale.x, ret.y*emitter->baseTransform->scale.y , ret.z*emitter->baseTransform->scale.z };
	}
	else
	{
		ret = Quat::FromEulerXYZ(0.0f, 0.0f, emitter->baseTransform->rotationEuler.z*DEGTORAD) * ret; //If 2D camera only use Z axis rotation
		ret = { ret.x*emitter->baseTransform->scale.x, ret.y*emitter->baseTransform->scale.y , ret.z };
	}

	return ret;
}

vec ShapeEmitterNode::GetDirectionInCone() const
{
	vec point = GetDirectionInCircle(360, radius); //Point on the top circle
	switch (direction)
	{
	case RANDOM_DIRECTION:
		point *= GET_RANDOM();
		break;
	}

	return vec(point.x, height, point.y);
}

vec ShapeEmitterNode::GetDirectionInSphere() const
{
	return vec(0.0f,1.0f,0.0f); //TODO
}

vec ShapeEmitterNode::GetDirectionInBox() const
{
	return vec(0.0f,1.0f,0.0f); //TODO
}

vec ShapeEmitterNode::GetDirectionInCircle(int angle, float radius) const
{
	float factor = 0.0f; //A number between -1 and 1
	switch (direction)
	{
	case RANDOM_DIRECTION:
		factor = (GET_RANDOM()*2.0f - 1.0f);
		break;
	case LOOP_DIRECTION:
		factor = -Mod(emitter->timeAlive*speed*2.0f+(float)phase/(float)angle*2.0f, 2.0f) + 1.0f;
		break;
	case PING_PONG:
		factor = Cos(emitter->timeAlive*speed*3.0f+(float)phase/(float)angle*180.0f*DEGTORAD);
		break;
	}
	float degrees = DEGTORAD*angle*0.5f * factor + DEGTORAD*90.0f;
	float x = Cos(degrees);
	float y = Sin(degrees);

	vec point = vec(x, y, 0.0f);

	return point.Normalized()*radius;
}

vec ShapeEmitterNode::GetDirectionInQuad() const
{
	return vec(0.0f,1.0f,0.0f); //TODO
}

void ShapeEmitterNode::SaveExtraInfo(JSON_Value* node)
{
	node->addUint("shape", shape);
	node->addString("currentShape", currentShape.c_str());

	node->addFloat("radius", radius);
	node->addFloat("height", height);

	node->addFloat("angle", angle);
	node->addUint("direction", direction);
	node->addFloat("speed", speed);
	node->addFloat("phase", phase);
	node->addString("currentDirection", currentDirection.c_str());

	node->addVector3("boxSize", boxSize);
}

void ShapeEmitterNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	shape = (emissionShape)nodeDef->getUint("shape");
	currentShape = nodeDef->getString("currentShape");

	radius = nodeDef->getFloat("radius");
	height = nodeDef->getFloat("height");

	angle = nodeDef->getFloat("angle");
	direction = (emissionDirection)nodeDef->getUint("direction");
	speed = nodeDef->getFloat("speed");
	phase = nodeDef->getFloat("phase");
	currentDirection = nodeDef->getString("currentDirection");

	boxSize = nodeDef->getVector3("boxSize");
}
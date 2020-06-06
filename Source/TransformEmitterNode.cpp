#include "Application.h"
#include "TransformEmitterNode.h"

#include "ModuleGUI.h"
#include "ModuleParticles.h"
#include "BaseTransformEmitterNode.h"
#include "ModuleCamera.h"

TransformEmitterNode::TransformEmitterNode(ParticleEmitter* emitter, const char* name, float2 position, float2 size) : EntityData(emitter), CanvasNode(name, EMITTER_TRANSFORM, position, size)
{
	update = true;
	matrix.Set(float4x4::FromTRS(this->position, rotation, scale));
}

void TransformEmitterNode::Execute(float dt)
{
	if (changed)
	{
		emitter->baseTransform->position = position;
		emitter->baseTransform->scale = scale;
		emitter->baseTransform->rotationEuler = rotationEuler;
		emitter->baseTransform->rotation = rotation = Quat::FromEulerXYZ(rotationEuler.x*DEGTORAD, rotationEuler.y*DEGTORAD, rotationEuler.z*DEGTORAD);

		emitter->baseTransform->CalculateMatrix();
		matrix = emitter->baseTransform->matrix; //Only if just one transform node applies
		changed = false;
	}
}

void TransformEmitterNode::DrawExtraInfo(float2 offset, int zoom)
{
	if (App->camera->type == CAMERA_2D)
	{
		float2 scaledSize = size * (zoom / 100.0f);
		ImGui::SetCursorScreenPos({ gridPosition.x + NODE_PADDING * (zoom / 100.0f), gridPosition.y + scaledSize.y - 17.0f*(zoom / 100.0f) });
		ImGui::TextColored({ 120.0f,0.0f,0.0f,1.0f }, "No guizmo in 2D mode");
	}
}

void TransformEmitterNode::DisplayConfig()
{
	if (App->camera->type == CAMERA_3D)
	{
		changed |= App->gui->DrawInputFloat3("Position", position.ptr(), 75.0f);
		changed |= App->gui->DrawInputFloat3("Rotation", rotationEuler.ptr(), 75.0f);
		changed |= App->gui->DrawInputFloat3("Scale", scale.ptr(), 75.0f);
	}
	else
	{
		changed |= App->gui->DrawInputFloat2("Position", position.ptr(), 75.0f);
		changed |= App->gui->DrawInputFloat("Rotation", &rotationEuler.z, 90.0f);
		changed |= App->gui->DrawInputFloat2("Scale", scale.ptr(), 75.0f);
	}
}

void TransformEmitterNode::SaveExtraInfo(JSON_Value* node)
{
	node->addTransform("matrix", matrix);
}

void TransformEmitterNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	matrix = nodeDef->getTransform("matrix");

	matrix.Decompose(position, rotation, scale);
	rotationEuler = rotation.ToEulerXYZ()*RADTODEG;
	changed = true;
}

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
		emitter->baseTransform->rotation = rotation = Quat::FromEulerXYZ(rotationEuler.x, rotationEuler.y, rotationEuler.z);

		emitter->baseTransform->CalculateMatrix();
		matrix = emitter->baseTransform->matrix; //Only if just one transform node applies
		//matrix.Set(float4x4::FromTRS(position, rotation, scale));
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
	//Position
	ImGui::Text("Position"); ImGui::SameLine();
	if (App->gui->DrawInputFloat("X", "##posX", &position.x, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Y", "##posY", &position.y, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Z", "##posZ", &position.z, 0.1f, true))
		changed = true;
	
	//Rotation
	ImGui::Text("Rotation"); ImGui::SameLine();
	if (App->gui->DrawInputFloat("X", "##rotX", &rotationEuler.x, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Y", "##rotY", &rotationEuler.y, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Z", "##rotZ", &rotationEuler.z, 0.1f, true))
		changed = true;
	
	//Scale
	ImGui::Text("Scale"); ImGui::SameLine();
	if (App->gui->DrawInputFloat("X", "##scaleX", &scale.x, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Y", "##scaleY", &scale.y, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Z", "##scaleZ", &scale.z, 0.1f, true))
		changed = true;
}

void TransformEmitterNode::SaveExtraInfo(JSON_Value* node)
{
	node->addTransform("matrix", matrix);
}

void TransformEmitterNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	matrix = nodeDef->getTransform("matrix");

	matrix.Decompose(position, rotation, scale);
	rotationEuler = rotation.ToEulerXYZ();
	changed = true;
}

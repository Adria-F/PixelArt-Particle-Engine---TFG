#include "Application.h"
#include "TransformParticleNode.h"

#include "ModuleParticles.h"
#include "BaseTransformParticleNode.h"
#include "ModuleCamera.h"
#include "ModuleGUI.h"

TransformParticleNode::TransformParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_TRANSFORM, position, size)
{
}

void TransformParticleNode::Init()
{
	particle->baseTransform->billboard = billboard;

	if (App->camera->type == CAMERA_2D || billboard)
	{
		float angle = rotation.z;
		if (randomRot)
			angle = Lerp(randRotation1.z, randRotation2.z, GET_RANDOM());
		particle->baseTransform->rotation = Quat::FromEulerXYZ(0.0f, 0.0f, DEGTORAD*angle);
	}
	else
	{
		vec rot = rotation;
		if (randomRot)
		{
			rot.x = Lerp(randRotation1.x, randRotation2.x, GET_RANDOM());
			rot.y = Lerp(randRotation1.y, randRotation2.y, GET_RANDOM());
			rot.z = Lerp(randRotation1.z, randRotation2.z, GET_RANDOM());
		}
		particle->baseTransform->rotation = Quat::FromEulerXYZ(DEGTORAD*rot.x, DEGTORAD*rot.y, DEGTORAD*rot.z);
	}
	float2 scl = scale;
	if (randomScale)
	{
		scl.x = Lerp(randScale1.x, randScale2.x, GET_RANDOM());
		scl.y = Lerp(randScale1.y, randScale2.y, GET_RANDOM());
	}
	particle->baseTransform->scale = { scl.x, scl.y, 1.0f };
}

void TransformParticleNode::Execute(float dt)
{
	BROFILER_CATEGORY("TransformParticleNode", Profiler::Color::LightGoldenRodYellow);

	particle->baseTransform->position += vec(partPosition.x, partPosition.y, (App->camera->type == CAMERA_2D)? 0.0f : partPosition.z) * dt;

	if (App->camera->type == CAMERA_2D || billboard)
		particle->baseTransform->rotation = Quat::FromEulerXYZ(0.0f, 0.0f, particle->baseTransform->rotation.ToEulerXYZ().z + (DEGTORAD*rotation.z*dt));
	else
	{
		vec rot = particle->baseTransform->rotation.ToEulerXYZ();
		particle->baseTransform->rotation = Quat::FromEulerXYZ(rot.x + (DEGTORAD*rotation.x*dt), rot.y + (DEGTORAD*rotation.y*dt), rot.z + (DEGTORAD*rotation.z*dt));
	}

	float2 scl = { Lerp(1.0f, scale.x, dt),Lerp(1.0f, scale.y, dt) };
	particle->baseTransform->scale = vec(particle->baseTransform->scale.x*scl.x, particle->baseTransform->scale.y*scl.y, particle->baseTransform->scale.z);
}

EntityData* TransformParticleNode::Copy(Particle* particle) const
{
	TransformParticleNode* ret = new TransformParticleNode(particle, name.c_str(), position, size);

	ret->update = update;
	ret->billboard = billboard;

	ret->spawnPoint = spawnPoint;

	ret->partPosition = partPosition;
	ret->randPosition1 = randPosition1;
	ret->randPosition2 = randPosition2;
	ret->randomPos = randomPos;

	ret->rotation = rotation;
	ret->randRotation1 = randRotation1;
	ret->randRotation2 = randRotation2;
	ret->randomRot = randomRot;

	ret->scale = scale;
	ret->randScale1 = randScale1;
	ret->randScale2 = randScale2;
	ret->randomScale = randomScale;

	return ret;
}

void TransformParticleNode::DisplayConfig()
{
	if (update)
	{
		if (App->camera->type == CAMERA_3D)
		{
			App->gui->DrawInputFloat3("Position:", partPosition.ptr(), 75.0f);
			ImGui::Separator();
			if (billboard)
				App->gui->DrawInputFloat("Rotation:", &rotation.z, 90.0f);
			else
				App->gui->DrawInputFloat3("Rotation:", rotation.ptr(), 75.0f);
			ImGui::Separator();
			App->gui->DrawInputFloat2("Scale:", scale.ptr(), 75.0f);
		}
		else
		{
			App->gui->DrawInputFloat2("Position:", partPosition.ptr(), 75.0f);
			ImGui::Separator();
			App->gui->DrawInputFloat("Rotation:", &rotation.z, 90.0f);
			ImGui::Separator();
			App->gui->DrawInputFloat2("Scale:", scale.ptr(), 75.0f);
		}
	}
	else
	{
		static const char* spawnOptions[] = { "Origin", "Edge", "Random" };
		ImGui::Text("Spawn point:"); ImGui::SameLine();
		if (ImGui::BeginCombo("##spawnPosition", spawnOptions[spawnPoint]))
		{
			if (ImGui::Selectable("Origin", spawnPoint == SPAWN_ORIGIN))
			{
				spawnPoint = SPAWN_ORIGIN;
			}
			if (ImGui::Selectable("Edge", spawnPoint == SPAWN_EDGE))
			{
				spawnPoint = SPAWN_EDGE;
			}
			if (ImGui::Selectable("Random", spawnPoint == SPAWN_RANDOM))
			{
				spawnPoint = SPAWN_RANDOM;
			}

			ImGui::EndCombo();
		}
		ImGui::Separator();

		if (App->camera->type == CAMERA_3D)
		{			
			if (billboard)
				App->gui->DrawInputFloatRandomOption("Rotation:", &rotation.z, &randRotation1.z, &randRotation2.z, &randomRot, 75.0f);
			else
				App->gui->DrawInputFloat3RandomOption("Rotation:", rotation.ptr(), randRotation1.ptr(), randRotation2.ptr(), &randomRot, 75.0f);
			ImGui::Text("Billboard:"); ImGui::SameLine(100.0f); ImGui::Checkbox("##billboard", &billboard);
			ImGui::Separator();
			App->gui->DrawInputFloat2RandomOption("Scale:", scale.ptr(), randScale1.ptr(), randScale2.ptr(), &randomScale, 75.0f);
		}
		else
		{
			App->gui->DrawInputFloatRandomOption("Rotation:", &rotation.z, &randRotation1.z, &randRotation2.z, &randomRot, 75.0f);
			ImGui::Separator();
			App->gui->DrawInputFloat2RandomOption("Scale:", scale.ptr(), randScale1.ptr(), randScale2.ptr(), &randomScale, 75.0f);
		}
	}
}

void TransformParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addBool("update", update);
	node->addBool("billboard", billboard);

	node->addUint("spawnPoint", spawnPoint);

	node->addVector3("partPosition", partPosition);
	node->addVector3("randPosition1", randPosition1);
	node->addVector3("randPosition2", randPosition2);
	node->addBool("randomPos", randomPos);

	node->addVector3("rotation", rotation);
	node->addVector3("randRotation1", randRotation1);
	node->addVector3("randRotation2", randRotation2);
	node->addBool("randomRot", randomRot);

	node->addVector2("scale", scale);
	node->addVector2("randScale1", randScale1);
	node->addVector2("randScale2", randScale2);
	node->addBool("randomScale", randomScale);
}

void TransformParticleNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	update = nodeDef->getBool("update");
	billboard = nodeDef->getBool("billboard");

	spawnPoint = (particleSpawnPoint)nodeDef->getUint("spawnPoint");

	partPosition = nodeDef->getVector3("partPosition");
	randPosition1 = nodeDef->getVector3("randPosition1");
	randPosition2 = nodeDef->getVector3("randPosition2");
	randomPos = nodeDef->getBool("randomPos");

	rotation = nodeDef->getVector3("rotation");
	randRotation1 = nodeDef->getVector3("randRotation1");
	randRotation2 = nodeDef->getVector3("randRotation2");
	randomRot = nodeDef->getBool("randomRot");

	scale = nodeDef->getVector2("scale");
	randScale1 = nodeDef->getVector2("randScale1");
	randScale2 = nodeDef->getVector2("randScale2");
	randomScale = nodeDef->getBool("randomScale");
}

void TransformParticleNode::SetSpawnPoint(vec direction)
{
	switch (spawnPoint)
	{
	case SPAWN_EDGE:
		particle->baseTransform->position += direction;
		break;
	case SPAWN_RANDOM:
		particle->baseTransform->position += direction * Lerp(0.0f, 1.0f, GET_RANDOM());
		break;
	}
}

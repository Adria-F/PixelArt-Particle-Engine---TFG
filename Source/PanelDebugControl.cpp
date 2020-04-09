#include "Application.h"
#include "PanelDebugControl.h"

#include "ModuleParticles.h"

#include "BaseColorParticleNode.h"
#include "BaseMovementParticleNode.h"
#include "BaseTransformParticleNode.h"
#include "ColorParticleNode.h"
#include "MakeGlobalParticleNode.h"
#include "SpeedParticleNode.h"

#include "BaseTransformEmitterNode.h"
#include "EmissionEmitterNode.h"

PanelDebugControl::PanelDebugControl(const char* name): Panel(name)
{
}

void PanelDebugControl::DrawContent()
{
	if (ImGui::Button("Play"))
		App->particles->Play();
	if (ImGui::Button("Pause"))
		App->particles->Pause();
	if (ImGui::Button("Stop"))
		App->particles->Stop();

	/*ParticleEmitter* emitter = App->particles->GetEmitter(0);
	Particle* particle = emitter->GetTemplate();

	
	if (ImGui::DragFloat3("Emitter position", &emitter->baseTransform->position.x, 0.1f))
	{
		emitter->baseTransform->CalculateMatrix();
	}
	
	ImGui::DragFloat("Frequency", &emitter->frequency, 0.1f);

	ImVec4 col;
	col.x = particle->baseColor->color.rgb.x;
	col.y = particle->baseColor->color.rgb.y;
	col.z = particle->baseColor->color.rgb.z;
	col.w = particle->baseColor->color.a;

	if (ImGui::ColorEdit4("Pick Color", &col.x))
	{
		particle->baseColor->color.rgb = { col.x,col.y,col.z };
		particle->baseColor->color.a = col.w;
	}

	ImGui::DragFloat("Particle Lifetime", &particle->lifeTime);

	ImGui::Text("Particle Count: %d", emitter->GetParticleCount());*/
}
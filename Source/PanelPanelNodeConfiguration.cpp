#include "Application.h"
#include "PanelPanelNodeConfiguration.h"

#include "ModuleParticles.h"
#include "ModuleNodeCanvas.h"
#include "ModuleGUI.h"

#include "BaseColorParticleNode.h"
#include "BaseMovementParticleNode.h"
#include "BaseTransformParticleNode.h"
#include "ColorParticleNode.h"
#include "MakeGlobalParticleNode.h"
#include "SpeedParticleNode.h"

#include "BaseTransformEmitterNode.h"
#include "EmissionEmitterNode.h"

PanelNodeConfiguration::PanelNodeConfiguration(const char* name): Panel(name)
{
}

void PanelNodeConfiguration::DrawContent()
{
	if (App->nodeCanvas->selectedNode == nullptr)
		return;

	ImFont* scaledFont = App->gui->GetFont(100);
	if (scaledFont != nullptr)
		ImGui::PushFont(scaledFont);

	ImGui::Text(App->nodeCanvas->selectedNode->name.c_str());

	if (scaledFont != nullptr)
		ImGui::PopFont();
	ImGui::NewLine();

	App->nodeCanvas->selectedNode->DisplayConfig();
}
#include "Application.h"
#include "PanelPanelNodeConfiguration.h"

#include "ModuleParticles.h"
#include "ModuleNodeCanvas.h"

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

	App->nodeCanvas->selectedNode->DisplayConfig();
}
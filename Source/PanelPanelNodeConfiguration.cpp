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

void PanelNodeConfiguration::SetFlags()
{
	flags = ImGuiWindowFlags_None;
}

void PanelNodeConfiguration::DrawContent()
{	
	if (ImGui::IsWindowCollapsed() && !collapsed)
	{
		if (openedPosition.x == math::inf)
		{
			ImGui::SetWindowCollapsed(false);
			collapsed = true;
		}
		else
		{
			collapsed = true;
			openedPosition = position;
			float4 canvas = App->gui->GetCanvasRegion();
			ImGui::SetWindowPos({ canvas.x + 10.0f, canvas.y + canvas.w - 30.0f });
		}
	}
	else if (!ImGui::IsWindowCollapsed() && collapsed)
	{
		if (openedPosition.x == math::inf)
		{
			openedPosition = { position.x, position.y - size.y + 20.0f };
		}
		collapsed = false;
		ImGui::SetWindowPos({ openedPosition.x, openedPosition.y });
	}
	else if (openedPosition.x == math::inf && !ImGui::IsWindowCollapsed())
	{
		openedPosition = position;
	}

	if (App->nodeCanvas->selectedNode == nullptr)
		return;

	ImFont* scaledFont = App->gui->GetFont(100, CANVAS_FONT_SIZE);
	if (scaledFont != nullptr)
		ImGui::PushFont(scaledFont);

	ImGui::Text(App->nodeCanvas->selectedNode->name.c_str());

	if (scaledFont != nullptr)
		ImGui::PopFont();
	ImGui::NewLine();

	App->nodeCanvas->selectedNode->DisplayConfig();
}
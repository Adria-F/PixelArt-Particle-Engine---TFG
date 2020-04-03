#include "Application.h"
#include "PanelScene.h"
#include "ModuleRender.h"
#include "ModuleGUI.h"

PanelScene::PanelScene(const char* name): Panel(name)
{
	
}

void PanelScene::DrawContent()
{
	App->gui->mouseOnScene = ImGui::IsWindowHovered(); //This bool is used to enable scene navigation using input

	ImGui::Image((ImTextureID)App->render->texture, { 960,540 }, { 0,1 }, { 1,0 });
}

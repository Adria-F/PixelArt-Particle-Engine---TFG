#include "Application.h"
#include "PanelScene.h"
#include "ModuleRender.h"

PanelScene::PanelScene(const char* name): Panel(name)
{
	
}

void PanelScene::DrawContent()
{
	ImGui::Image((ImTextureID)App->render->texture, { 960,540 }, { 0,1 }, { 1,0 });
}

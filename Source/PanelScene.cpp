#include "Application.h"
#include "PanelScene.h"
#include "ModuleRender.h"
#include "ModuleGUI.h"
#include "ModuleCamera.h"

PanelScene::PanelScene(const char* name): Panel(name)
{
	
}

void PanelScene::SetFlags()
{
	flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
}

void PanelScene::DrawContent()
{
	App->gui->mouseOnScene = ImGui::IsWindowHovered(); //This bool is used to enable scene navigation using input

	ImGui::SetCursorPos({ 0,0 }); //Set image position at top left corner
	ImGui::Image((ImTextureID)App->render->texture, { size.x, size.y }, { 0,1 }, { 1,0 });
}

void PanelScene::OnResize()
{
	App->camera->setAspectRatio(size.x / size.y);
	App->render->OnResize(size.x, size.y);
}
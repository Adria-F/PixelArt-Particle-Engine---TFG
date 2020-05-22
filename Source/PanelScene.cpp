#include "Application.h"
#include "PanelScene.h"

#include "ModuleRender.h"
#include "ModuleGUI.h"
#include "ModuleCamera.h"
#include "ModuleNodeCanvas.h"

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
	
	//Store position and size for accessing from other modules
	ImVec2 wPos = ImGui::GetCursorScreenPos();
	App->gui->scenePosition = { wPos.x, wPos.y };
	App->gui->sceneSize = size;

	ImGui::Image((ImTextureID)App->render->texture, { size.x, size.y }, { 0,1 }, { 1,0 });

	ImGuizmo::SetDrawlist();
	App->nodeCanvas->DrawGuizmo();
}

void PanelScene::OnResize()
{
	App->camera->OnResize(size.x, size.y);
	App->render->OnResize(size.x, size.y);
}
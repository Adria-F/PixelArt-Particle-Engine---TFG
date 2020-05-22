#include "Application.h"
#include "PanelPixelArt.h"

#include "ModuleRender.h"
#include "ModuleGUI.h"

PanelPixelArt::PanelPixelArt(const char* name): Panel(name)
{
}

void PanelPixelArt::SetFlags()
{
	flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
}

void PanelPixelArt::DrawContent()
{
	App->gui->mouseOnPixelScene = ImGui::IsWindowHovered(); //This bool is used to enable scene navigation using input

	float2 imageSize = App->gui->sceneSize;
	float AR = imageSize.y / imageSize.x;
	float min = min(ImGui::GetWindowContentRegionWidth(), imageSize.x);
	imageSize = { min, min*AR };

	ImGui::SetCursorPos({ (size.x - imageSize.x) / 2, (size.y - imageSize.y) / 2 });
	ImGui::Image((ImTextureID)App->render->pixelartTexture, { imageSize.x, imageSize.y }, { 0,1 }, { 1,0 });
}

void PanelPixelArt::OnResize()
{
}

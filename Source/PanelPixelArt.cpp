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
	float AR = imageSize.x / imageSize.y;
	imageSize.x = ImGui::GetWindowContentRegionWidth();
	imageSize.y = imageSize.x / AR;
	if (imageSize.y > ImGui::GetWindowHeight() - 50.0f)
	{
		imageSize = { (ImGui::GetWindowHeight() - 50.0f)*AR, ImGui::GetWindowHeight() - 50.0f };
	}

	ImGui::SetCursorPos({ (size.x - imageSize.x) / 2, (size.y - imageSize.y) / 2 });
	ImGui::Image((ImTextureID)App->render->pixelartTexture, { imageSize.x, imageSize.y }, { 0,1 }, { 1,0 });

	ImGui::SetCursorScreenPos({ ImGui::GetCursorScreenPos().x , ImGui::GetWindowPos().y + ImGui::GetWindowSize().y-20.0f });
	ImGui::Text("Pixel Size:"); ImGui::SameLine();
	static int pixelSize = App->render->pixelSize;
	ImGui::PushItemWidth(100.0f);
	if (ImGui::InputInt("##pixelSize", &pixelSize, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (pixelSize < 1.0f)
			pixelSize = 1.0f;
		App->render->pixelSize = pixelSize;
	}
	ImGui::PopItemWidth();
}

void PanelPixelArt::OnResize()
{
}

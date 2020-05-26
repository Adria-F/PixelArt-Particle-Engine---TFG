#include "Application.h"
#include "PanelExportSpritesheet.h"

#include "ModuleProjectManager.h"
#include "SpriteSheetExporter.h"

PanelExportSpritesheet::PanelExportSpritesheet(const char* name): Panel(name)
{
	exporter = new SpriteSheetExporter(duration, frames, frameSize, pixelSize, rows, columns);
}

PanelExportSpritesheet::~PanelExportSpritesheet()
{
	RELEASE(exporter);
}

void PanelExportSpritesheet::SetFlags()
{
}

void PanelExportSpritesheet::DrawContent()
{
	ImGui::Text("Path:"); ImGui::SameLine();
	if (ImGui::Button(path.c_str(), { ImGui::GetWindowContentRegionWidth() - 100, 0.0f }))
	{
		std::string newPath = App->projectManager->SaveFileDialog(SPRITESHEET_FORMAT_HINT, SPRITESHEET_EXTENSION);
		if (newPath.length() > 0)
			path = newPath;
	}
	
	ImGui::Text("Effect duration:"); ImGui::SameLine();
	ImGui::InputFloat("##duration", &duration, 0.1f); ImGui::SameLine();
	ImGui::Text("seconds");

	ImGui::Text("Frames:"); ImGui::SameLine();
	ImGui::InputInt("##frames", &frames);

	ImGui::Text("Pixel size:"); ImGui::SameLine();
	ImGui::InputInt("##pixelSize", &pixelSize);

	ImGui::Text("Reoslution:"); ImGui::SameLine();
	ImGui::InputFloat2("##resolution", frameSize.ptr());
	if ((int)frameSize.x % pixelSize != 0 || (int)frameSize.y%pixelSize != 0)
	{
		ImGui::SameLine();
		ImGui::TextColored({ 0.75f,0.0f,0.0f,1.0f }, "Not multiple of pixel size"); ImGui::SameLine();
		if (ImGui::Button("Fix"))
		{
			frameSize.x = (int)(frameSize.x / pixelSize) * pixelSize;
			frameSize.y = (int)(frameSize.y / pixelSize) * pixelSize;
		}
	}

	ImGui::Separator();
	float section = ImGui::GetWindowContentRegionWidth() / 3.0f;
	ImGui::BeginChild("imageDisposition", { section, 0.0f });
	ImGui::Text("Frames disposition");
	static const char* types[] = { "Square", "Single row", "Single column", "Custom" };
	if (ImGui::BeginCombo("##dispositionType", typeDisplay.c_str()))
	{
		for (int i = 0; i < 4; i++)
		{
			if (ImGui::Selectable(types[i], i == disposition))
			{
				disposition = (spritesheetType)i;
				typeDisplay = types[i];
			}
		}
		ImGui::EndCombo();
	}
	if (disposition == CUSTOM)
	{
		ImGui::Text("Rows:"); ImGui::SameLine();
		ImGui::InputInt("##rows", &rows);
		ImGui::Text("Columns:"); ImGui::SameLine();
		ImGui::InputInt("##columns", &columns);
	}
	ImGui::EndChild(); ImGui::SameLine();

	ImGui::BeginChild("preview", { section, 0.0f });
	ImGui::Text("Preview");
	ImGui::Button("Refresh");
	//ImGui::Image((ImTextureID)App->render->pixelartTexture, { section, imageSize.y }, { 0,1 }, { 1,0 });
	ImGui::EndChild(); ImGui::SameLine();

	ImGui::BeginChild("outputFile", { section, 0.0f });
	ImGui::Text("Current spritesheet");
	float2 resultSize = { columns*frameSize.x, rows*frameSize.y };
	ImGui::Text(("Size: " + std::to_string(resultSize.x) + "x" + std::to_string(resultSize.y)).c_str());
	ImGui::Checkbox("Make multiple of 4", &multipleOf4);
	ImGui::EndChild();

	if (ImGui::Button("Export Spritesheet"))
	{
		if (path.length() > 0)
			exporter->ExportSpriteSheet(path.c_str());
	}
}

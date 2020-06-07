#include "Application.h"
#include "PanelExportSpritesheet.h"

#include "ModuleProjectManager.h"
#include "SpriteSheetExporter.h"
#include "ModuleGUI.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"

PanelExportSpritesheet::PanelExportSpritesheet(const char* name): Panel(name, false)
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
	if (App->gui->mouseOnExportScene && ImGui::IsMouseClicked(2))
		App->gui->clickedScene = true;
	if (!ImGui::IsMouseDown(2))
		App->gui->clickedScene = false;
	
	// To detect changes that require extra operations
	bool cameraChanged = false;

	//Update Camera and Render	
	exporter->camera->Update(-1.0f);
	App->render->DrawScene(exporter->camera->getProjectionMatrix(), exporter->camera->getViewMatrix());
	App->render->DrawPixelArt(frameSize, pixelSize);

	float windowWidth = ImGui::GetWindowContentRegionWidth();

	// Configuration --------------------------------------------
	ImGui::BeginChild("configuration", { windowWidth*5.0f/8.0f, 150.0f }); // 5/8 of the screen width

	App->gui->DrawInputFloat("Effect duration:", &duration, 130.0f); ImGui::SameLine();
	ImGui::Text("seconds");

	App->gui->DrawInputInt("Frames:", &frames, 130.0f);

	App->gui->DrawInputInt("Pixel size:", &pixelSize, 130.0f);

	if (App->gui->DrawInputFloat("Frame resolution:", &frameSize.x, 130.0f))
		cameraChanged = true;
	ImGui::SameLine();
	ImGui::PushItemWidth(50.0f);
	if (ImGui::InputFloat("##frameResY", &frameSize.y))
		cameraChanged = true;
	ImGui::PopItemWidth();
	if ((int)frameSize.x % pixelSize != 0 || (int)frameSize.y%pixelSize != 0)
	{
		ImGui::TextColored({ 0.75f,0.0f,0.0f,1.0f }, "Not multiple of pixel size"); ImGui::SameLine();
		if (ImGui::Button("Fix"))
		{
			frameSize.x = (int)(frameSize.x / pixelSize) * pixelSize;
			frameSize.y = (int)(frameSize.y / pixelSize) * pixelSize;
			cameraChanged = true;
		}
	}

	ImGui::EndChild(); ImGui::SameLine();

	if (cameraChanged)
	{
		exporter->camera->OnResize(frameSize.x, frameSize.y);
		App->render->GenerateExportFrameBuffer(frameSize.x, frameSize.y);
	}

	// Frames disposition ----------------------------------------
	ImGui::BeginChild("dispositionConfig", { windowWidth*3.0f/8.0f, 150.0f }); // 3/8 of the screen width

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
		App->gui->DrawInputInt("Rows:", &customRows, 65.0f);
		App->gui->DrawInputInt("Columns:", &customColumns, 65.0f);
	}
	ImGui::EndChild();

	// Previews ---------------------------------------------------
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, { 0.1f,0.1,0.1f,1.0f });
	float menuHeight = 400.0f;
	ImGui::BeginChild("previews", { 0.0f, menuHeight });
	/// Preview Spritesheet ---------------------------------------
	ImGui::BeginChild("spritesheetPreview", { windowWidth *5.0f / 8.0f, menuHeight });

	ImGui::Text("Preview:"); ImGui::SameLine();
	if (ImGui::Button("Refresh"))
	{
		RefreshSpriteSheet();
	}
	float AR = (float)exporter->spritesheetTexture->width / (float)exporter->spritesheetTexture->height;
	float2 textureSize;
	textureSize.x = windowWidth * 5.0f / 8.0f;
	textureSize.y = textureSize.x / AR;
	if (textureSize.y > menuHeight-50.0f)
	{
		textureSize = { (menuHeight-50.0f)*AR, menuHeight-50.0f };
	}
	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::ImageButton((ImTextureID)exporter->spritesheetTexture->GL_id, { textureSize.x, textureSize.y }, { 0,1 }, { 1,0 }, 0, { 0.0f,0.0f,0.0f,1.0f });
	ImGui::Text(("Size: " + std::to_string(exporter->spritesheetTexture->width) + "x" + std::to_string(exporter->spritesheetTexture->height)).c_str());
	
	ImGui::EndChild(); ImGui::SameLine();

	/// Preview camera --------------------------------------------
	ImGui::BeginChild("cameraPreview", { windowWidth*3.0f/8.0f, menuHeight });
	ImGui::Text("Center effect on camera");
	AR = frameSize.x / frameSize.y;
	textureSize.x = windowWidth * 3.0f / 8.0f;
	textureSize.y = textureSize.x / AR;
	if (textureSize.y > menuHeight - 50.0f)
	{
		textureSize = { (menuHeight - 50.0f)*AR, menuHeight - 50.0f };
	}
	ImGui::ImageButton((ImTextureID)App->render->exportPixelartTexture, { textureSize.x, textureSize.y }, { 0,1 }, { 1,0 }, 0, { 0.0f,0.0f,0.0f,1.0f });
	App->gui->mouseOnExportScene = ImGui::IsItemHovered();

	ImGui::EndChild();

	ImGui::EndChild();
	ImGui::PopStyleColor();

	// Export menu ---------------------------------------------------
	ImGui::Text("Path:"); ImGui::SameLine();
	if (ImGui::Button(path.c_str(), { ImGui::GetWindowContentRegionWidth() - 150, 0.0f }))
	{
		std::string newPath = App->projectManager->SaveFileDialog(SPRITESHEET_FORMAT_HINT, SPRITESHEET_EXTENSION);
		if (newPath.length() > 0)
			path = newPath;
	}

	ImGui::SameLine();
	if (ImGui::Button("Export"))
	{
		if (path.length() > 0)
		{
			RefreshSpriteSheet();
			exporter->ExportSpriteSheet(path.c_str());
			ToggleActive();
		}
	}
}

void PanelExportSpritesheet::RefreshSpriteSheet()
{
	exporter->forceSquared = false;
	switch (disposition)
	{
	case SINGLE_ROW:
		rows = 1;
		columns = frames;
		break;
	case SINGLE_COLUMN:
		rows = frames;
		columns = 1;
		break;
	case CUSTOM:
		rows = customRows;
		columns = customColumns;
		break;
	case SQUARE:
		float maxValue = max(frames*frameSize.x, frameSize.y);
		float prevArea = maxValue * maxValue;
		int i;
		for (i = frames; i >= 1; i--)
		{
			columns = i;
			rows = ceil((float)frames / (float)i);
			float maxValue = max(columns*frameSize.x, rows*frameSize.y);
			float area = maxValue * maxValue;
			if (area <= prevArea)
				prevArea = area;
			else
				break;
		}
		columns = i + 1;
		rows = ceil((float)frames / (float)columns);
		exporter->forceSquared = true;
		break;
	}

	exporter->duration = duration;
	exporter->frameNum = frames;
	exporter->rows = rows;
	exporter->columns = columns;
	exporter->frameSize = frameSize;
	exporter->pixelSize = pixelSize;

	exporter->CreateSpriteSheet();
}

void PanelExportSpritesheet::OnOpen()
{
	exporter->camera->OnResize(frameSize.x, frameSize.y);
	App->render->GenerateExportFrameBuffer(frameSize.x, frameSize.y);
}

void PanelExportSpritesheet::OnClose()
{
	App->render->OnResize(App->gui->sceneSize.x, App->gui->sceneSize.y);
}

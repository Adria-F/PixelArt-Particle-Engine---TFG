#include "Application.h"
#include "ModuleGUI.h"

#include "ImGui\imgui_impl_sdl.h"
#include "ImGui\imgui_impl_opengl3.h"

#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleProjectManager.h"
#include "ModuleParticles.h"
#include "ModuleTextures.h"
#include "ModuleCamera.h"

//TMP
#include "SpriteSheetExporter.h"

// Include all panels
#include "PanelScene.h"
#include "PanelPanelNodeConfiguration.h"
#include "PanelNodeCanvas.h"
#include "PanelPixelArt.h"
#include "PanelExportSpritesheet.h"

ModuleGUI::ModuleGUI(bool start_enabled): Module(start_enabled)
{
}

ModuleGUI::~ModuleGUI()
{
}

bool ModuleGUI::Init()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.IniFilename = "Settings/imgui.ini";
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoDecoration = false;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->render->context);
	ImGui_ImplOpenGL3_Init();

	//Load default font
	AddFont();
	defaultFont = GetFont();

	//Add all panels
	panels.push_back(new PanelScene("Scene"));
	panels.push_back(new PanelNodeConfiguration("Node Configuration"));
	panels.push_back(new PanelNodeCanvas("Node Canvas"));
	panels.push_back(new PanelPixelArt("PixelArt"));
	panelExport = new PanelExportSpritesheet("Export Spritesheet");
	panels.push_back(panelExport);

	return true;
}

bool ModuleGUI::Start()
{
	//Set Default Style
	
	//Load GUI textures

	return true;
}

update_state ModuleGUI::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	ImGui::PushFont(defaultFont);

	//Draw Central Docking Window
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({ (float)App->window->width, (float)App->window->height });
	//ImGui::SetNextWindowBgAlpha(0.0f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", 0, window_flags);
	ImGui::PopStyleVar(3);	

	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

	return UPDATE_CONTINUE;
}

update_state ModuleGUI::Update(float dt)
{	
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
				//TODO: Request to save if changes made
				//Create empty project
				App->projectManager->NewProject();
			}
			if (ImGui::MenuItem("Load"))
			{
				//TODO: Request to save if changes made
				std::string newProject = App->projectManager->OpenFileDialog(PROJECT_FORMAT_HINT, (std::string("*")+PROJECT_EXTENSION).c_str());
				if (newProject.length() > 0)
				{
					App->projectManager->LoadProject(newProject.c_str());
				}
			}
			if (ImGui::MenuItem("Save"))
			{
				if (App->projectManager->workingDir.length() == 0)
				{
					App->projectManager->workingDir = App->projectManager->SaveFileDialog(PROJECT_FORMAT_HINT, PROJECT_EXTENSION);
				}
				if (App->projectManager->workingDir.length() > 0)
				{
					App->projectManager->SaveProject(App->projectManager->workingDir.c_str());
				}
			}
			if (ImGui::MenuItem("Save As"))
			{
				App->projectManager->workingDir = App->projectManager->SaveFileDialog(PROJECT_FORMAT_HINT, PROJECT_EXTENSION);
				if (App->projectManager->workingDir.length() > 0)
				{
					App->projectManager->SaveProject(App->projectManager->workingDir.c_str());
				}
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Particles"))
		{
			if (ImGui::MenuItem("Play"))
			{
				App->particles->Play();
			}
			if (ImGui::MenuItem("Pause"))
			{
				App->particles->Pause();
			}
			if (ImGui::MenuItem("Stop"))
			{
				App->particles->Stop();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Sprites"))
		{
			if (ImGui::MenuItem("Import Sprites"))
			{
				std::string texture = App->projectManager->OpenFileDialog("","");
				if (texture.length() > 0)
				{
					App->textures->ImportTexture(texture.c_str());
				}
			}
			if (ImGui::MenuItem("Export Spritesheet", nullptr, panelExport->IsActive()))
			{
				panelExport->ToggleActive();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::MenuItem("Camera 2D", nullptr, App->camera->type == CAMERA_2D))
			{
				App->camera->type = CAMERA_2D;
			}
			if (ImGui::MenuItem("Camera 3D", nullptr, App->camera->type == CAMERA_3D))
			{
				App->camera->type = CAMERA_3D;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	return UPDATE_CONTINUE;
}

update_state ModuleGUI::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleGUI::CleanUp()
{
	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); ++it_p)
	{
		RELEASE((*it_p));
	}
	panels.clear();

	return true;
}

void ModuleGUI::Draw()
{
	for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); ++it_p)
	{
		if ((*it_p)->IsActive())
		{
			(*it_p)->DrawPanel();
		}
	}

	ImGui::End();

	ImGui::PopFont();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(App->window->window, App->render->context);
	}
}

void ModuleGUI::handleInput(SDL_Event * event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

void ModuleGUI::AddFont(int fontPercent, int fontSize)
{
	ImGuiIO &io = ImGui::GetIO();
	ImFontConfig config;
	config.SizePixels = fontPercent/100.0f*(fontSize);
	config.OversampleH = config.OversampleV = 1;
	config.PixelSnapH = true;
	ImFont *font = io.Fonts->AddFontDefault(&config);
	
	fonts.insert(std::pair<int, ImFont*>(fontPercent*fontSize, font));
}

ImFont* ModuleGUI::GetFont(int fontPercent, int fontSize) const
{
	if (fonts.find(fontPercent*fontSize) != fonts.end())
	{
		return fonts.at(fontPercent*fontSize);
	}

	return nullptr;
}

bool ModuleGUI::UsingMouse() const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureMouse && !mouseOnScene && !mouseOnPixelScene && !mouseOnExportScene;
}

bool ModuleGUI::UsingKeyboard() const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantTextInput && !mouseOnScene && !mouseOnPixelScene && !mouseOnExportScene;
}

bool ModuleGUI::IsExportPanelActive() const
{
	return panelExport->IsActive();
}

bool ModuleGUI::DrawInputFloat(const char* label, const char* id, float* value, float step, bool enabled, float* alternativeValue, bool condition)
{
	bool ret = false;

	if (!enabled)
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
	
	if (std::string(label).size() > 0)
	{
		if (enabled)
			ImGui::Text(label);
		else
			ImGui::TextDisabled(label);
		ImGui::SameLine();
	}
	ImGui::PushItemWidth(50.0f);
	if (enabled)
	{
		float* modifyingValue = value;
		if (alternativeValue != nullptr && condition)
			modifyingValue = alternativeValue;
		ret = ImGui::DragFloat(id, modifyingValue, step, step, 0.0f, "%.2f");
		*value = *modifyingValue;
		if (alternativeValue != nullptr)
			*alternativeValue = *modifyingValue;
	}
	else
	{
		char str[1] = "";
		ImGui::InputText(id, &str[0], 1, ImGuiInputTextFlags_ReadOnly);
	}
	ImGui::PopItemWidth();

	if (!enabled)
		ImGui::PopStyleVar();

	return ret;
}

void ModuleGUI::DrawColorBox(Color& color)
{
	ImGui::PushItemWidth(150.0f);
	float colorArray[4] = { color.rgb.x, color.rgb.y, color.rgb.z, color.a };
	ImGui::ColorEdit4("##color", &colorArray[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar);
	color.rgb = { colorArray[0],colorArray[1],colorArray[2] };
	color.a = colorArray[3];
	ImGui::PopItemWidth();
}

void ModuleGUI::DrawGradientBox(Gradient& gradient)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float itemWidth = 150.0f;
	float itemHeight = 17.0f;
	float buttonWidth = 5.0f;
	float buttonHeight = 10.0f; //TODO: Currently not applying

	ImVec2 startPos = ImGui::GetCursorScreenPos();
	startPos.y += buttonHeight * 2.0f;
	draw_list->AddCheckerboardRect(startPos, { startPos.x + itemWidth, startPos.y + itemHeight });

	std::map<float, vec>::iterator prevC = gradient.colorList.end();
	std::map<float, float>::iterator prevA = gradient.alphaList.begin();
	std::map<float, float>::iterator currA = ++gradient.alphaList.begin();
	for (std::map<float, vec>::iterator currC = gradient.colorList.begin(); currC != gradient.colorList.end();)
	{
		float prevPercent;
		float currPercent;
		ImVec4 prevColor;
		ImVec4 currColor;

		prevC = currC++;
		if (currC == gradient.colorList.end())
		{
			prevPercent = (*prevC).first;
			if (prevPercent == 1.0f)
				break;
			currPercent = 1.0f;
			prevColor = { (*prevC).second.x, (*prevC).second.y, (*prevC).second.z, 1.0f };
			prevColor.w = gradient.GetColor(prevPercent).a;
			currColor = prevColor;
		}
		else
		{
			prevPercent = (*prevC).first;
			currPercent = (*currC).first;
			prevColor = { (*prevC).second.x, (*prevC).second.y, (*prevC).second.z, 1.0f };
			prevColor.w = gradient.GetColor(prevPercent).a;
			currColor = { (*currC).second.x, (*currC).second.y, (*currC).second.z, 1.0f };
			currColor.w = gradient.GetColor(currPercent).a;
		}

		//Draw alpha boxes
		float prevAPercent = (*prevA).first;
		float currAPercent = (*currA).first;
		if (prevAPercent >= prevPercent && prevAPercent < currPercent)
		{
			Color prevAlphaColor = gradient.GetColor(prevAPercent);
			ImVec4 prevAColor = { prevAlphaColor.rgb.x, prevAlphaColor.rgb.y, prevAlphaColor.rgb.z, prevAlphaColor.a };
			//Draw prevPercent->prevAPercent
			draw_list->AddRectFilledMultiColor({ startPos.x + itemWidth * prevPercent, startPos.y }, { startPos.x + itemWidth * prevAPercent, startPos.y + itemHeight }, ImGui::GetColorU32(prevColor), ImGui::GetColorU32(prevAColor), ImGui::GetColorU32(prevAColor), ImGui::GetColorU32(prevColor));

			if (currA != gradient.alphaList.end())
			{
				currAPercent = (*currA).first;
				Color currAlphaColor;
				ImVec4 currAColor;
				while (currAPercent < currPercent)
				{
					currAlphaColor = gradient.GetColor(currAPercent);
					currAColor = { currAlphaColor.rgb.x, currAlphaColor.rgb.y, currAlphaColor.rgb.z, currAlphaColor.a };
					//Draw prevAPercent->currAPercent
					draw_list->AddRectFilledMultiColor({ startPos.x + itemWidth * prevAPercent, startPos.y }, { startPos.x + itemWidth * currAPercent, startPos.y + itemHeight }, ImGui::GetColorU32(prevAColor), ImGui::GetColorU32(currAColor), ImGui::GetColorU32(currAColor), ImGui::GetColorU32(prevAColor));
					prevA = currA++;
					prevAPercent = currAPercent;
					prevAlphaColor = currAlphaColor;
					prevAColor = currAColor;
					if (currA == gradient.alphaList.end())
						break;
					currAPercent = (*currA).first;
				}
			}
			//Draw prevAPercent->currPercent
			draw_list->AddRectFilledMultiColor({ startPos.x + itemWidth * prevAPercent, startPos.y }, { startPos.x + itemWidth * currPercent, startPos.y + itemHeight }, ImGui::GetColorU32(prevAColor), ImGui::GetColorU32(currColor), ImGui::GetColorU32(currColor), ImGui::GetColorU32(prevAColor));

			if (currA != gradient.alphaList.end())
				prevA = currA++;
		}
		else
		{
			if (prevC == gradient.colorList.begin() && prevPercent > 0.0f)
				draw_list->AddRectFilledMultiColor({ startPos.x, startPos.y }, { startPos.x + itemWidth * prevPercent, startPos.y + itemHeight }, ImGui::GetColorU32(prevColor), ImGui::GetColorU32(prevColor), ImGui::GetColorU32(prevColor), ImGui::GetColorU32(prevColor));

			draw_list->AddRectFilledMultiColor({ startPos.x + itemWidth * prevPercent, startPos.y }, { startPos.x + itemWidth * currPercent, startPos.y + itemHeight }, ImGui::GetColorU32(prevColor), ImGui::GetColorU32(currColor), ImGui::GetColorU32(currColor), ImGui::GetColorU32(prevColor));
		}
	}

	//Draw Keys
	static float movingColorKey = -1.0f;
	static float movingAlphaKey = -1.0f;

	ImGui::PushItemWidth(buttonWidth);
	///Color
	int count = 0;
	for (std::map<float, vec>::iterator it_c = gradient.colorList.begin(); it_c != gradient.colorList.end(); ++it_c)
	{
		float percent = (*it_c).first;
		ImVec4 color = { (*it_c).second.x, (*it_c).second.y, (*it_c).second.z, 1.0 };

		ImGui::SetCursorScreenPos({ startPos.x + itemWidth * percent - buttonWidth * 0.5f, startPos.y + itemHeight - buttonHeight * 0.25f });
		if (ImGui::IsMouseDragging(0) && movingColorKey != -1.0f)
		{
			ImGui::ColorButton(("##gradientColor" + std::to_string(count)).c_str(), color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, { buttonWidth, 0.0f });
		}
		else
			ImGui::ColorEdit3(("##gradientColor" + std::to_string(count)).c_str(), (*it_c).second.ptr(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop);

		if (ImGui::IsItemClicked(0) && !ImGui::IsPopupOpen("picker"))
			movingColorKey = percent;

		count++;
	}
	///Alpha
	count = 0;
	for (std::map<float, float>::iterator it_c = gradient.alphaList.begin(); it_c != gradient.alphaList.end(); ++it_c)
	{
		float percent = (*it_c).first;
		ImVec4 alpha = { (*it_c).second, (*it_c).second, (*it_c).second, 1.0 };

		ImGui::SetCursorScreenPos({ startPos.x + itemWidth * percent - buttonWidth * 0.5f, startPos.y - buttonHeight*1.5f});
		if (ImGui::IsMouseDragging(0) && movingAlphaKey != -1.0f)
		{
			ImGui::ColorButton(("##gradientAlpha" + std::to_string(count)).c_str(), alpha, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, { buttonWidth, 0.0f });
		}
		else
			ImGui::AlphaEdit(("##gradientAlpha" + std::to_string(count)).c_str(), &(*it_c).second, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop);

		if (ImGui::IsItemClicked(0) && !ImGui::IsPopupOpen("picker"))
			movingAlphaKey = percent;

		count++;
	}
	ImGui::PopItemWidth();

	//Add keys
	ImGui::SetCursorScreenPos(startPos);
	if (ImGui::InvisibleButton("##addColor", { itemWidth, itemHeight }))
	{
		ImVec2 mousePos = ImGui::GetMousePos();
		float percent = (mousePos.x - startPos.x) / itemWidth;
		float verticalPercent = (mousePos.y - startPos.y) / itemHeight;
		if (verticalPercent >= 0.5f) //Bottom part of the box
		{
			vec color = gradient.GetColor(percent).rgb;
			gradient.SetColor(color, percent);
		}
		else //Top part of the box
		{
			float alpha = gradient.GetColor(percent).a;
			gradient.SetAlpha(alpha, percent);
		}
	}

	//Move keys
	///Color
	if (movingColorKey != -1.0)
	{
		float percent = (ImGui::GetMousePos().x - startPos.x) / itemWidth;
		if (!gradient.hasColorKey(percent) && ImGui::IsMouseDragging(0))
		{
			gradient.MoveColorKey(movingColorKey, percent);
			movingColorKey = percent;
		}		

		if (ImGui::IsMouseReleased(0))
			movingColorKey = -1.0f;
	}
	///Alpha
	if (movingAlphaKey != -1.0)
	{
		float percent = (ImGui::GetMousePos().x - startPos.x) / itemWidth;
		if (!gradient.hasAlphaKey(percent) && ImGui::IsMouseDragging(0))
		{
			gradient.MoveAlphaKey(movingAlphaKey, percent);
			movingAlphaKey = percent;
		}

		if (ImGui::IsMouseReleased(0))
			movingAlphaKey = -1.0f;
	}

	ImGui::SetCursorScreenPos(startPos); ImGui::Dummy({ 0.0f,0.0f });
}

bool ModuleGUI::DrawInputInt(const char* label, int* value, float indent)
{
	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::Text(label);

	ImGui::SetCursorPos({ cursor.x + indent, cursor.y });
	ImGui::PushItemWidth(50.0f);
	bool ret = ImGui::InputInt(("##" + std::string(label)).c_str(), value, 0);
	ImGui::PopItemWidth();

	return ret;
}

bool ModuleGUI::DrawInputFloat(const char * label, float * value, float indent)
{
	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::Text(label);

	ImGui::SetCursorPos({ cursor.x + indent, cursor.y });
	ImGui::PushItemWidth(50.0f);
	bool ret = ImGui::InputFloat(("##" + std::string(label)).c_str(), value);
	ImGui::PopItemWidth();

	return ret;
}

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
	panelCanvas = new PanelNodeCanvas("Node Canvas");
	panels.push_back(panelCanvas);
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
	BROFILER_CATEGORY("ModuleGUI Pre-Update", Profiler::Color::Blue);

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
	BROFILER_CATEGORY("ModuleGUI Update", Profiler::Color::Blue);

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
			if (ImGui::BeginMenu("Max Particles"))
			{
				static int maxParticles = App->particles->GetParticlePoolSize();
				if (ImGui::InputInt("", &maxParticles, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					App->particles->ResizeParticlePool(maxParticles);
				}
				ImGui::EndMenu();
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
		if (ImGui::BeginMenu("Windows"))
		{
			for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); ++it_p)
			{
				if ((*it_p) == panelExport)
					continue;

				if (ImGui::MenuItem((*it_p)->GetName(), nullptr, (*it_p)->IsActive()))
					(*it_p)->ToggleActive();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

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

void ModuleGUI::SaveSettings(JSON_Value* settings)
{
	JSON_Value* panelsValue = settings->createValue();

	for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); ++it_p)
	{
		JSON_Value* panel = panelsValue->createValue();
		(*it_p)->SaveSettings(panel);
		panelsValue->addValue((*it_p)->GetName(), panel);
	}

	settings->addValue("panels", panelsValue);
}

void ModuleGUI::LoadSettings(JSON_Value* settings)
{
	JSON_Value* panelsValue = settings->getValue("panels");

	if (panelsValue)
	{
		for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); ++it_p)
		{
			JSON_Value* panel = panelsValue->getValue((*it_p)->GetName());
			if (panel)
				(*it_p)->LoadSettings(panel);
		}
	}
}

void ModuleGUI::Draw()
{
	BROFILER_CATEGORY("Draw GUI", Profiler::Color::DeepPink);

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
	return io.WantCaptureMouse && !mouseOnScene && !mouseOnPixelScene && !mouseOnExportScene && !clickedScene;
}

bool ModuleGUI::UsingKeyboard() const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantTextInput && !mouseOnScene && !mouseOnPixelScene && !mouseOnExportScene && !clickedScene;
}

bool ModuleGUI::IsExportPanelActive() const
{
	return panelExport->IsActive();
}

void ModuleGUI::ResetCanvas()
{
	panelCanvas->Reset();
}

void ModuleGUI::DrawPlayMenu()
{
	if (ImGui::BeginMenuBar())
	{
		static ImDrawList* drawList = ImGui::GetWindowDrawList();
		static ImVec2 padding = { 4.6f,4.5f };
		static ImVec2 button = { 19.0f,19.0f };
		static ImVec2 quad = { 10.0f,10.0f };

		if (App->particles->IsPlaying())
		{
			ImVec2 pos = ImGui::GetCursorScreenPos();

			if (ImGui::Button("##pause", button))
			{
				App->particles->Pause();
			}

			drawList->AddLine({ pos.x + padding.x*1.3f, pos.y + padding.y}, { pos.x + padding.x*1.3f, pos.y + padding.y + quad.y }, IM_COL32(255, 255, 255, 255), 2.0f);
			drawList->AddLine({ pos.x + padding.x*1.3f + quad.x*0.6f, pos.y + padding.y }, { pos.x + padding.x*1.3f + quad.x*0.6f, pos.y + padding.y + quad.y }, IM_COL32(255, 255, 255, 255), 2.0f);
		}
		else
		{
			ImVec2 pos = ImGui::GetCursorScreenPos();

			if (ImGui::Button("##play", button))
			{
				App->particles->Play();
			}

			ImVec2 pointA = { pos.x + padding.x, pos.y + padding.y };
			ImVec2 pointB = { pos.x + padding.x + quad.x, pos.y + padding.y + quad.y / 2.0f };
			ImVec2 pointC = { pos.x + padding.x, pos.y + padding.y + quad.y};

			drawList->AddTriangleFilled(pointA, pointB, pointC, IM_COL32(255, 255, 255, 255));
		}
		ImGui::SameLine();

		ImVec2 pos = ImGui::GetCursorScreenPos();

		if (ImGui::Button("##stop", button))
		{
			App->particles->Stop();
		}
		
		drawList->AddRectFilled({ pos.x + padding.x, pos.y + padding.y }, { pos.x + padding.x + quad.x, pos.y + padding.y + quad.y }, IM_COL32(255, 255, 255, 255));

		ImGui::EndMenuBar();
	}
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

		//Remove Keys
		if (ImGui::IsItemClicked(1))
			ImGui::OpenPopup(("##colorKeyConfig"+std::to_string(percent)).c_str());
		if (ImGui::BeginPopup(("##colorKeyConfig" + std::to_string(percent)).c_str(), ImGuiWindowFlags_NoMove))
		{
			if (gradient.colorList.size() <= 1)
			{
				ImGui::Text("Remove");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Cannot remove last key");
					ImGui::EndTooltip();
				}
			}
			else
			{
				if (ImGui::Selectable("Remove"))
				{
					it_c = gradient.colorList.erase(it_c);
				}
			}
			ImGui::EndPopup();
		}

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

		//Remove Keys
		if (ImGui::IsItemClicked(1))
			ImGui::OpenPopup(("##alphaKeyConfig" + std::to_string(percent)).c_str());
		if (ImGui::BeginPopup(("##alphaKeyConfig" + std::to_string(percent)).c_str(), ImGuiWindowFlags_NoMove))
		{
			if (gradient.alphaList.size() <= 1)
			{
				ImGui::Text("Remove");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Cannot remove last key");
					ImGui::EndTooltip();
				}
			}
			else
			{
				if (ImGui::Selectable("Remove"))
				{
					it_c = gradient.alphaList.erase(it_c);
				}
			}
			ImGui::EndPopup();
		}

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

	if (indent > 0.0f)
		ImGui::SetCursorPos({ cursor.x + indent, cursor.y });
	else
		ImGui::SameLine();
	ImGui::PushItemWidth(50.0f);
	bool ret = ImGui::InputInt(("##" + std::string(label)).c_str(), value, 0);
	ImGui::PopItemWidth();

	return ret;
}

bool ModuleGUI::DrawInputFloat(const char* label, float* value, float indent, bool enabled)
{
	bool ret = false;
	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::Text(label);

	if (indent > 0.0f)
		ImGui::SetCursorPos({ cursor.x + indent, cursor.y });
	else
		ImGui::SameLine();
	
	ImGui::PushItemWidth(50.0f);
	if (enabled)
		ret = ImGui::InputFloat(("##" + std::string(label)).c_str(), value);
	else
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
		char str[1] = "";
		ImGui::InputText(("##" + std::string(label)).c_str(), &str[0], 1, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopStyleVar();
	}
	
	ImGui::PopItemWidth();

	return ret;
}

bool ModuleGUI::DrawInputFloatRandomOption(const char* label, float* value, float* rand1, float* rand2, bool* random, float indent)
{
	bool ret = false;

	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::Text(label);
	if (indent > 0.0f)
		ImGui::SetCursorPos({ cursor.x + indent, cursor.y });
	else
		ImGui::SameLine();
	float X = ImGui::GetCursorPosX();

	ImGui::PushID(("Value" + std::string(label)).c_str());
	ret |= DrawInputFloat(" ", (*random) ? rand1 : value);
	ImGui::SameLine();
	ImGui::PushID(("Rand" + std::string(label)).c_str());
	ret |= DrawInputFloat(" ", rand2, 0.0f, *random);
	ImGui::PopID();

	ImGui::SameLine(max(ImGui::GetWindowContentRegionWidth() - 70.0f, 300.0f));
	ImGui::Checkbox("Random", random);
	ImGui::PopID();

	return ret;
}

bool ModuleGUI::DrawInputFloat2(const char* label, float* value, float indent, bool enabled)
{
	bool ret = false;

	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::Text(label);
	if (indent > 0.0f)
		ImGui::SetCursorPos({ cursor.x + indent, cursor.y });
	else
		ImGui::SameLine();

	ImGui::PushID(("Value" + std::string(label)).c_str());
	ret |= DrawInputFloat("X", &value[0]);
	ImGui::SameLine();
	ret |= DrawInputFloat("Y", &value[1]);
	ImGui::PopID();

	return ret;
}

bool ModuleGUI::DrawInputFloat2RandomOption(const char* label, float* value, float* rand1, float* rand2, bool* random, float indent)
{
	bool ret = false;

	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::Text(label);
	if (indent > 0.0f)
		ImGui::SetCursorPos({ cursor.x + indent, cursor.y });
	else
		ImGui::SameLine();
	float X = ImGui::GetCursorPosX();

	ImGui::PushID(("Value" + std::string(label)).c_str());
	ret |= DrawInputFloat("X", (*random) ? &rand1[0] : &value[0]);
	ImGui::SameLine();
	ret |= DrawInputFloat("Y", (*random) ? &rand1[1] : &value[1]);

	ImGui::SameLine(max(ImGui::GetWindowContentRegionWidth() - 70.0f, 300.0f));
	ImGui::Checkbox("Random", random);
	ImGui::PopID();

	ImGui::PushID(("Rand" + std::string(label)).c_str());
	ImGui::SetCursorPosX(X);

	ret |= DrawInputFloat("X", &rand2[0], 0.0f, *random);
	ImGui::SameLine();
	ret |= DrawInputFloat("Y", &rand2[1], 0.0f, *random);
	
	ImGui::PopID();

	return ret;
}

bool ModuleGUI::DrawInputFloat3(const char* label, float* value, float indent, bool enabled)
{
	bool ret = false;

	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::Text(label);
	if (indent > 0.0f)
		ImGui::SetCursorPos({ cursor.x + indent, cursor.y });
	else
		ImGui::SameLine();

	ImGui::PushID(("Value" + std::string(label)).c_str());
	ret |= DrawInputFloat("X", &value[0]);
	ImGui::SameLine();
	ret |= DrawInputFloat("Y", &value[1]);
	ImGui::SameLine();
	ret |= DrawInputFloat("Z", &value[2]);
	ImGui::PopID();

	return ret;
}

bool ModuleGUI::DrawInputFloat3RandomOption(const char* label, float* value, float* rand1, float* rand2, bool* random, float indent)
{
	bool ret = false;

	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::Text(label);
	if (indent > 0.0f)
		ImGui::SetCursorPos({ cursor.x + indent, cursor.y });
	else
		ImGui::SameLine();
	float X = ImGui::GetCursorPosX();

	ImGui::PushID(("Value" + std::string(label)).c_str());
	ret |= DrawInputFloat("X", (*random) ? &rand1[0] : &value[0]);
	ImGui::SameLine();
	ret |= DrawInputFloat("Y", (*random) ? &rand1[1] : &value[1]);
	ImGui::SameLine();
	ret |= DrawInputFloat("Z", (*random) ? &rand1[2] : &value[2]);

	ImGui::SameLine(max(ImGui::GetWindowContentRegionWidth() - 70.0f, 300.0f));
	ImGui::Checkbox("Random", random);
	ImGui::PopID();

	ImGui::PushID(("Rand" + std::string(label)).c_str());
	ImGui::SetCursorPosX(X);

	ret |= DrawInputFloat("X", &rand2[0], 0.0f, *random);
	ImGui::SameLine();
	ret |= DrawInputFloat("Y", &rand2[1], 0.0f, *random);
	ImGui::SameLine();
	ret |= DrawInputFloat("Z", &rand2[2], 0.0f, *random);

	ImGui::PopID();

	return ret;
}

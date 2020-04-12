#include "Application.h"
#include "ModuleGUI.h"

#include "ImGui\imgui_impl_sdl.h"
#include "ImGui\imgui_impl_opengl3.h"

#include "ModuleWindow.h"
#include "ModuleRender.h"

// Include all panels
#include "PanelScene.h"
#include "PanelPanelNodeConfiguration.h"
#include "PanelNodeCanvas.h"

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
	ImFontConfig config;
	config.SizePixels = DEFAULT_FONT_SIZE;
	config.OversampleH = config.OversampleV = 1;
	config.PixelSnapH = true;
	defaultFont = io.Fonts->AddFontDefault(&config);

	//Add all panels
	panels.push_back(new PanelScene("Scene"));
	panels.push_back(new PanelNodeConfiguration("Node Configuration"));
	panels.push_back(new PanelNodeCanvas("Node Canvas"));

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

	ImGui::PushFont(defaultFont);

	//Draw Central Docking Window
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({ (float)App->window->width, (float)App->window->height });
	//ImGui::SetNextWindowBgAlpha(0.0f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking;
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

void ModuleGUI::AddFont(int fontPercent)
{
	ImGuiIO &io = ImGui::GetIO();
	ImFontConfig config;
	config.SizePixels = fontPercent/100.0f*(CANVAS_FONT_SIZE);
	config.OversampleH = config.OversampleV = 1;
	config.PixelSnapH = true;
	ImFont *font = io.Fonts->AddFontDefault(&config);
	
	fonts.insert(std::pair<int, ImFont*>(fontPercent, font));
}

ImFont* ModuleGUI::GetFont(int fontPercent) const
{
	if (fonts.find(fontPercent) != fonts.end())
	{
		return fonts.at(fontPercent);
	}

	return nullptr;
}

bool ModuleGUI::UsingMouse() const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureMouse && !mouseOnScene;
}

bool ModuleGUI::UsingKeyboard() const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantTextInput && !mouseOnScene;
}

void ModuleGUI::DrawInputFloat(const char* label, const char* id, float* value, float step, bool enabled, float* alternativeValue, bool condition)
{
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
		ImGui::DragFloat(id, modifyingValue, step, 0.0f, 0.0f, "%.2f");
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

	ImVec2 startPos = ImGui::GetCursorScreenPos();

	std::map<float, vec>::iterator prevC = gradient.colorList.end();
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
			currPercent = 1.0f;
			prevColor = { (*prevC).second.x, (*prevC).second.y, (*prevC).second.z, 1.0f };
			currColor = prevColor;
		}
		else
		{
			prevPercent = (*prevC).first;
			currPercent = (*currC).first;
			prevColor = { (*prevC).second.x, (*prevC).second.y, (*prevC).second.z, 1.0f };
			currColor = { (*currC).second.x, (*currC).second.y, (*currC).second.z, 1.0f };
		}
		if (prevC == gradient.colorList.begin() && prevPercent > 0.0f)
			draw_list->AddRectFilledMultiColor({ startPos.x, startPos.y }, { startPos.x + itemWidth * prevPercent, startPos.y + itemHeight }, ImGui::GetColorU32(prevColor), ImGui::GetColorU32(prevColor), ImGui::GetColorU32(prevColor), ImGui::GetColorU32(prevColor));

		draw_list->AddRectFilledMultiColor({ startPos.x + itemWidth * prevPercent, startPos.y }, { startPos.x + itemWidth * currPercent, startPos.y + itemHeight }, ImGui::GetColorU32(prevColor), ImGui::GetColorU32(currColor), ImGui::GetColorU32(currColor), ImGui::GetColorU32(prevColor));
	}

	float buttonWidth = 5.0f;
	float buttonHeight = 10.0f; //TODO: Currently not applying

	static float movingKey = -1.0f;

	ImGui::PushItemWidth(buttonWidth);
	int count = 0;
	for (std::map<float, vec>::iterator it_c = gradient.colorList.begin(); it_c != gradient.colorList.end(); ++it_c)
	{
		float percent = (*it_c).first;
		ImVec4 color = { (*it_c).second.x, (*it_c).second.y, (*it_c).second.z, 1.0 };

		ImGui::SetCursorScreenPos({ startPos.x + itemWidth * percent - buttonWidth * 0.5f, startPos.y + itemHeight - buttonHeight * 0.25f });
		if (ImGui::IsMouseDragging(0) && movingKey != -1.0f)
		{
			ImGui::ColorButton(("##gradientColor" + std::to_string(count)).c_str(), color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, { buttonWidth, 0.0f });
		}
		else
			ImGui::ColorEdit3(("##gradientColor" + std::to_string(count)).c_str(), (*it_c).second.ptr(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop);

		if (ImGui::IsItemClicked(0) && !ImGui::IsPopupOpen("picker"))
			movingKey = percent;

		count++;
	}
	ImGui::PopItemWidth();

	//Add keys
	ImGui::SetCursorScreenPos(startPos);
	if (ImGui::InvisibleButton("##addColor", { itemWidth, itemHeight }))
	{
		float percent = (ImGui::GetMousePos().x - startPos.x) / itemWidth;
		vec color = gradient.GetColor(percent).rgb;
		gradient.SetColor(color, percent);
	}

	//Move keys
	if (movingKey != -1.0)
	{
		float percent = (ImGui::GetMousePos().x - startPos.x) / itemWidth;
		if (!gradient.hasKey(percent) && ImGui::IsMouseDragging(0))
		{
			gradient.MoveColorKey(movingKey, percent);
			movingKey = percent;
		}		

		if (ImGui::IsMouseReleased(0))
			movingKey = -1.0f;
	}

	ImGui::SetCursorScreenPos(startPos); ImGui::Dummy({ 0.0f,0.0f });
}
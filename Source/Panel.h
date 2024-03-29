#ifndef __PANEL_H__
#define __PANEL_H__

#include "Globals.h"

class JSON_Value;

class Panel
{
public:

	Panel(const char* name, bool startActive = true): name(name), active(startActive)
	{
		flags = ImGuiWindowFlags_NoCollapse;
	}
	~Panel() {}

	void DrawPanel()
	{
		SetFlags();
		ImGui::Begin(name.c_str(), &active, flags);
		if (!active)
			OnClose();

		//Check for resize
		position = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
		ImVec2 wSize = ImGui::GetWindowSize();
		if (wSize.x != size.x || wSize.y != size.y)
		{
			size = { wSize.x, wSize.y };
			OnResize();
		}

		DrawContent();

		ImGui::End();
	}

	virtual void SetFlags() {}
	virtual void DrawContent() {}
	virtual void OnResize() {}

	const char* GetName() { return name.c_str(); } const
	float2 GetSize() { return size; } const
	float2 GetPosition() { return position; } const
	float4 GetRegion() { return float4(position.x, position.y, size.x, size.y); } const
	bool IsActive() { return active; } const
	void ToggleActive() { active = !active; if (active) OnOpen(); else OnClose(); }

	virtual void OnOpen() {}
	virtual void OnClose() {}

	virtual void SaveSettings(JSON_Value* settings) {}
	virtual void LoadSettings(JSON_Value* settings) {}

protected:

	std::string name;

	float2 position;
	float2 size;

	bool active = true;

	ImGuiWindowFlags flags;
};

#endif // !__PANEL_H__
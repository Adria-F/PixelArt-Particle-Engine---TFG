#ifndef __PANEL_H__
#define __PANEL_H__

#include "Globals.h"

class Panel
{
public:

	Panel(const char* name, bool startActive = true): name(name), active(startActive)
	{
		flags = ImGuiWindowFlags_None;
	}
	~Panel() {}

	void DrawPanel()
	{
		SetFlags();
		ImGui::Begin(name.c_str(), &active, flags);

		//Check for resize
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

	const char* GetName() { return name.c_str(); }
	float2 GetSize() { return size; }
	bool IsActive() { return active; }
	void ToggleActive() { active = !active; if (active) OnOpen(); else OnClose(); }

	virtual void OnOpen() {}
	virtual void OnClose() {}

protected:

	std::string name;

	//float2 position;
	float2 size;

	bool active = true;

	ImGuiWindowFlags flags;
};

#endif // !__PANEL_H__
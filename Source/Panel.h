#ifndef __PANEL_H__
#define __PANEL_H__

#include "Globals.h"

class Panel
{
public:

	Panel(const char* name): name(name)
	{
		flags = ImGuiWindowFlags_None;
	}
	~Panel() {}

	void DrawPanel()
	{
		ImGui::Begin(name.c_str(), &active, flags);

		DrawContent();

		ImGui::End();
	}

	virtual void DrawContent() {}

	const char* GetName() { return name.c_str(); }
	float2 GetSize() { return size; }
	bool IsActive() { return active; }
	void ToggleActive() { active = !active; }

protected:

	std::string name;

	float2 position;
	float2 size;

	bool active = true;

	ImGuiWindowFlags flags;
};

#endif // !__PANEL_H__
#include "pch.h"
#include "ResUI.h"

ResUI::ResUI(const string& _strName, RES_TYPE _Type)	:
	UI(_strName),
	m_ResType(_Type)
{
}

ResUI::~ResUI()
{
}

int ResUI::render_update()
{
	if (!m_TargetRes.Get())
		return FALSE;

	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.49f, 0.04f, 0.9f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.49f, 0.04f, 0.9f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.49f, 0.04f, 0.9f, 1.f));
	ImGui::Button(GetName().c_str());
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	return TRUE;
}


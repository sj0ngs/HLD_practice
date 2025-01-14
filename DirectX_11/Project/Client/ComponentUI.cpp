#include "pch.h"
#include "ComponentUI.h"

#include "InspectorUI.h"
#include "ImGuiMgr.h"


ComponentUI::ComponentUI(const string& _Name, COMPONENT_TYPE _Type)	:
	UI(_Name),
	m_pTarget(nullptr),
	m_Type(_Type)
{
}

ComponentUI::~ComponentUI()
{
}


int ComponentUI::render_update()
{
	if (!IsValid(m_pTarget) || nullptr == m_pTarget->GetComponent(m_Type))
		return FALSE;

	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
	ImGui::Button(GetName().c_str());
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	if (COMPONENT_TYPE::TRANSFORM == m_Type || COMPONENT_TYPE::MESHRENDER <= m_Type)
		return TRUE;

	string DelBut = "Delete##" + GetName();
	ImGui::SameLine(0.f, 100.f);
	if (ImGui::Button(DelBut.c_str()))
	{
		GetTarget()->DeleteComponent(m_Type);
		InspectorUI* inspector = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");
		inspector->SetTargetObject(GetTarget());
		return false;
	}

	return TRUE;
}

void ComponentUI::SetTargetObject(CGameObject* _pTarget)
{
	m_pTarget = _pTarget;

	if (nullptr == _pTarget)
	{
		SetActive(false);
		return;
	}

	if (nullptr == m_pTarget->GetComponent(m_Type))
	{
		SetActive(false);
	}
	else
	{
		SetActive(true);
	}
}

void ComponentUI::GetResKey(Ptr<CRes> _Res, char* _Buff, size_t _BuffSize)
{
	memset(_Buff, 0, sizeof(char) * _BuffSize);

	if (nullptr == _Res)
		return;

	wstring wstrKey = _Res->GetKey();
	string strKey = string(wstrKey.begin(), wstrKey.end());
	memcpy(_Buff, strKey.data(), sizeof(char) * strKey.length());
}
#include "pch.h"
#include "ScriptUI.h"

#include <Script\CScriptMgr.h>

#include "ParamUI.h"

ScriptUI::ScriptUI()    :
    ComponentUI("##Script", COMPONENT_TYPE::SCRIPT),
	m_pTargetScript(nullptr)
{
	SetSize(0.f, 150.f);
}

ScriptUI::~ScriptUI()
{
}

int ScriptUI::render_update()
{
	if (nullptr == GetTarget())
		return FALSE;

	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.925f, 0.735f, 0.223f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.925f, 0.735f, 0.223f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.925f, 0.735f, 0.223f, 1.f));
	ImGui::Button(m_strScriptName.c_str());
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::SameLine();
	if (ImGui::Button("Delete##Scripts"))
	{
		GetTarget()->DeleteScript(m_pTargetScript->GetScriptType());
		SetActive(false);
		return FALSE;
	}

	const vector<tScriptParam>& vecScriptParam = m_pTargetScript->GetScriptParam();
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.f, 1.f), "Parameters");

	for (size_t i = 0; i < vecScriptParam.size(); i++)
	{
		switch (vecScriptParam[i].eParam)
		{
		case SCRIPT_PARAM::INT:
			ParamUI::Param_Int(vecScriptParam[i].strDesc, (int*)vecScriptParam[i].pData, true);
			break;
		case SCRIPT_PARAM::FLOAT:
			ParamUI::Param_Float(vecScriptParam[i].strDesc, (float*)vecScriptParam[i].pData, true);
			break;
		case SCRIPT_PARAM::VEC2:
			ParamUI::Param_Vec2(vecScriptParam[i].strDesc, (Vec2*)vecScriptParam[i].pData, true);
			break;
		case SCRIPT_PARAM::VEC4:
			ParamUI::Param_Vec4(vecScriptParam[i].strDesc, (Vec4*)vecScriptParam[i].pData, true);
			break;
		case SCRIPT_PARAM::TEX:
			Ptr<CTexture> pTex = (CTexture*)vecScriptParam[i].pData;
			if (ParamUI::Param_Tex(vecScriptParam[i].strDesc, pTex, this, (UI_DELEGATE_1)&ScriptUI::SelectTexParam))
			{
				m_pSelectedDesc = vecScriptParam[i].strDesc;
			}
			//sm_pTargetScript->SetScriptTex(vecScriptParam[i].strDesc, pTex);

			break;
		}
	}

	return TRUE;
}

void ScriptUI::SetScript(CScript* _pScript)
{
	assert(_pScript);

	m_pTargetScript = _pScript;
	wstring strName = CScriptMgr::GetScriptName(m_pTargetScript);
	m_strScriptName = string(strName.begin(), strName.end());
}

void ScriptUI::SelectTexParam(DWORD_PTR _strKey)
{
	string strKey = (char*)_strKey;
	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstring(strKey.begin(), strKey.end()));
	m_pTargetScript->SetScriptTex(m_pSelectedDesc, pTex);
	m_pSelectedDesc.clear();
}
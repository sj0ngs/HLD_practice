#include "pch.h"
#include "PrefabUI.h"

#include <Engine\CPrefab.h>
#include <Engine\CGameObject.h>
#include <Engine\CRenderMgr.h>

#include "ImGuiMgr.h"
#include "OutlinerUI.h"

#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "Collider2DUI.h"
#include "Animator2DUI.h"
#include "Light2DUI.h"
#include "CamaraUI.h"
#include "TileMapUI.h"
#include "ParticleSystemUI.h"
#include "ScriptUI.h"

PrefabUI::PrefabUI()	:
	ResUI("##Prefab", RES_TYPE::PREFAB),
	m_iLayerSelected(0)
{
	SetName("Prefab");

	for (int i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i] = to_string(i);
	}

	// ============
	// Component UI
	// ============
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM] = new TransformUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM]->SetSize(0.f, 120.f);

	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER] = new MeshRenderUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]->SetSize(0.f, 100.f);

	m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D] = new Collider2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D]->SetSize(0.f, 120.f);

	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D] = new Animator2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D]->SetSize(0.f, 180.f);

	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D] = new Light2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D]->SetSize(0.f, 150.f);

	m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA] = new CamaraUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA]->SetSize(0.f, 100.f);

	m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP] = new TileMapUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP]->SetSize(0.f, 100.f);

	m_arrComUI[(UINT)COMPONENT_TYPE::PARTICLESYSTEM] = new ParticleSystemUI;

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (nullptr != m_arrComUI[i])
		{
			AddChildUI(m_arrComUI[i]);
		}
	}
}

PrefabUI::~PrefabUI()
{
}

void PrefabUI::ClearTargetObject()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (nullptr != m_arrComUI[i])
		{
			m_arrComUI[i]->SetTargetObject(nullptr);
		}
	}
}

void PrefabUI::ClearScriptUI()
{
	for (size_t i = 0; i < m_vecScriptUI.size(); i++)
	{
		m_vecScriptUI[i]->SetActive(false);
	}
}

int PrefabUI::render_update()
{
	ResUI::render_update();

	Ptr<CPrefab> pPref = (CPrefab*)GetTargetRes().Get();

	if (ImGui::Button("Instantiate Prefab##"))
	{
		CGameObject* pClone = pPref->Instantiate();
		SpawnGameObject(pClone, CRenderMgr::GetInst()->GetEditorCamPos(), m_iLayerSelected);

		OutlinerUI* pOutliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");
		pOutliner->SetReserveData((DWORD_PTR)pClone);
	}

	ImGui::SetNextItemWidth(50.f);
	if (ImGui::BeginCombo("##InstLayer", m_arrLayer[m_iLayerSelected].c_str()))
	{
		for (int i = 0; i < MAX_LAYER; i++)
		{
			const bool is_selected = (m_iLayerSelected == i);
			if (ImGui::Selectable(m_arrLayer[i].c_str(), is_selected))
			{
				m_iLayerSelected = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	return 0;
}

void PrefabUI::SetTargetRes(Ptr<CRes> _Res)
{
	ResUI::SetTargetRes(_Res);
	ClearTargetObject();

	Ptr<CPrefab> pPrefab = (CPrefab*)GetTargetRes().Get();

	if (nullptr == pPrefab)
	{
		ClearScriptUI();
		return;
	}

	CGameObject* pGO = pPrefab->GetProtoObj();

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (nullptr != m_arrComUI[i])
		{
			m_arrComUI[i]->SetTargetObject(pGO);
		}
	}

	const vector<CScript*>& vecScripts = pGO->GetScripts();

	if (m_vecScriptUI.size() < vecScripts.size())
	{
		size_t sDiffer = vecScripts.size() - m_vecScriptUI.size();
		for (size_t i = 0; i < sDiffer; i++)
		{
			ScriptUI* UI = new ScriptUI;

			m_vecScriptUI.push_back(UI);
			AddChildUI(UI);
			UI->SetActive(true);
		}
	}

	for (size_t i = 0; i < m_vecScriptUI.size(); i++)
	{
		if (vecScripts.size() <= i)
		{
			m_vecScriptUI[i]->SetActive(false);
			continue;
		}

		m_vecScriptUI[i]->SetTargetObject(pGO);
		m_vecScriptUI[i]->SetScript(vecScripts[i]);
		m_vecScriptUI[i]->SetActive(true);
	}
}

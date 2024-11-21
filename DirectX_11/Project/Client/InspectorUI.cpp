#include "pch.h"
#include "InspectorUI.h"

#include <Engine\CLevelMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\CEventMgr.h>

#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "Collider2DUI.h"
#include "Animator2DUI.h"
#include "Light2DUI.h"
#include "CamaraUI.h"
#include "TileMapUI.h"
#include "ParticleSystemUI.h"

#include "ResUI.h"
#include "MeshUI.h"
#include "MaterialUI.h"
#include "PrefabUI.h"
#include "TextureUI.h"
#include "SoundUI.h"
#include "GraphicsShaderUI.h"
#include "ComputeShaderUI.h"
#include "MeshDataUI.h"
#include "ScriptUI.h"

InspectorUI::InspectorUI()	:
	UI("##Inspector"),
	m_pTargetObject(nullptr),
	m_arrComUI{},
	m_arrResUI{}
{
	SetName("Inspector");

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

	// ===========
	// Resource UI
	// ===========
	m_arrResUI[(UINT)RES_TYPE::MESHDATA] = new MeshDataUI;
	m_arrResUI[(UINT)RES_TYPE::MATERIAL] = new MaterialUI;
	m_arrResUI[(UINT)RES_TYPE::PREFAB] = new PrefabUI;
	m_arrResUI[(UINT)RES_TYPE::MESH] = new MeshUI;
	m_arrResUI[(UINT)RES_TYPE::TEXTURE] = new TextureUI;
	m_arrResUI[(UINT)RES_TYPE::SOUND] = new SoundUI;
	m_arrResUI[(UINT)RES_TYPE::GRAPHICS_SHADER] = new GraphicsShaderUI;
	m_arrResUI[(UINT)RES_TYPE::COMPUTE_SHADER] = new ComputeShaderUI;

	for (UINT i = 0; i < (UINT)RES_TYPE::END; i++)
	{
		if (nullptr != m_arrResUI[i])
		{
			AddChildUI(m_arrResUI[i]);
		}
	}


	for (int i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i] = to_string(i);
	}
}

InspectorUI::~InspectorUI()
{
}

void InspectorUI::init()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (nullptr != m_arrComUI[i])
		{
			m_arrComUI[i]->SetTargetObject(m_pTargetObject);
		}
	}
}

void InspectorUI::tick()
{
}

int InspectorUI::render_update()
{
	if (!IsValid(m_pTargetObject))
	{
		m_pTargetObject = nullptr;
		return FALSE;
	}

	ImGui::Text("Layer : ");
	ImGui::SameLine();

	int iLayer = m_pTargetObject->GetLayerIndex();

	if (ImGui::BeginCombo("##Layer", m_arrLayer[iLayer].c_str()))
	{
		for (int i = 0; i < MAX_LAYER; i++)
		{
			const bool is_selected = (iLayer == i);
			if (ImGui::Selectable(m_arrLayer[i].c_str(), is_selected))
			{
				iLayer = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();

		tEvent evn;
		evn.wParam = (DWORD_PTR)m_pTargetObject;
		evn.lParam = (DWORD_PTR)iLayer;
		evn.Type = EVENT_TYPE::LAYER_CHANGE;
		CEventMgr::GetInst()->AddEvent(evn);
	}

	return TRUE;
}

void InspectorUI::SetTargetObject(CGameObject* _pTarget)
{
	ClearTargetRes();

	m_pTargetObject = _pTarget;

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (nullptr != m_arrComUI[i])
		{
			m_arrComUI[i]->SetTargetObject(m_pTargetObject);
		}
	}

	if (nullptr == m_pTargetObject)
	{
		for (size_t i = 0; i < m_vecScriptUI.size(); i++)
		{
			m_vecScriptUI[i]->SetActive(false);
		}
		return;
	}

	const vector<CScript*>& vecScripts = m_pTargetObject->GetScripts();

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

		m_vecScriptUI[i]->SetTargetObject(m_pTargetObject);
		m_vecScriptUI[i]->SetScript(vecScripts[i]);
		m_vecScriptUI[i]->SetActive(true);
	}
}

void InspectorUI::SetTargetRes(Ptr<CRes> _Res)
{
	ClearTargetObject();

	for (UINT i = 0; i < (UINT)RES_TYPE::END; i++)
	{
		m_arrResUI[i]->SetActive(false);
	}

	m_pTargetRes = _Res;

	if (nullptr == m_pTargetRes)
		return;

	RES_TYPE type = m_pTargetRes->GetType();
	m_arrResUI[(UINT)type]->SetActive(true);
	m_arrResUI[(UINT)type]->SetTargetRes(m_pTargetRes);

	ClearScriptUI();
}

void InspectorUI::ClearTargetObject()
{
	m_pTargetObject = nullptr;

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (nullptr != m_arrComUI[i])
		{
			m_arrComUI[i]->SetTargetObject(nullptr);
		}
	}
}

void InspectorUI::ClearTargetRes()
{
	for (UINT i = 0; i < (UINT)RES_TYPE::END; i++)
	{
		if (nullptr != m_arrResUI[i])
		{
			m_arrResUI[i]->SetTargetRes(nullptr);
			m_arrResUI[i]->SetActive(false);
		}
	}
}

void InspectorUI::ClearScriptUI()
{
	for (size_t i = 0; i < m_vecScriptUI.size(); i++)
	{
		m_vecScriptUI[i]->SetActive(false);
	}
}

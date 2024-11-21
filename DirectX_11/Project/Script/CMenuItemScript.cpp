#include "pch.h"
#include "CMenuItemScript.h"
#include "CMenuScript.h"

#include "../Client/CLevelSaveLoad.h"
#include "CMenuScript.h"
#include <Engine\CEventMgr.h>
#include <Engine\CRenderMgr.h>

#include "CCameraMoveScript.h"

CMenuItemScript::CMenuItemScript() :
	CScript(SCRIPT_TYPE::MENUITEMSCRIPT),
	m_bPrevEnter(false),
	m_pMenu(nullptr),
	m_bLevelChange(false),
	m_fAccTime(0.f),
	m_fTime(0.f)
{
}

CMenuItemScript::~CMenuItemScript()
{
}

void CMenuItemScript::begin()
{
	m_pMenu = CLevelMgr::GetInst()->FindObjectByName(L"Menu");
	dynamic_cast<CMenuScript*>(m_pMenu->GetScript(SCRIPT_TYPE::MENUSCRIPT))->AddDynamicStartButton(this, (SCRIPT_DELEGATE)&CMenuItemScript::ButtonVisible);
	Transform()->SetRelativeScale(0.f, 0.f, 0.f);
	//Vec4 vColor = Vec4(0.f, 0.f, 0.f, 1.f);
	//CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->BeginFade(m_fTime, vColor, true);
}

void CMenuItemScript::tick()
{
	if (IsValid(m_pMenu) && m_pMenu->GetScript<CMenuScript>()->GetMouseClicked())
	{
		Vec3 vMousePos = CKeyMgr::GetInst()->GetMouseWorldPos();
		Vec3 vPos = Transform()->GetRelativePos();
		Vec3 vScale = Transform()->GetRelativeScale();

		if ((vMousePos.x >= vPos.x - vScale.x / 2.f) && (vMousePos.x <= vPos.x + vScale.x / 2.f) &&
			(vMousePos.y >= vPos.y - vScale.y / 2.f) && (vMousePos.y <= vPos.y + vScale.y / 2.f))
		{
			if(!m_bPrevEnter)
				SOUND(L"snd_MenuScroll")->Play(1, 10.f, true);

			m_vChangedColor = Vec4(0.992f, 0.234f, 0.6f, 1.f);
			m_bPrevEnter = true;

			if (KEY_TAP(KEY::LBTN))
			{
				m_bLevelChange = true;
				m_fTime = 1.f;
				
				Vec4 vColor = Vec4(0.f, 0.f, 0.f, 1.f);
				CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->BeginFade(m_fTime, vColor, false);
			}
		}
		else
		{
			m_vChangedColor = Vec4(0.f, 0.f, 0.f, 0.f);
			m_bPrevEnter = false;
		}
	}

	if (m_bLevelChange)
	{
		if (m_fAccTime >= m_fTime)
		{
			CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(L"Level\\room_moon.level");
			tEvent evn = {};
			evn.Type = EVENT_TYPE::LEVEL_CHANGE;
			evn.wParam = (DWORD_PTR)pLoadedLevel;

			CEventMgr::GetInst()->AddEvent(evn);
		}
		else
		{
			m_fAccTime += DT;
		}
	}

	int i0 = 1;
	MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &i0);
	Vec4 vDefault = Vec4(0.f, 0.f, 0.f, 0.f);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_0, &vDefault);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &m_vChangedColor);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &m_vChangedColor);
}

void CMenuItemScript::ButtonVisible()
{
	Transform()->SetRelativeScale(440.f, 36.f, 1.f);
}

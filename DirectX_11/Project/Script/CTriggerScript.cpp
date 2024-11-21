#include "pch.h"
#include "CTriggerScript.h"

#include <Engine\CLevel.h>
#include <Engine\CRenderMgr.h>
#include "../Client/CLevelSaveLoad.h"
#include "CCameraMoveScript.h"

CTriggerScript::CTriggerScript()	:
	CScript(SCRIPT_TYPE::TRIGGERSCRIPT),
	m_bLevelChange(false),
	m_fAccTime(0.f),
	m_fTime(0.5f)
{
}

CTriggerScript::~CTriggerScript()
{
}

void CTriggerScript::begin()
{
	Vec4 vCol = Vec4(0.f, 0.f, 0.f, 1.f);
	CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->BeginFade(m_fTime, vCol, true);

	CResMgr::GetInst()->FindRes<CSound>(L"AMB_North2")->Play(0, 1.f, true);
}

void CTriggerScript::end()
{
	CResMgr::GetInst()->FindRes<CSound>(L"AMB_North2")->Stop();
}

void CTriggerScript::tick()
{
	if (m_bLevelChange)
	{
		m_fAccTime += DT;
		if (m_fAccTime >= m_fTime)
		{
			CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(L"Level\\room_boss.level");
			tEvent evn = {};
			evn.Type = EVENT_TYPE::LEVEL_CHANGE;
			evn.wParam = (DWORD_PTR)pLoadedLevel;

			CEventMgr::GetInst()->AddEvent(evn);
		}
	}
}

void CTriggerScript::BeginOverlap(CCollider2D* _Other)
{
	if (false == m_bLevelChange)
	{
		m_bLevelChange = true;
		Vec4 vCol = Vec4(0.f, 0.f, 0.f, 1.f);
		CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->BeginFade(m_fTime, vCol, false);
	}
}

void CTriggerScript::OnOverlap(CCollider2D* _Other)
{

}

void CTriggerScript::EndOverlap(CCollider2D* _Other)
{
}


#include "pch.h"
#include "CMenuScript.h"

CMenuScript::CMenuScript()	:
	CScript(SCRIPT_TYPE::MENUSCRIPT),
	m_bMouseClicked(false),
	m_bBooked(false),
	m_pStartButtonInst(nullptr),
	m_pButtonVisibleFunc(nullptr)
{
}

CMenuScript::~CMenuScript()
{
}

void CMenuScript::begin()
{
}

void CMenuScript::tick()
{
	if (m_bBooked)
	{
		m_bMouseClicked = true;
	}

	if (!m_bMouseClicked && KEY_TAP(KEY::LBTN))
	{
		m_bBooked = true;
		Transform()->SetRelativeScale(Vec3(244.f, 200.f, 1.f) * 2.f);
		CResMgr::GetInst()->FindRes<CSound>(L"snd_MenuAction")->Play(1, 10.f, true);
		if (m_pStartButtonInst)
			((m_pStartButtonInst)->*(m_pButtonVisibleFunc))();
	}
}


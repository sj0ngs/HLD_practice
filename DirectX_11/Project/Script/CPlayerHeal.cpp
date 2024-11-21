#include "pch.h"
#include "CPlayerHeal.h"

#include "CPlayerScript.h"
#include <Engine\CAnim2D.h>

CPlayerHeal::CPlayerHeal() :
	m_pCurAnim(nullptr),
	m_bHealed(false)
{
}

CPlayerHeal::~CPlayerHeal()
{
}

void CPlayerHeal::tick()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());

	if (!m_bHealed && m_pCurAnim->GetCurFrameNotify() == L"Heal")
	{	
		m_bHealed = true;
		pPlayer->Heal();
		CResMgr::GetInst()->FindRes<CSound>(L"snd_HealthUSE")->Play(1, 10.f, true);
	}

	if (m_pCurAnim->IsFinish())
	{
		ChangeState(L"Idle");
	}
}

void CPlayerHeal::Enter()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());

	GetOwnerAI()->Transform()->SetRelativeScale(Vec3(32.f, 32.f, 1.f) * 2.f);
	m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharHeal");
	m_pCurAnim->Reset();
	pPlayer->Animator2D()->Play(L"spr_CharHeal", false);
}

void CPlayerHeal::Exit()
{
	m_bHealed = false;
}

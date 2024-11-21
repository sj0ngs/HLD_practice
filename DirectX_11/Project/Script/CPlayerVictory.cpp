#include "pch.h"
#include "CPlayerVictory.h"

#include <Engine\CAnim2D.h>

#include "CPlayerScript.h"

CPlayerVictory::CPlayerVictory() :
	m_pCurAnim(nullptr)
{
}

CPlayerVictory::~CPlayerVictory()
{
}

void CPlayerVictory::tick()
{
	if (m_pCurAnim->IsFinish())
		ChangeState(L"Idle");
}

void CPlayerVictory::Enter()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());

	GetOwnerAI()->Transform()->SetRelativeScale(Vec3(64.f, 53.f, 1.f) * 2.f);
	pPlayer->Animator2D()->Play(L"spr_CharVictory", false);
	m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharVictory");
}

void CPlayerVictory::Exit()
{
}

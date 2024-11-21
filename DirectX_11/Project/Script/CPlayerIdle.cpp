#include "pch.h"
#include "CPlayerIdle.h"

#include "CPlayerScript.h"

CPlayerIdle::CPlayerIdle()	:
	m_eFD(EFACE_DIR::NORTH)
{
}

CPlayerIdle::~CPlayerIdle()
{
}

void CPlayerIdle::tick()
{
	// w, s, a, d 입력시 move state 로 변경
	if (KEY_PRESSED(KEY::W) || KEY_PRESSED(KEY::S) || KEY_PRESSED(KEY::A) || KEY_PRESSED(KEY::D))
	{
		ChangeState(L"Move");
	}
	else if (KEY_TAP(KEY::SPACE))
	{
		ChangeState(L"Dash");
	}
	else if (KEY_TAP(KEY::LBTN))
	{
		ChangeState(L"Attack");
	}
	else if (KEY_TAP(KEY::RBTN))
	{
		ChangeState(L"Shoot");
	}
	else if (KEY_TAP(KEY::Q))
	{
		CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
		if (pPlayer->GetHP() < MAX_HP && pPlayer->GetHealthKit() > 0)
		{
			ChangeState(L"Heal");
		}
	}
}

void CPlayerIdle::Enter()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	GetOwnerAI()->Transform()->SetRelativeScale(Vec3(16.f, 32.f, 1.f) * 2.f);
	pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));

	switch (m_eFD)
	{
	case EFACE_DIR::EAST:
		pPlayer->Animator2D()->Play(L"spr_charstandside", true);
		break;
	case EFACE_DIR::WEST:
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		pPlayer->Animator2D()->Play(L"spr_charstandside", true);
		break;
	case EFACE_DIR::SOUTH:
		pPlayer->Animator2D()->Play(L"spr_charstandfront", true);
		break;
	case EFACE_DIR::NORTH:
		pPlayer->Animator2D()->Play(L"spr_charstandback", true);
		break;
	}
}

void CPlayerIdle::Exit()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
}

void CPlayerIdle::RotateFace()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	Vec3 vFaceDir = pPlayer->GetFaceDir();
	float fCos = vFaceDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// 오른쪽 보기
	if (45.f >= fAngle)
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		pPlayer->Animator2D()->Play(L"spr_charstandside", true);
	}
	else if (135.f >= fAngle)
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		if (0 <= vFaceDir.y)
		{
			pPlayer->Animator2D()->Play(L"spr_charstandback", true);
		}
		else
		{
			pPlayer->Animator2D()->Play(L"spr_charstandfront", true);
		}
	}
	// 왼쪽 보기
	else
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		pPlayer->Animator2D()->Play(L"spr_charstandside", true);
	}
}

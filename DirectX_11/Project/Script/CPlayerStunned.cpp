#include "pch.h"
#include "CPlayerStunned.h"

#include <Engine\CAnim2D.h>
#include "CPlayerScript.h"
#include <Engine\CRenderMgr.h>
#include "CCameraMoveScript.h"

CPlayerStunned::CPlayerStunned()	:
	m_pCurAnim(nullptr)
{
}

CPlayerStunned::~CPlayerStunned()
{
}

void CPlayerStunned::tick()
{
	if (m_pCurAnim->IsFinish())
	{
		ChangeState(L"Idle");
	}
}

void CPlayerStunned::Enter()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());

	pPlayer->SetInvin(true);

	Vec3 vFaceDir = pPlayer->GetFaceDir();
	float fCos = vFaceDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	GetOwnerAI()->Transform()->SetRelativeScale(Vec3(58.f, 52.f, 1.f) * 2.f);
	m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_charStunned");
	pPlayer->Animator2D()->Play(L"spr_charStunned", false);
	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	}
	// 왼쪽 보기
	else
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
	}

	int iRand = GetRandInt(2);
	switch (iRand)
	{
	case 0:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_DrifterHurt1Dmg")->Play(1, 10.f, true);
		break;
	case 1:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_DrifterHurt2Dmg")->Play(1, 10.f, true);
		break;
	case 2:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_DrifterHurt3Dmg")->Play(1, 10.f, true);
		break;
	}
}

void CPlayerStunned::Exit()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	pPlayer->SetInvin(false);
}

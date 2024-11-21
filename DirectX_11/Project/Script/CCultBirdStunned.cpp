#include "pch.h"
#include "CCultBirdStunned.h"
#include "CCultBirdScript.h"
#include <Engine\CAnim2D.h>

CCultBirdStunned::CCultBirdStunned()	:
	m_pCurAnim(nullptr)
{
}

CCultBirdStunned::~CCultBirdStunned()
{
}

void CCultBirdStunned::tick()
{
	CCultBirdScript* pMon = ((CCultBirdScript*)GetOwnerAI());
	if (m_pCurAnim->IsFinish())
	{
		if (pMon->GetHP() > 0)
		{
			ChangeState(L"Move");
		}
	}
}

void CCultBirdStunned::Enter()
{
	CCultBirdScript* pMon = ((CCultBirdScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	if (pMon->GetHP() > 0.f)
	{
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_CultBirdStunned");
		pMon->Animator2D()->Play(L"spr_CultBirdStunned", false);
	}
	else
	{
		pMon->SetDead(true);
		pMon->Collider2D()->OffCollision();
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_CultBirdDead");
		pMon->Animator2D()->Play(L"spr_CultBirdDead", false);

		int iRand = GetRandInt(2);
		switch (iRand)
		{
		case 0:
			CResMgr::GetInst()->FindRes<CSound>(L"snd_EnemyDeath1_v2")->Play(1, 10.f, true);
			break;
		case 1:
			CResMgr::GetInst()->FindRes<CSound>(L"snd_EnemyDeath2_v2")->Play(1, 10.f, true);
			break;
		case 2:
			CResMgr::GetInst()->FindRes<CSound>(L"snd_EnemyDeath3_v2")->Play(1, 10.f, true);
			break;
		}
	}
	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	}
	// 왼쪽 보기
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
	}
}

void CCultBirdStunned::Exit()
{
}

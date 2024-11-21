#include "pch.h"
#include "CCultBirdDodge.h"

#include "CCultBirdScript.h"
#include <Engine\CAnim2D.h>

CCultBirdDodge::CCultBirdDodge()	:
	m_fDodgeSpeed(500.f),
	m_pCurAnim(nullptr)
{
}

CCultBirdDodge::~CCultBirdDodge()
{
}

void CCultBirdDodge::tick()
{
	if (m_pCurAnim->IsFinish())
	{
		ChangeState(L"Move");
		return;
	}
	else
	{
		CCultBirdScript* pMon = ((CCultBirdScript*)GetOwnerAI());
		Vec3 vMonPos = pMon->Transform()->GetRelativePos();

		vMonPos += pMon->GetFaceDir() * m_fDodgeSpeed * DT;
		pMon->Transform()->SetRelativePos(vMonPos);
	}
}

void CCultBirdDodge::Enter()
{	
	CCultBirdScript* pMon = ((CCultBirdScript*)GetOwnerAI());
	Vec3 vMonPos = pMon->Transform()->GetRelativePos();
	vMonPos.z = 0.f;
	Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
	vPlayerPos.z = 0.f;

	Vec3 vDir =  vMonPos - vPlayerPos;
	vDir.Normalize();

	pMon->SetFaceDir(vDir);

	vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_CultBirdDodge");
	pMon->Animator2D()->Play(L"spr_CultBirdDodge", false);
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

	pMon->SetDodgeCool(1.5f);
}

void CCultBirdDodge::Exit()
{
}

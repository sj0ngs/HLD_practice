#include "pch.h"
#include "CGhostBeamBirdWalk.h"

#include "CGhostBeamBirdScript.h"
#include <Engine\CAnim2D.h>

CGhostBeamBirdWalk::CGhostBeamBirdWalk()	:
	m_fWalkSpeed(50.f),
	m_fWalkTime(1.f),
	m_fAccWalkTime(0.f)
{
}

CGhostBeamBirdWalk::~CGhostBeamBirdWalk()
{
}

void CGhostBeamBirdWalk::tick()
{
	CGhostBeamBirdScript* pMon = ((CGhostBeamBirdScript*)GetOwnerAI());

	if (m_fAccWalkTime >= m_fWalkTime)
	{
		ChangeState(L"Idle");
		return;
	}
	else
	{
		m_fAccWalkTime += DT;
		Vec3 vMonPos = pMon->Transform()->GetRelativePos();
		vMonPos += pMon->GetFaceDir() * m_fWalkSpeed * DT;
		pMon->Transform()->SetRelativePos(vMonPos);
	}
}

void CGhostBeamBirdWalk::Enter()
{
	CGhostBeamBirdScript* pMon = ((CGhostBeamBirdScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		pMon->Animator2D()->Play(L"spr_GhostBeamBirdWalk", true);
	}
	// 왼쪽 보기
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		pMon->Animator2D()->Play(L"spr_GhostBeamBirdWalk", true);
	}
}

void CGhostBeamBirdWalk::Exit()
{
	m_fAccWalkTime = 0.f;
}

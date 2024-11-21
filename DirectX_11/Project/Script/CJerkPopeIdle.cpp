#include "pch.h"
#include "CJerkPopeIdle.h"
#include "CJerkPopeScript.h"

CJerkPopeIdle::CJerkPopeIdle()	:
	m_fDetectRadius(200.f),
	m_bDetected(false)
{
}

CJerkPopeIdle::~CJerkPopeIdle()
{
}

void CJerkPopeIdle::tick()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());

	if (!m_bDetected)
	{
		//Vec3 vMonPos = pMon->Transform()->GetRelativePos();
		//vMonPos.z = 0.f;
		//Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
		//vPlayerPos.z = 0.f;

		//float fDist = Vec3::Distance(vMonPos, vPlayerPos);

		//if (fDist <= m_fDetectRadius)
		//{
		//	m_bDetected = true;
		//}

		if (true == pMon->IsInvoke())
		{
			m_bDetected = true;
			pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
			pMon->Animator2D()->FindAnim(L"spr_JerkPopeRaise")->Reset();
			pMon->Animator2D()->Play(L"spr_JerkPopeRaise", false);
		}
	}
	else
	{
		Vec3 vDir = pMon->GetFaceDir();

		float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
		float fAngle = acosf(fCos);
		fAngle *= 180 / XM_PI;

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

		if (pMon->IsAttackAvail())
		{
			if (pMon->IsPhase2())
			{
				pMon->SetMoveCenter(false);
				ChangeState(L"Teleport");
				return;
			}

			if (2 == pMon->GetTileAttack() && 2 == pMon->GetTraceAttack())
			{
				ChangeState(L"Spawn");
				return;
			}

			ChangeState(L"Attack");
		}
	}
}

void CJerkPopeIdle::Enter()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		pMon->Animator2D()->Play(L"spr_JerkPopeIdle", true);
	}
	// 왼쪽 보기
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		pMon->Animator2D()->Play(L"spr_JerkPopeIdle", true);
	}
}

void CJerkPopeIdle::Exit()
{
}

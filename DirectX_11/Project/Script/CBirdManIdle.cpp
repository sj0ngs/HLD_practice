#include "pch.h"
#include "CBirdManIdle.h"

#include <Engine\CAnim2D.h>
#include "CBirdManScript.h"

CBirdManIdle::CBirdManIdle() :
	m_fWalkSpeed(50.f),
	m_fDetectRadius(150.f),
	m_fDetectAngle(45.f),
	m_pCurAnim(nullptr),
	m_fWalkTime(0.f),
	m_fAccWalkTime(0.f),
	m_fStandTime(0.f),
	m_fAccStandTime(0.f),
	m_bWalk(false)
{
}

CBirdManIdle::~CBirdManIdle()
{
}

void CBirdManIdle::tick()
{
	CBirdManScript* pMon = ((CBirdManScript*)GetOwnerAI());

	// 랜덤 초 동안 기다리고
	if (!m_bWalk)
	{
		if (m_fAccStandTime >= m_fStandTime)
		{
			SetWalkAnim();
			m_fWalkTime = (float)GetRandInt(2, 1);
			m_bWalk = true;
			m_fAccStandTime = 0.f;
		}
		else
		{
			m_fAccStandTime += DT;
		}
	}
	// 랜덤 초 동안 이동
	else
	{
		if (m_fAccWalkTime >= m_fWalkTime)
		{
			SetStandAnim();
			m_fStandTime = (float)GetRandInt(5, 3);
			m_bWalk = false;
			m_fAccWalkTime = 0.f;
		}
		else
		{
			m_fAccWalkTime += DT;
			Vec3 vMonPos = pMon->Transform()->GetRelativePos();
			vMonPos += pMon->GetFaceDir() * m_fWalkSpeed * DT;
			pMon->Transform()->SetRelativePos(vMonPos);
		}
	}

	// 플레이어가 감지 영역안에 들어오면 스테이트 변경
	Vec3 vMonPos = pMon->Transform()->GetRelativePos();
	vMonPos.z = 0.f;
	Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
	vPlayerPos.z = 0.f;

	float fDist = Vec3::Distance(vMonPos, vPlayerPos);

	if (fDist <= m_fDetectRadius)
	{
		//float fCos = pMon->GetFaceDir().Dot(vPlayerPos - vMonPos);
		//float fAngle = acosf(fCos);

		//if (fAngle <= m_fDetectAngle)
		//{
		//	ChangeState(L"Fly");
		//}

		ChangeState(L"Fly");
	}
}

void CBirdManIdle::Enter()
{	
	GetOwnerAI()->Transform()->SetRelativeScale(Vec3(48.f, 48.f, 1.f) * 2.f);
	SetRandomFaceDir();
	SetStandAnim();
	m_fStandTime = (float)GetRandInt(5, 3);
}

void CBirdManIdle::Exit()
{
	m_pCurAnim = nullptr;

	m_fWalkTime = 0.f;
	m_fAccWalkTime = 0.f;
	m_fStandTime = 0.f;
	m_fAccStandTime = 0.f;

	m_bWalk = false;
}

void CBirdManIdle::SetRandomFaceDir()
{
	CBirdManScript* pMon = ((CBirdManScript*)GetOwnerAI());

	int iRand = GetRandInt(360);
	float fAngle = XMConvertToRadians((float)iRand);

	Matrix matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(fAngle);
	Vec4 vDir = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), matRot);

	pMon->SetFaceDir(vDir);
}

void CBirdManIdle::SetStandAnim()
{
	CBirdManScript* pMon = ((CBirdManScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_BirdmanStand");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_BirdmanStand", false);
	}
	// 왼쪽 보기
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_BirdmanStand");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_BirdmanStand", false);
	}
}

void CBirdManIdle::SetWalkAnim()
{
	SetRandomFaceDir();

	CBirdManScript* pMon = ((CBirdManScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_BirdmanWalk");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_BirdmanWalk", true);
	}
	// 왼쪽 보기
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_BirdmanWalk");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_BirdmanWalk", true);
	}
}

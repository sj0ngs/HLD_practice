#include "pch.h"
#include "CBirdManFly.h"

#include "CBirdManScript.h"
#include <Engine\CAnim2D.h>
#include "CShadowScript.h"

CBirdManFly::CBirdManFly()	:
	m_fFlySpeed(50.f),
	m_pCurAnim(nullptr),
	m_fFlyTime(2.f),
	m_fAccFlyTime(0.f),
	m_fHeight(100.f)
{
}

CBirdManFly::~CBirdManFly()
{
}

void CBirdManFly::tick()
{
	CBirdManScript* pMon = ((CBirdManScript*)GetOwnerAI());
	Vec3 vMonPos = pMon->Transform()->GetRelativePos();

	// 특정 고도까지 상승
	if ((vMonPos.y - m_vEnterPos.y) <= m_fHeight)
	{	
		vMonPos.y += m_fFlySpeed * DT;
	}

	if (m_fAccFlyTime >= m_fFlyTime)
	{
		ChangeState(L"Dive");
	}
	else
	{
		m_fAccFlyTime += DT;
		WatchPlayer();
		SetFlyAnim();
		vMonPos += pMon->GetFaceDir() * m_fFlySpeed / 3.f * DT;
	}

	pMon->Transform()->SetRelativePos(vMonPos);
}

void CBirdManFly::Enter()
{
	CBirdManScript* pMon = ((CBirdManScript*)GetOwnerAI());
	m_fFlyTime = (float)GetRandInt(3, 2);
	m_vEnterPos = pMon->Transform()->GetRelativePos();

	CShadowScript* pShadow = dynamic_cast<CShadowScript*>(pMon->GetOwner()->GetScript(SCRIPT_TYPE::SHADOWSCRIPT));
	if (pShadow)
		pShadow->SetOffset(Vec2(0.f, -50.f));

	WatchPlayer();
	SetFlyAnim();
	pMon->SetFly(true);
}

void CBirdManFly::Exit()
{
	CBirdManScript* pMon = ((CBirdManScript*)GetOwnerAI());
	m_fAccFlyTime = 0.f;
	pMon->SetFly(false);

	CShadowScript* pShadow = dynamic_cast<CShadowScript*>(pMon->GetOwner()->GetScript(SCRIPT_TYPE::SHADOWSCRIPT));
	if (pShadow)
		pShadow->SetOffset(Vec2(0.f, -40.f));
}

void CBirdManFly::SetFlyAnim()
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
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_BirdmanFly");
		pMon->Animator2D()->Play(L"spr_BirdmanFly", true);
	}
	// 왼쪽 보기
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_BirdmanFly");
		pMon->Animator2D()->Play(L"spr_BirdmanFly", true);
	}
}

void CBirdManFly::WatchPlayer()
{
	// 항상 플레이어 방향을 바라보게하기
	CBirdManScript* pMon = ((CBirdManScript*)GetOwnerAI());
	Vec3 vMonPos = pMon->Transform()->GetRelativePos();
	vMonPos.z = 0.f;
	Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
	vPlayerPos.z = 0.f;

	Vec3 vDir = vPlayerPos - vMonPos;
	vDir.Normalize();

	pMon->SetFaceDir(vDir);
}

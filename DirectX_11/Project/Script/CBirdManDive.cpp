#include "pch.h"
#include "CBirdManDive.h"

#include "CBirdManScript.h"
#include <Engine\CAnim2D.h>
#include "CAttackScript.h"

CBirdManDive::CBirdManDive()	:
	m_fDiveSpeed(300.f),
	m_pCurAnim(nullptr),
	m_fDiveTime(0.f),
	m_fAccDiveTime(0.f),
	m_fHeight(100.f),
	m_fDist(0.f),
	m_pAttack(nullptr)
{
}

CBirdManDive::~CBirdManDive()
{
}

void CBirdManDive::tick()
{
	CBirdManScript* pMon = ((CBirdManScript*)GetOwnerAI());
	Vec3 vMonPos = pMon->Transform()->GetRelativePos();
	if (m_fDist <= 0)
	{
		ChangeState(L"Fly");
	}
	else
	{
		m_fDist -= DT * m_fDiveSpeed;
		if (m_fAccDiveTime < m_fDiveTime)
		{
			m_fAccDiveTime += DT;
		}
		else
		{
			m_fAccDiveTime = m_fDiveTime;
		}

		vMonPos += (pMon->GetFaceDir() * (m_fAccDiveTime / m_fDiveTime) + Vec3(0.f, -1.f, 0.f) * (1 - m_fAccDiveTime / m_fDiveTime)) * m_fDiveSpeed * DT;
		pMon->Transform()->SetRelativePos(vMonPos);
	}
}

void CBirdManDive::Enter()
{
	// 높이를 고려한 돌진 방향 설정
	CBirdManScript* pMon = ((CBirdManScript*)GetOwnerAI());
	Vec3 vCurPos = pMon->Transform()->GetRelativePos();
	vCurPos.y -= m_fHeight;
	vCurPos.z = 0.f;
	Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
	vPlayerPos.z = 0.f;
	
	m_fDiveTime = m_fHeight / m_fDiveSpeed;
	m_fDist = Vec3::Distance(vPlayerPos, vCurPos) + 300.f;
	Vec3 vDir = vPlayerPos - vCurPos;
	vDir.Normalize();

	pMon->SetFaceDir(vDir);

	// 돌진 애니메이션 설정
	vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;
	
	Vec3 vOffset;
	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_BirdmanDive");
		pMon->Animator2D()->Play(L"spr_BirdmanDive", false);
		vOffset = Vec3(24.f, -24.f, 0.f);
	}
	// 왼쪽 보기
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_BirdmanDive");
		pMon->Animator2D()->Play(L"spr_BirdmanDive", false);
		vOffset = Vec3(-24.f, -24.f, 0.f);
	}

	m_pAttack = CResMgr::GetInst()->FindRes<CPrefab>(L"BirdManAttack")->Instantiate();
	m_pAttack->GetScript<CAttackScript>()->SetDamage(1.f);
	m_pAttack->GetScript<CAttackScript>()->SetFollowingObj(pMon->GetOwner());
	m_pAttack->GetScript<CAttackScript>()->SetFollowingObj(pMon->GetOwner());
	m_pAttack->GetScript<CAttackScript>()->SetOffset(vOffset);
	SpawnGameObject(m_pAttack, pMon->Transform()->GetRelativePos(), L"MonsterAttack");
}

void CBirdManDive::Exit()
{
	m_fAccDiveTime = 0.f;
	if (IsValid(m_pAttack))
	{
		DestoryObject(m_pAttack);
		m_pAttack = nullptr;
	}
}

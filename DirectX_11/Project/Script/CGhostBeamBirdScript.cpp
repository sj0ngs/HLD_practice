#include "pch.h"
#include "CGhostBeamBirdScript.h"

#include "CGhostBeamBirdIdle.h"
#include "CGhostBeamBirdAttack.h"
#include "CGhostBeamBirdWalk.h"

#include "CProjectileScript.h"
#include "CAttackScript.h"

CGhostBeamBirdScript::CGhostBeamBirdScript()	:
	CMonsterScript(SCRIPT_TYPE::GHOSTBEAMBIRDSCRIPT),
	m_bSound(false),
	m_bHit(false),
	m_fAccHitTime(0.f),
	m_fHitTime(0.5f)
{
	m_fHP = 5.f;
	m_vHitBoxSize = Vec3(80.f, 90.f, 1.f);
	AddState(L"Idle", new CGhostBeamBirdIdle());
	AddState(L"Attack", new CGhostBeamBirdAttack());
	AddState(L"Walk", new CGhostBeamBirdWalk());
}

CGhostBeamBirdScript::~CGhostBeamBirdScript()
{
}

void CGhostBeamBirdScript::begin()
{
	SpawnHitBox();
	m_pTarget = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	ChangeState(L"Idle");
}

void CGhostBeamBirdScript::tick()
{
	CMonsterScript::tick();
	if (IsDead())
		return;

	if (0.f >= m_fHP)
	{
		SetDead(true);
		m_fHP = 0.f;
		Vec3 vDir = GetFaceDir();
		vDir.Normalize();

		float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
		float fAngle = acosf(fCos);
		fAngle *= 180 / XM_PI;

		Collider2D()->OffCollision();
		Animator2D()->Play(L"spr_GhostBeamBirdDead", false);

		if (!m_bSound)
		{
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

			m_bSound = true;
		}

		// 오른쪽 보기
		if (90.f >= fAngle)
		{
			Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		}
		// 왼쪽 보기
		else
		{
			Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		}
		return;
	}

	AI::tick();
}

void CGhostBeamBirdScript::BeginOverlap(CCollider2D* _Other)
{
}

void CGhostBeamBirdScript::OnOverlap(CCollider2D* _Other)
{
}

void CGhostBeamBirdScript::EndOverlap(CCollider2D* _Other)
{
}

#include "pch.h"
#include "CBirdManScript.h"

#include <Engine\CLevelMgr.h>
#include "CBirdManIdle.h"
#include "CBirdManFly.h"
#include "CBirdManDive.h"
#include "CProjectileScript.h"
#include "CAttackScript.h"
#include "CShadowScript.h"

CBirdManScript::CBirdManScript() :
	CMonsterScript((UINT)SCRIPT_TYPE::BIRDMANSCRIPT),
	m_bFly(false),
	m_bSpawned(false),
	m_bSound(false)
{
	m_vHitBoxSize = Vec3(40.f, 60.f, 1.f);
	m_fHP = 1.f;
	AddState(L"Idle", new CBirdManIdle());
	AddState(L"Fly", new CBirdManFly());
	AddState(L"Dive", new CBirdManDive());
}

CBirdManScript::~CBirdManScript()
{
}

void CBirdManScript::begin()
{
	m_pTarget = CLevelMgr::GetInst()->FindObjectByName(L"Player");

	SpawnHitBox();

	if (!m_bSpawned)
		ChangeState(L"Idle");
	else if (m_bSpawned)
		ChangeState(L"Fly");
}

void CBirdManScript::tick()
{
	CMonsterScript::tick();

	if (IsDead())
		return;

	if (0.f >= m_fHP)
	{
		CShadowScript* pShadow = dynamic_cast<CShadowScript*>(GetOwner()->GetScript(SCRIPT_TYPE::SHADOWSCRIPT));
		if (pShadow)
			pShadow->SetOffset(Vec2(0.f, -25.f));
		SetDead(true);
		m_fHP = 0.f;
		Vec3 vDir = GetFaceDir();
		vDir.Normalize();
	
		float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
		float fAngle = acosf(fCos);
		fAngle *= 180 / XM_PI;

		Collider2D()->OffCollision();
		Animator2D()->Play(L"spr_BirdmanDead", false);

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

void CBirdManScript::TakeDamage(CCollider2D* _pOther)
{
	int iLayer = _pOther->GetOwner()->GetLayerIndex();
	if (iLayer == CLevelMgr::GetInst()->GetCurLevel()->FindLayerByName(L"PlayerProjectile")->GetLayerIdx())
	{
		m_fHP -= _pOther->GetOwner()->GetScript<CProjectileScript>()->GetDamage();
		ChangeState(L"Idle");
	}
	else if (iLayer == CLevelMgr::GetInst()->GetCurLevel()->FindLayerByName(L"PlayerAttack")->GetLayerIdx())
	{
		if (m_bFly)
			return;

		m_fHP -= _pOther->GetOwner()->GetScript<CAttackScript>()->GetDamage();
		ChangeState(L"Idle");
	}
}

void CBirdManScript::BeginOverlap(CCollider2D* _Other)
{
}

void CBirdManScript::OnOverlap(CCollider2D* _Other)
{
}

void CBirdManScript::EndOverlap(CCollider2D* _Other)
{
}

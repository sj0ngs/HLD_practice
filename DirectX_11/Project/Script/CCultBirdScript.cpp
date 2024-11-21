#include "pch.h"
#include "CCultBirdScript.h"

#include <Engine\CLevelMgr.h>

#include "CCultBirdIdle.h"
#include "CCultBirdMove.h"
#include "CCultBirdAttack.h"
#include "CCultBirdDodge.h"
#include "CCultBirdStunned.h"

#include "CProjectileScript.h"
#include "CAttackScript.h"

CCultBirdScript::CCultBirdScript() :
	CMonsterScript(SCRIPT_TYPE::CULTBIRDSCRIPT),
	m_fAttackCool(0.f),
	m_fDodgeCool(0.f)
{
	m_fHP = 2.f;
	m_vHitBoxSize = Vec3(30.f, 60.f, 1.f);
	AddState(L"Idle", new CCultBirdIdle());
	AddState(L"Move", new CCultBirdMove());
	AddState(L"Attack", new CCultBirdAttack());
	AddState(L"Dodge", new CCultBirdDodge());
	AddState(L"Stunned", new CCultBirdStunned());
}

CCultBirdScript::~CCultBirdScript()
{
}


void CCultBirdScript::begin()
{
	SpawnHitBox();
	m_pTarget = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	ChangeState(L"Idle");
}

void CCultBirdScript::tick()
{
	AI::tick();
	CMonsterScript::tick();

	if (m_fAttackCool > 0)
	{
		m_fAttackCool -= DT;
	}
	else
	{
		m_fAttackCool = 0;
	}

	if (m_fDodgeCool > 0)
	{
		m_fDodgeCool -= DT;
	}
	else
	{
		m_fDodgeCool = 0;
	}
}

void CCultBirdScript::TakeDamage(CCollider2D* _pOther)
{
	int iLayer = _pOther->GetOwner()->GetLayerIndex();
	if (iLayer == CLevelMgr::GetInst()->GetCurLevel()->FindLayerByName(L"PlayerProjectile")->GetLayerIdx())
	{
		m_fHP -= _pOther->GetOwner()->GetScript<CProjectileScript>()->GetDamage();
		ChangeState(L"Stunned");
		HitBlink();
	}
	else if (iLayer == CLevelMgr::GetInst()->GetCurLevel()->FindLayerByName(L"PlayerAttack")->GetLayerIdx())
	{
		m_fHP -= _pOther->GetOwner()->GetScript<CAttackScript>()->GetDamage();
		ChangeState(L"Stunned");
		HitBlink();
	}
}

void CCultBirdScript::BeginOverlap(CCollider2D* _Other)
{
}

void CCultBirdScript::OnOverlap(CCollider2D* _Other)
{
}

void CCultBirdScript::EndOverlap(CCollider2D* _Other)
{
}
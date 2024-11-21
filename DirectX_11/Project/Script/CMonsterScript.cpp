#include "pch.h"
#include "CMonsterScript.h"

#include "CHitDetectorScript.h"	
#include "CProjectileScript.h"
#include "CAttackScript.h"

CMonsterScript::CMonsterScript() :
	AI((UINT)SCRIPT_TYPE::MONSTERSCRIPT),
	m_vFaceDir(Vec3(1.f, 0.f, 0.f)),
	m_pTarget(nullptr),
	m_fHP(3),
	m_bHit(false),
	m_fAccTime(0.f),
	m_bInvin(false)
{
}

CMonsterScript::CMonsterScript(UINT _iScriptType)	:
	AI(_iScriptType),
	m_vFaceDir(Vec3(1.f, 0.f, 0.f)),
	m_pTarget(nullptr),
	m_fHP(3),
	m_bHit(false),
	m_fAccTime(0.f),
	m_bInvin(false)
{
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::begin()
{
	MeshRender()->GetDynamicMaterial();
}

void CMonsterScript::tick()
{
	if (m_bHit)
	{
		m_fAccTime += DT;

		if (0.25f <= m_fAccTime)
		{
			m_bHit = false;
			int a = 0;
			MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_1, &a);
			m_fAccTime = 0.f;
		}
	}
}

void CMonsterScript::TakeDamage(CCollider2D* _pOther)
{
	if (m_bInvin)
		return;

	int iLayer = _pOther->GetOwner()->GetLayerIndex();
	if (iLayer == CLevelMgr::GetInst()->GetCurLevel()->FindLayerByName(L"PlayerProjectile")->GetLayerIdx())
	{
		m_fHP -= _pOther->GetOwner()->GetScript<CProjectileScript>()->GetDamage();
		HitBlink();
	}
	else if (iLayer == CLevelMgr::GetInst()->GetCurLevel()->FindLayerByName(L"PlayerAttack")->GetLayerIdx())
	{
		m_fHP -= _pOther->GetOwner()->GetScript<CAttackScript>()->GetDamage();
		HitBlink();
	}
}

void CMonsterScript::BeginOverlap(CCollider2D* _Other)
{
}

void CMonsterScript::OnOverlap(CCollider2D* _Other)
{
}

void CMonsterScript::EndOverlap(CCollider2D* _Other)
{
}

void CMonsterScript::SpawnHitBox()
{
	CGameObject* pHitBox = CResMgr::GetInst()->FindRes<CPrefab>(L"HitDetector")->Instantiate();
	pHitBox->Transform()->SetRelativeScale(m_vHitBoxSize);
	CHitDetectorScript* pScript = dynamic_cast<CHitDetectorScript*>(pHitBox->GetScript(SCRIPT_TYPE::HITDETECTORSCRIPT));
	if (pScript)
		pScript->SetOwnerAI(this);
	SpawnGameObject(pHitBox, Transform()->GetRelativePos(), L"MonsterHitBox");
}

void CMonsterScript::HitBlink()
{
	m_bHit = true;

	int iOn = 2;
	MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_1, &iOn);
	Vec4 vColor = Vec4(3.f, 0.f, 0.f, 1.f);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vColor);
}

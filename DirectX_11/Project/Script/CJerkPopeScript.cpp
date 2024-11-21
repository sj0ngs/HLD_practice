#include "pch.h"
#include "CJerkPopeScript.h"
#include "CJerkPopeIdle.h"
#include "CJerkPopeAttack.h"
#include "CJerkPopeSpawn.h"
#include "CJerkPopeTeleport.h"

#include "CProjectileScript.h"
#include "CAttackScript.h"

CJerkPopeScript::CJerkPopeScript()	:
	CMonsterScript(SCRIPT_TYPE::JERKPOPESCRIPT),
	m_fAttackCool(5.f),
	m_iTraceAttack(0),
	m_iTileAttack(0),
	m_bMoveCenter(false),
	m_bAttackAvail(false),
	m_bPhase2(false),
	m_bInvoke(false),
	m_bSound(false),
	m_iGridRow(0),
	m_iGridCol(0)
{
	m_vHitBoxSize = Vec3(100.f, 110.f, 1.f);
	m_fHP = 50.f;

	AddState(L"Idle", new CJerkPopeIdle());
	AddState(L"Attack", new CJerkPopeAttack());
	AddState(L"Spawn", new CJerkPopeSpawn());
	AddState(L"Teleport", new CJerkPopeTeleport());
}

CJerkPopeScript::~CJerkPopeScript()
{
}

void CJerkPopeScript::MakeGridMap(Vec3 _vCenterPos, int _iRow, int _iCol, float _fTileSize)
{
	m_iGridRow = _iRow;
	m_iGridCol = _iCol;

	Vec3 vLeftTop = _vCenterPos;
	vLeftTop.x += (float)(1 - _iCol) * (_fTileSize / 2.f);
	vLeftTop.y += (float)(_iRow - 1) * (_fTileSize / 2.f);

	for (int i = 0; i < m_iGridRow; i++)
	{
		for (int j = 0; j < m_iGridCol; j++)
		{
			m_matTile[i][j].vPos = vLeftTop + Vec3(_fTileSize * j, _fTileSize * -i, 0.f);
			m_matTile[i][j].bUse = false;
		}
	}
}

void CJerkPopeScript::begin()
{
	Vec3 vCenterPos = Vec3(0.f, -178.f, 600.f);
	//vCenterPos.z = 600.f;
	MakeGridMap(vCenterPos, 6, 10, 96.f);

	SpawnHitBox();
	m_pTarget = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	ChangeState(L"Idle");
}

void CJerkPopeScript::tick()
{
	CMonsterScript::tick();
	if (IsDead())
		return;

	// HP 0 보다 낮으면 사망 애니메이션 실행
	if (m_fHP <= 0.f)
	{
		SetDead(true);
		m_fHP = 0.f;
		Vec3 vDir = GetFaceDir();
		vDir.Normalize();

		float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
		float fAngle = acosf(fCos);
		fAngle *= 180 / XM_PI;

		Collider2D()->OffCollision();
		Animator2D()->Play(L"spr_JerkPopeDeath", false);

		if (!m_bSound)
		{
			CResMgr::GetInst()->FindRes<CSound>(L"snd_JerkPopeDeath")->Play(1, 10.f, true);
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
	else if (!m_bPhase2 && m_fHP <= 25.f)
	{
		m_bPhase2 = true;
	}

	// 항상 플레이어 방향을 바라보게하기
	Vec3 vMonPos = Transform()->GetRelativePos();
	vMonPos.z = 0.f;
	Vec3 vPlayerPos = m_pTarget->Transform()->GetRelativePos();
	vPlayerPos.z = 0.f;

	m_vFaceDir = vPlayerPos - vMonPos;
	m_vFaceDir.Normalize();

	AI::tick();

	if (!m_bAttackAvail)
	{
		m_fAttackCool -= DT;

		if (m_fAttackCool <= 0.f)
		{
			m_fAttackCool = 3.f;
			m_bAttackAvail = true;
		}
	}
}

void CJerkPopeScript::BeginOverlap(CCollider2D* _Other)
{
}

void CJerkPopeScript::OnOverlap(CCollider2D* _Other)
{
}

void CJerkPopeScript::EndOverlap(CCollider2D* _Other)
{
}

void CJerkPopeScript::ClearMatrix()
{
	for (int i = 0; i < m_iGridRow; i++)
	{
		for (int j = 0; j < m_iGridCol; j++)
		{
			m_matTile[i][j].bUse = false;
		}
	}
}

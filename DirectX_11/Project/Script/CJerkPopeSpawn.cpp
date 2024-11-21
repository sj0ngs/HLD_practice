#include "pch.h"
#include "CJerkPopeSpawn.h"

#include <Engine\CAnim2D.h>
#include "CJerkPopeScript.h"
#include "CBirdManScript.h"

CJerkPopeSpawn::CJerkPopeSpawn()	:
	m_pCurAnim(nullptr),
	m_bSpawned(false),
	m_fTime(5.f),
	m_fAccTime(0.f)
{
	Clear();
}

CJerkPopeSpawn::~CJerkPopeSpawn()
{
}

void CJerkPopeSpawn::tick()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());

	if (!m_bSpawned && m_pCurAnim->GetCurFrameNotify() == L"Spawn")
	{
		m_bSpawned = true;
		Spawn();
	}

	//m_fAccTime += DT;
	//if (m_fAccTime >= m_fTime)
	//{
	//	ChangeState(L"Idle");
	//}

	bool bAllDead = false;
	for (int i = 0; i < 4; i++)
	{
		if (IsValid(m_arrBirdMan[i]))
		{
			CBirdManScript* pScript = static_cast<CBirdManScript*>(m_arrBirdMan[i]->GetScript(SCRIPT_TYPE::BIRDMANSCRIPT));

			if (pScript->IsDead())
				bAllDead = true;
			else
				bAllDead = false;
		}
	}

	if (bAllDead)
		ChangeState(L"Idle");
}

void CJerkPopeSpawn::Enter()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());

	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// ¿À¸¥ÂÊ º¸±â
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_JerkPopeRaise");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_JerkPopeRaise", false);
	}
	// ¿ÞÂÊ º¸±â
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_JerkPopeRaise");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_JerkPopeRaise", false);
	}
}

void CJerkPopeSpawn::Exit()
{
	m_fAccTime = 0.f;
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());
	pMon->SetTraceAttack(0);
	pMon->SetTileAttack(0);
	pMon->SetAttackCool(3.f);

	m_pCurAnim = nullptr;
	m_bSpawned = false;
	Clear();
}

void CJerkPopeSpawn::Spawn()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());
	
	// ÁÂ»ó´Ü
	CGameObject* pSpawn = CResMgr::GetInst()->FindRes<CPrefab>(L"BirdMan")->Instantiate();
	pSpawn->GetScript<CBirdManScript>()->SetSpawned(true);
	SpawnGameObject(pSpawn, pMon->m_matTile[1][1].vPos, L"Monster");
	m_arrBirdMan[0] = pSpawn;

	// ÁÂÇÏ´Ü
	pSpawn = CResMgr::GetInst()->FindRes<CPrefab>(L"BirdMan")->Instantiate();
	pSpawn->GetScript<CBirdManScript>()->SetSpawned(true);
	SpawnGameObject(pSpawn, pMon->m_matTile[1][pMon->GetGridCol() - 2].vPos, L"Monster");
	m_arrBirdMan[1] = pSpawn;

	// ¿ì»ó´Ü
	pSpawn = CResMgr::GetInst()->FindRes<CPrefab>(L"BirdMan")->Instantiate();
	pSpawn->GetScript<CBirdManScript>()->SetSpawned(true);
	SpawnGameObject(pSpawn, pMon->m_matTile[pMon->GetGridRow() - 2][1].vPos, L"Monster");
	m_arrBirdMan[2] = pSpawn;

	// ÁÂÇÏ´Ü
	pSpawn = CResMgr::GetInst()->FindRes<CPrefab>(L"BirdMan")->Instantiate();
	pSpawn->GetScript<CBirdManScript>()->SetSpawned(true);
	SpawnGameObject(pSpawn, pMon->m_matTile[pMon->GetGridRow() - 2][pMon->GetGridCol() - 2].vPos, L"Monster");
	m_arrBirdMan[3] = pSpawn;

	CResMgr::GetInst()->FindRes<CSound>(L"snd_JerkPopeSummon")->Play(1, 10.f, true);
}

void CJerkPopeSpawn::Clear()
{
	for (int i = 0; i < 4; i++)
	{
		m_arrBirdMan[i] = nullptr;
	}
}

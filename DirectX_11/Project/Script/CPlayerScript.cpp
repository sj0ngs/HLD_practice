#include "pch.h"
#include "CPlayerScript.h"

#include "CMonsterScript.h"

#include <Engine\CDevice.h>
#include <Engine\CRenderMgr.h>
#include <Engine\CAnim2D.h>
#include <Engine\CFontMgr.h>

#include "CPlayerIdle.h"
#include "CPlayerMove.h"
#include "CPlayerDash.h"
#include "CPlayerAttack.h"
#include "CPlayerShoot.h"
#include "CPlayerHeal.h"
#include "CPlayerStunned.h"
#include "CPlayerFall.h"
#include "CPlayerVictory.h"

#include "CAttackScript.h"
#include "CProjectileScript.h"

#include "CHUDScript.h"
#include "CRoomScript.h"

#include "CHitDetectorScript.h"
#include "CCameraMoveScript.h"

CPlayerScript::CPlayerScript() :
	AI((UINT)SCRIPT_TYPE::PLAYERSCRIPT),
	m_fMoveSpeed(300.f),
	m_vFaceDir{ 0.f, 1.f, 0.f },
	m_fHP(MAX_HP),
	m_fStamina(100.f),
	m_fStaminaUse(20.f),
	m_fAmmo(100.f),
	m_iHealthKit(3),
	m_eGunType(EGUN_TYPE::PISTOL),
	m_iGranade(3),
	m_bInvin(false),
	m_pHUD(nullptr),
	m_pRoom(nullptr),
	m_bAmmoRegain(false),
	m_fAccRegainTime(0.f),
	m_fRegainTime(0.25f),
	m_bHit(false),
	m_fAccHitTime(0.f),
	m_fHitTime(0.25f),
	m_bHeal(false),
	m_fAccHealTime(0.f),
	m_fHealTime(0.25f)
{
	AddScriptParam((void*)&m_fMoveSpeed, SCRIPT_PARAM::FLOAT, "Move Speed");
	AddState(L"Idle", new CPlayerIdle());
	AddState(L"Move", new CPlayerMove());
	AddState(L"Dash", new CPlayerDash());
	AddState(L"Attack", new CPlayerAttack());
	AddState(L"Shoot", new CPlayerShoot());
	AddState(L"Heal", new CPlayerHeal());
	AddState(L"Stunned", new CPlayerStunned());
	AddState(L"Fall", new CPlayerFall());
	AddState(L"Victory", new CPlayerVictory());
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::AmmoRegain(DWORD_PTR _pRegain)
{
	float fRegain = (float)_pRegain;

	m_fAmmo += _pRegain;

	if (m_fAmmo >= 100.f)
		m_fAmmo = 100.f;

	m_pHUD->AmmoChange(m_fAmmo);

	m_bAmmoRegain = true;

	int iRegain = 2;
	Vec4 vColor = Vec4(5.f, 5.f, 5.f, 1.f);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_1, &iRegain);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vColor);
}

void CPlayerScript::begin()
{
	LoadFromFile();
	MeshRender()->GetDynamicMaterial();
	ChangeState(L"Idle");

	// 히트박스생성
	CGameObject* pHitBox = CResMgr::GetInst()->FindRes<CPrefab>(L"HitDetector")->Instantiate();
	pHitBox->Transform()->SetRelativeScale(Vec3(20.f, 52.f, 1.f));
	CHitDetectorScript* pScript = dynamic_cast<CHitDetectorScript*>(pHitBox->GetScript(SCRIPT_TYPE::HITDETECTORSCRIPT));
	if (pScript)
		pScript->SetOwnerAI(this);
	SpawnGameObject(pHitBox, Transform()->GetRelativePos(), L"PlayerHitBox");
}

void CPlayerScript::end()
{
	if (GetOwner()->GetLayerIndex() >= 0)
	{
		if (CLevelMgr::GetInst()->GetCurLevel() != nullptr &&
			CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
			SaveToFile();
	}
}

void CPlayerScript::tick()
{
	if (KEY_TAP(KEY::B))
	{
		int iGun = (UINT)m_eGunType + 1;
		if (iGun > (UINT)EGUN_TYPE::HANDCANNON)
			iGun = 0;

		m_eGunType = (EGUN_TYPE)iGun;
		m_pHUD->WeaponChange();

		CResMgr::GetInst()->FindRes<CSound>(L"snd_WeaponSwap")->Play(1, 10.f, true);
	}

	if (m_fStamina < 100.f)
	{
		m_fStamina += DT * 25.f;

		if (m_fStamina > 100.f)
			m_fStamina = 100.f;

		m_pHUD->StaminaChange(m_fStamina);
	}

	int iColorChange = 0;
	// Ammo Regain Color Change
	if (m_bAmmoRegain)
	{
		m_fAccRegainTime += DT;
		if (m_fAccRegainTime >= m_fRegainTime)
		{
			m_bAmmoRegain = false;
			m_fAccRegainTime = 0.f;
			MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_1, &iColorChange);
		}
	}

	// Heal 
	if (m_bHeal)
	{
		m_fAccHealTime += DT;
		if (m_fAccHealTime >= m_fHealTime)
		{
			m_bHeal = false;
			m_fAccHealTime = 0.f;
			MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_1, &iColorChange);
		}
	}

	// Hit
	if (m_bHit)
	{
		m_fAccHitTime += DT;
		if (m_fAccHitTime >= m_fHitTime)
		{
			m_bHit = false;
			m_fAccHitTime = 0.f;
			MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_1, &iColorChange);
		}
	}


	AI::tick();
	return;

	Vec3 vCurPos = Transform()->GetRelativePos();

	// 마우스 클릭 이동
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec3 vMousePos = CKeyMgr::GetInst()->GetMouseWorldPos();
		Vec3 vMousePos3D = Vec3(vMousePos.x, vMousePos.y, vCurPos.z);

		Vec3 vDir = vMousePos3D - vCurPos;
		vDir.Normalize();

		vCurPos.x += vDir.x * DT * m_fMoveSpeed;
		vCurPos.y += vDir.y * DT * m_fMoveSpeed;

		Transform()->SetRelativePos(vCurPos);
	}

	Dissolve();
}

bool CPlayerScript::Shoot()
{
	float fUse = 0.f;

	switch (m_eGunType)
	{
	case EGUN_TYPE::PISTOL:
		fUse = 100.f / 6.f;
		break;
	case EGUN_TYPE::RAILGUN:
		fUse = 100.f / 4.f;
		break;
	case EGUN_TYPE::SHOTGUN:
		fUse = 100.f / 3.f;
		break;
	case EGUN_TYPE::HANDCANNON:
		fUse = 100.f / 4.f;
		break;
	}

	if (m_fAmmo >= fUse)
	{
		m_fAmmo -= fUse;
		m_pHUD->AmmoChange(m_fAmmo);
		return true;
	}
	
	return false;
}

bool CPlayerScript::Dash()
{
	if(m_fStamina < m_fStaminaUse)
		return false;
	
	m_fStamina -= m_fStaminaUse;
	return true;
}

void CPlayerScript::Heal()
{
	m_fHP = MAX_HP;
	m_pHUD->HPChange(m_fHP);

	m_iHealthKit--;
	m_pHUD->HealthKitChange(m_iHealthKit);

	int iVal = 2;
	Vec4 vColor = Vec4(1.2f, 1.f, 0.5f, 1.f);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_1, &iVal);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vColor);

	m_bHeal = true;
}

void CPlayerScript::RoomCollisionCheck(int _iType)
{
	if (nullptr == m_pRoom)
		return;

	CTransform* pTrans = Transform();
	Vec2 vPos = {};
	vPos.x = pTrans->GetRelativePos().x;
	vPos.y = pTrans->GetRelativePos().y - 23.f;

	float fSize = 7.5f;
	m_pRoom->RoomCollisionCheckAndMove(vPos, fSize, pTrans, _iType);
}

int CPlayerScript::RoomCollisionCheckType()
{
	if (nullptr == m_pRoom)
		return 0;

	CTransform* pTrans = Transform();
	Vec2 vPos = {};
	vPos.x = pTrans->GetRelativePos().x;
	vPos.y = pTrans->GetRelativePos().y - 23.f;
	float fSize = 7.5f;
	return m_pRoom->RoomCollisionCheckFloorType(vPos, fSize);
}
void CPlayerScript::TakeDamage(float _fDmg)
{
	m_fHP -= _fDmg;
	if (m_fHP < 0)
		m_fHP = 0.f;

	m_pHUD->HPChange(m_fHP);

	m_bHit = true;
	int iVal = 2;
	Vec4 vColor = Vec4(3.f, 0.f, 0.f, 1.f);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_1, &iVal);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vColor);

	CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();
	pCam->GetOwner()->GetScript<CCameraMoveScript>()->BeginShake(Vec3(0.f, 1.f, 0.f), 0.25f, 100.f);
}

void CPlayerScript::Victory()
{
	ChangeState(L"Victory");
}

void CPlayerScript::TakeDamage(CCollider2D* _pOther)
{
	if (m_bInvin)
		return;

	if (_pOther->GetOwner()->GetLayerIndex() == CLevelMgr::GetInst()->GetLayerIdx(L"MonsterProjectile") || 
		_pOther->GetOwner()->GetLayerIndex() == CLevelMgr::GetInst()->GetLayerIdx(L"MonsterAttack"))
	{
		float fDmg = 0.f;
		// 몬스터 공격일 때
		if (_pOther->GetOwner()->GetLayerIndex() == CLevelMgr::GetInst()->GetLayerIdx(L"MonsterAttack"))
		{
			fDmg = _pOther->GetOwner()->GetScript<CAttackScript>()->GetDamage();
			ChangeState(L"Stunned");
			m_bHit = true;

			TakeDamage(fDmg);
		}
		// 몬스터 투사체일 때
		else if (_pOther->GetOwner()->GetLayerIndex() == CLevelMgr::GetInst()->GetLayerIdx(L"MonsterProjectile"))
		{
			fDmg = _pOther->GetOwner()->GetScript<CProjectileScript>()->GetDamage();
			ChangeState(L"Stunned");
			m_bHit = true;

			TakeDamage(fDmg);
		}

		//if (m_fHP < 0)
		//	m_fHP = 0.f;

		//m_pHUD->HPChange(m_fHP);

		//m_bHit = true;
		//int iVal = 2;
		//Vec4 vColor = Vec4(3.f, 0.f, 0.f, 1.f);
		//MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_1, &iVal);
		//MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vColor);
	}
}

void CPlayerScript::BeginOverlap(CCollider2D* _Other)
{
}

void CPlayerScript::OnOverlap(CCollider2D* _Other)
{
}

void CPlayerScript::EndOverlap(CCollider2D* _Other)
{
}

void CPlayerScript::LoadFromLevelFile(FILE* _pFile)
{
	CScript::LoadFromLevelFile(_pFile);

	for (size_t i = 0; i < m_vecParam.size(); i++)
	{
		int iType = 0;
		fread(&iType, sizeof(int), 1, _pFile);
		m_vecParam[i].eParam = (SCRIPT_PARAM)iType;

		int iData = 0;
		float fData = 0;
		Vec2 v2Data;
		Vec3 v3Data;
		Vec4 v4Data;
		switch (m_vecParam[i].eParam)
		{
		case SCRIPT_PARAM::INT:
		{
			fread(&iData, sizeof(iData), 1, _pFile);
		}
		break;
		case SCRIPT_PARAM::FLOAT:
		{
			fread(&fData, sizeof(fData), 1, _pFile);
		}
		break;
		case SCRIPT_PARAM::VEC2:
		{
			fread(&v2Data, sizeof(v2Data), 1, _pFile);
		}
		break;
		case SCRIPT_PARAM::VEC3:
		{
			fread(&v3Data, sizeof(v3Data), 1, _pFile);
		}
		break;
		case SCRIPT_PARAM::VEC4:
		{
			fread(&v4Data, sizeof(v4Data), 1, _pFile);
		}
		break;
		}

		Loadstring(m_vecParam[i].strDesc, _pFile);
	
		if (m_vecParam[i].strDesc == "Move Speed")
		{
			m_fMoveSpeed = fData;
			m_vecParam[i].pData = (void*)&m_fMoveSpeed;
		}
	}
}

// Dissolve Effect
// g_tex_2           : Dissolve Texture
// g_float_0         : Edge Size
// g_float_1         : Dissolve Amount   
// g_vec4_0          : Edge Color
void CPlayerScript::Dissolve()
{
	static float fDissTime = 0.f;

	fDissTime += DT;

	if (5.f < fDissTime)
	{
		fDissTime -= 5.f;
	}

	float fDissAmount = fDissTime / 5.f;
	float fEdgeSize = 0.1f;
	Vec4 vDissColor = Vec4(0.f, 0.f, 1.f, 1.f);

	MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &fDissAmount);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_1, &fEdgeSize);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_0, &vDissColor);

	Vec4 vResolution = Vec4((float)MeshRender()->GetMaterial()->GetTexParam(TEX_0)->Width(),
							(float)MeshRender()->GetMaterial()->GetTexParam(TEX_0)->Height(),
							0.f, 0.f);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vResolution);
}

void CPlayerScript::LoadFromFile()
{
	wstring strPath = CPathMgr::GetInst()->GetContentPath();
	strPath += L"\\Player.data";

	FILE* pFile = nullptr;

	errno_t iErrNo = _wfopen_s(&pFile, strPath.c_str(), L"rb");

	// 파일이 없으면 불러오지 않음
	if (nullptr == pFile)
	{
		return;
	}

	// Load
	fread(&m_fHP, sizeof(m_fHP), 1, pFile);
	fread(&m_fAmmo, sizeof(m_fAmmo), 1, pFile);
	fread(&m_iHealthKit, sizeof(m_iHealthKit), 1, pFile);
	fread(&m_eGunType, sizeof(m_eGunType), 1, pFile);
	fread(&m_iGranade, sizeof(m_iGranade), 1, pFile);

	fclose(pFile);
}

void CPlayerScript::SaveToFile()
{
	wstring strPath = CPathMgr::GetInst()->GetContentPath();
	strPath += L"\\Player.data";

	FILE* pFile = nullptr;
	errno_t iErrNo = _wfopen_s(&pFile, strPath.c_str(), L"wb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Player Save 실패, Error Number = %d", iErrNo);
		MessageBox(nullptr, szStr, L"플레이어 저장 실패", MB_OK);
		return;
	}

	// Save
	fwrite(&m_fHP, sizeof(m_fHP), 1, pFile);
	fwrite(&m_fAmmo, sizeof(m_fAmmo), 1, pFile);
	fwrite(&m_iHealthKit, sizeof(m_iHealthKit), 1, pFile);
	fwrite(&m_eGunType, sizeof(m_eGunType), 1, pFile);
	fwrite(&m_iGranade, sizeof(m_iGranade), 1, pFile);

	fclose(pFile);
}

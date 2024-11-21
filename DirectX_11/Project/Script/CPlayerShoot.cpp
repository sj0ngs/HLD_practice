#include "pch.h"
#include "CPlayerShoot.h"

#include "CPlayerScript.h"
#include <Engine\CAnim2D.h>
#include "CPlayerIdle.h"
#include "CProjectileScript.h"
#include <Engine\CResMgr.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CRenderMgr.h>
#include "CCameraMoveScript.h"

CPlayerShoot::CPlayerShoot()	:
	m_eFaceDir(EFACE_DIR::SOUTH),
	m_pCurAnim(nullptr),
	m_bAble(false),
	m_bShot(false)
{
}

CPlayerShoot::~CPlayerShoot()
{
}

void CPlayerShoot::tick()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	if (m_pCurAnim->GetCurFrameNotify() == L"Shoot")
	{
		if (!m_bAble)
		{
			CResMgr::GetInst()->FindRes<CSound>(L"snd_LastShot")->Play(1, 10.f, true);
			ChangeState(L"Idle");
			return;
		}

		if (!m_bShot)
		{
			switch (pPlayer->GetGunType())
			{
			case EGUN_TYPE::PISTOL:
				ShootPistol();
				break;
			case EGUN_TYPE::RAILGUN:
				ShootRailGun();
				break;
			case EGUN_TYPE::SHOTGUN:
				ShootShotGun();
				break;
			case EGUN_TYPE::HANDCANNON:
				ShootHandCannon();
				break;
			}
			m_bShot = true;
		}

	}
	else if (m_pCurAnim->IsFinish())
	{
		ChangeState(L"Idle");
	}
}

void CPlayerShoot::Enter()
{
	SetShootDir();
}

void CPlayerShoot::Exit()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	CPlayerIdle* pIdle = (CPlayerIdle*)pPlayer->FindState(L"Idle");
	pIdle->SetFaceDir(m_eFaceDir);
	m_bShot = false;
}

void CPlayerShoot::SetShootDir()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	Vec3 vFaceDir = pPlayer->GetFaceDir();
	vFaceDir.Normalize();
	float fCos = vFaceDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	wstring strAnimName;

	m_bAble = pPlayer->Shoot();

	switch (pPlayer->GetGunType())
	{
	case EGUN_TYPE::PISTOL:
		strAnimName = L"spr_charpistol";
		break;
	case EGUN_TYPE::RAILGUN:
	{
		strAnimName = L"spr_charrifle";
		// Muzzle effect 생성
		if (m_bAble)
		{
			CGameObject* pEffect = CResMgr::GetInst()->FindRes<CPrefab>(L"RailGunMuzzle")->Instantiate();
			pEffect->Transform()->SetRelativeScale(Vec3(18.f, 29.f, 1.f) * 2.f);
			pEffect->Transform()->SetRelativeRot(Vec3(0.f, 0.f, CalculateAngle()));
			SpawnGameObject(pEffect, CalculateSpawnPos(50.f), L"PlayerProjectile");
		}
		break;
	}
	case EGUN_TYPE::SHOTGUN:
		strAnimName = L"spr_charrifle";
		break;
	case EGUN_TYPE::HANDCANNON:
		strAnimName = L"spr_charpistol";
		break;
	}


	pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	Vec3 vScale = Vec3(1.f, 1.f, 1.f);
	// Right Side
	if (22.5f >= fAngle)
	{
		vScale.x = 32.f;
		vScale.y = 32.f;
		strAnimName += L"side";
		m_eFaceDir = EFACE_DIR::EAST;
	}
	else if (67.5f >= fAngle)
	{
		vScale.x = 64.f;
		vScale.y = 64.f;
		// 우상단
		if (0 <= vFaceDir.y)
		{
			strAnimName += L"upside";
		}
		// 우하단
		else
		{
			strAnimName += L"downside";
		}
		m_eFaceDir = EFACE_DIR::EAST;
	}
	else if (112.5f >= fAngle)
	{
		vScale.x = 64.f;
		vScale.y = 64.f;
		// 상단
		if (0 <= vFaceDir.y)
		{
			strAnimName += L"up";
			m_eFaceDir = EFACE_DIR::NORTH;
		}
		// 하단
		else
		{
			strAnimName += L"down";
			m_eFaceDir = EFACE_DIR::SOUTH;
		}
	}
	else if (157.5f >= fAngle)
	{
		vScale.x = 64.f;
		vScale.y = 64.f;
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		// 좌상단
		if (0 <= vFaceDir.y)
		{
			strAnimName += L"upside";
		}
		// 좌하단
		else
		{
			strAnimName += L"downside";
		}
		m_eFaceDir = EFACE_DIR::WEST;
	}
	// Left Side
	else
	{
		vScale.x = 32.f;
		vScale.y = 32.f;
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		strAnimName += L"side";
		m_eFaceDir = EFACE_DIR::WEST;
	}

	pPlayer->Transform()->SetRelativeScale(vScale * 2.f);
	m_pCurAnim = pPlayer->Animator2D()->FindAnim(strAnimName);
	m_pCurAnim->Reset();
	pPlayer->Animator2D()->Play(strAnimName, false);
}

void CPlayerShoot::ShootPistol()
{
	int iRand = GetRandInt(2);
	switch (iRand)
	{
	case 0:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_Pistol1")->Play(1, 10.f, true);
		break;
	case 1:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_Pistol2")->Play(1, 10.f, true);
		break;
	case 2:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_Pistol3")->Play(1, 10.f, true);
		break;
	}

	CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"PistolBullet")->Instantiate();
	pProj->Transform()->SetRelativeRot(Vec3(0.f, 0.f, CalculateAngle()));

	// 속도등 스크립트 설정
	CProjectileScript* pScript = pProj->GetScript<CProjectileScript>();
	if (nullptr != pScript)
	{
		pScript->SetSpeed(1000.f);
		pScript->SetDir(GetPlayerDir());
		pScript->SetDist(2000.f);
		pScript->SetDamage(1);
		pScript->SetEndSound(CResMgr::GetInst()->FindRes<CSound>(L"snd_BulletHitWall"));
	}

	// 레벨에 추가
	SpawnGameObject(pProj, CalculateSpawnPos(30.f), L"PlayerProjectile");

	// Muzzle effect 생성
	CGameObject* pEffect = CResMgr::GetInst()->FindRes<CPrefab>(L"PistolMuzzle")->Instantiate();
	SpawnGameObject(pEffect, CalculateSpawnPos(30.f), L"PlayerProjectile");

	Recoil(0.1f, 300.f);
}

void CPlayerShoot::ShootRailGun()
{
	CResMgr::GetInst()->FindRes<CSound>(L"snd_RailgunFire")->Play(1, 10.f, true);

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	//const vector<CGameObject*>& vecMon = pCurLevel->FindLayerByName(L"Monster")->GetParentObject();
	//Vec3 vMonPos = RayCast(GetPlayerDir(), vecMon);
	
	const vector<CGameObject*>& vecObs = pCurLevel->FindLayerByName(L"Obstacle")->GetParentObject();
	Vec3 vObstaclePos = RayCast(GetPlayerDir(), vecObs);
	Vec3 vPlayerPos = CalculateSpawnPos(40.f);

	Vec3 vPoint = vPlayerPos;
	//float fMonDist = Vector3::Distance(vPlayerPos, vMonPos);
	float fObsDist = Vector3::Distance(vPlayerPos, vObstaclePos);

	//if (fMonDist <= fObsDist)
	//	vPoint = vMonPos;
	//else
	vPoint = vObstaclePos;

	CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"RailgunBeam")->Instantiate();

	vPoint.z = vPlayerPos.z;
	float fXScale = Vector3::Distance(vPoint, vPlayerPos);
	pProj->Transform()->SetRelativeScale(Vec3(fXScale, 11.f, 1.f));
	pProj->Transform()->SetRelativeRot(Vec3(0.f, 0.f, CalculateAngle()));

	// 속도등 스크립트 설정
	CProjectileScript* pScript = pProj->GetScript<CProjectileScript>();
	if (nullptr != pScript)
	{
		pScript->SetSpeed(0.f);
		pScript->SetDir(GetPlayerDir());
		pScript->SetDist(100.f);
		pScript->SetDamage(2);
	}

	Vec3 vSpawnPos = (vPlayerPos + vPoint) / 2.f;

	pProj->SetLifeSpan(0.25f);
	// 레벨에 추가
	SpawnGameObject(pProj, vSpawnPos, L"PlayerProjectile");

	// Muzzle effect 생성
	CGameObject* pEffect = CResMgr::GetInst()->FindRes<CPrefab>(L"RailGunBlastMuzzle")->Instantiate();
	pEffect->Transform()->SetRelativeScale(Vec3(32.f, 46.f, 1.f) * 2.f);
	pEffect->Transform()->SetRelativeRot(Vec3(0.f, 0.f, CalculateAngle()));
	vSpawnPos = CalculateSpawnPos(50.f);
	vSpawnPos.z -= 1.f;
	SpawnGameObject(pEffect, CalculateSpawnPos(50.f), L"PlayerProjectile");

	Recoil(0.25f, 300.f);
}

void CPlayerShoot::ShootShotGun()
{
	int iRand = GetRandInt(2);
	switch (iRand)
	{
	case 0:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_TripleShotFire1")->Play(1, 10.f, true);
		break;
	case 1:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_TripleShotFire2")->Play(1, 10.f, true);
		break;
	case 2:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_TripleShotFire3")->Play(1, 10.f, true);
		break;
	}

	// 속도등 스크립트 설정
	for (int i = 0; i < 10; i++)
	{
		CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"ShotgunBullet")->Instantiate();
		// scale 설정
		pProj->Transform()->SetRelativeScale(Vec3(17, 16.f, 1.f) * 2.f);
		CProjectileScript* pScript = pProj->GetScript<CProjectileScript>();

		if (nullptr != pScript)
		{
			pScript->SetSpeed(500.f);
			float fAngle = CalculateAngle();
			fAngle += XMConvertToRadians((float)GetRandInt(45, -45));

			Matrix matRot = XMMatrixIdentity();
			matRot *= XMMatrixRotationZ(fAngle);
			Vec4 vDir = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), matRot);
			pScript->SetDir(vDir);
			pScript->SetDist(150.f);
			pScript->SetDeadAnim(L"spr_DiamondBlunderShotDie");
			pScript->SetDeadSize(Vec3(16.f, 16.f, 0.f) * 2.f);
			pScript->SetDamage(0.5f);
			pScript->SetEndSound(CResMgr::GetInst()->FindRes<CSound>(L"snd_BulletHitWall"));
		}

		float fOffset = (float)GetRandInt(50, 20);
		SpawnGameObject(pProj, CalculateSpawnPos(fOffset), L"PlayerProjectile");
	}

	// Muzzle effect 생성
	CGameObject* pEffect = CResMgr::GetInst()->FindRes<CPrefab>(L"ShotgunMuzzle")->Instantiate();
	pEffect->Transform()->SetRelativeRot(Vec3(0.f, 0.f, CalculateAngle()));
	SpawnGameObject(pEffect, CalculateSpawnPos(50.f), L"PlayerProjectile");

	Recoil(0.1f, 400.f);
}

void CPlayerShoot::ShootHandCannon()
{
	int iRand = GetRandInt(2);
	switch (iRand)
	{
	case 0:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_ZeliskaFire1")->Play(1, 10.f, true);
		break;
	case 1:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_ZeliskaFire2")->Play(1, 10.f, true);
		break;
	case 2:
		CResMgr::GetInst()->FindRes<CSound>(L"snd_ZeliskaFire3")->Play(1, 10.f, true);
		break;
	}

	// 속도등 스크립트 설정
	CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"HandCannonBullet")->Instantiate();
	// scale 설정
	pProj->Transform()->SetRelativeScale(Vec3(16.f, 16.f, 1.f) * 2.f);
	CProjectileScript* pScript = pProj->GetScript<CProjectileScript>();

	if (nullptr != pScript)
	{
		pScript->SetSpeed(400.f);
		pScript->SetDir(GetPlayerDir());
		pScript->SetDist(500.f);
		pScript->SetDamage(3);
		pScript->SetEndSound(CResMgr::GetInst()->FindRes<CSound>(L"snd_ZeliskaHitWall"));
	}

	SpawnGameObject(pProj, CalculateSpawnPos(50.f), L"PlayerProjectile");
	// Muzzle effect 생성
	CGameObject* pEffect = CResMgr::GetInst()->FindRes<CPrefab>(L"HandCannonMuzzle")->Instantiate();
	SpawnGameObject(pEffect, CalculateSpawnPos(30.f), L"PlayerProjectile");

	//// smoke 생성
	//CGameObject* pSmoke = CResMgr::GetInst()->FindRes<CPrefab>(L"ParticleEffect")->Instantiate();
	//CParticleSystem* pPart = pSmoke->ParticleSystem();
	//pPart->Load(L"\\particle\\handgun_smoke1.particle");
	//pSmoke->SetLifeSpan(1.f);

	//SpawnGameObject(pSmoke, CalculateSpawnPos(30.f), L"PlayerProjectile");

	//pSmoke = CResMgr::GetInst()->FindRes<CPrefab>(L"ParticleEffect")->Instantiate();
	//pPart = pSmoke->ParticleSystem();
	//pPart->Load(L"\\particle\\handgun_smoke3.particle");
	//pSmoke->SetLifeSpan(1.f);

	//SpawnGameObject(pSmoke, CalculateSpawnPos(30.f), L"PlayerProjectile");

	Recoil(0.1f, 500.f);
}

float CPlayerShoot::CalculateAngle()
{
	CPlayerScript* pPlayer = GetOwnerObj()->GetScript<CPlayerScript>();
	Vec3 vDir = pPlayer->GetFaceDir();
	vDir.Normalize();
	float fDir = 1.f;
	if (vDir.y < 0.f)
	{
		fDir = -1.f;
	}

	float fAngle = vDir.Dot(Vec3(1.0, 0.f, 0.f));
	fAngle = acosf(fAngle) * fDir;

	return fAngle;
}

Vec3 CPlayerShoot::CalculateSpawnPos(float _fOffset)
{
	CPlayerScript* pPlayer = GetOwnerObj()->GetScript<CPlayerScript>();
	Vec3 vDir = pPlayer->GetFaceDir();
	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	vPlayerPos += vDir * _fOffset;

	return vPlayerPos;
}

Vec3 CPlayerShoot::GetPlayerDir()
{
	CPlayerScript* pPlayer = GetOwnerObj()->GetScript<CPlayerScript>();
	Vec3 vDir = pPlayer->GetFaceDir();
	vDir.Normalize();
	return vDir;
}

void CPlayerShoot::Recoil(float _fTime, float _fSpeed)
{
	CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();
	Vec3 vFaceDir = GetPlayerDir();
	vFaceDir.z = 0.f;
	vFaceDir *= -1.f;
	pCam->GetOwner()->GetScript<CCameraMoveScript>()->BeginShake(vFaceDir, _fTime, _fSpeed);
}

Vec3 CPlayerShoot::RayCast(Vec3 _vDir, const vector<CGameObject*>& _pVecObj)
{
	float fAdd = 3.f;
	_vDir.Normalize();

	Vec3 vPlayerPos = CalculateSpawnPos(40.f);
	Vec3 vPoint = vPlayerPos;
	float fMaxDist = 1000.f;

	CGameObject* pTarget = nullptr;
	while (true)
	{
		vPoint += _vDir * fAdd;

		for (size_t i = 0; i < _pVecObj.size(); i++)
		{
			CCollider2D* pCol = _pVecObj[i]->Collider2D();
			if (!pCol || !pCol->IsOnCollision())
				continue;

			if (COLLIDER2D_TYPE::CIRCLE == _pVecObj[i]->Collider2D()->GetCollider2DType())
			{
				Vec3 vObj = _pVecObj[i]->Transform()->GetRelativePos();
				float fRadius = pCol->GetRadius();
				vPoint.z = vObj.z;
				float fDist = Vector3::Distance(vPoint, vObj);

				if (fDist <= fRadius)
				{
					pTarget = _pVecObj[i];
					break;
				}
			}
			else if (COLLIDER2D_TYPE::RECT == _pVecObj[i]->Collider2D()->GetCollider2DType())
			{
				Matrix matBoxScale = pCol->GetColliderWorldScale();
				Matrix matBoxRTInv = XMMatrixInverse(nullptr, matBoxScale);
				matBoxRTInv *= pCol->GetColliderWorldMatrix();
				matBoxRTInv = XMMatrixInverse(nullptr, matBoxRTInv);

				Vec3 vBoxCoord = XMVector3TransformCoord(vPoint, matBoxRTInv);

				Vec3 arrLocal[2] =
				{
					Vec3(-0.5f, 0.5f, 0.f),
					Vec3(0.5f, -0.5f, 0.f)
				};
				for (int i = 0; i < 2; i++)
					arrLocal[i] = XMVector3TransformCoord(arrLocal[i], matBoxScale);

				if ((arrLocal[0].x <= vBoxCoord.x) && (arrLocal[1].x >= vBoxCoord.x) &&
					(arrLocal[0].y >= vBoxCoord.y) && (arrLocal[1].y <= vBoxCoord.y))
				{
					pTarget = _pVecObj[i];
					break;
				}
			}
		}

		if (pTarget)
			break;

		vPoint.z = vPlayerPos.z;
		if (fMaxDist <= Vector3::Distance(vPoint, vPlayerPos))
			break;
	}

	return vPoint;
}

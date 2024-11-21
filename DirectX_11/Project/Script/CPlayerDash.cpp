#include "pch.h"
#include "CPlayerDash.h"

#include "CPlayerScript.h"

#include <Engine\CAnim2D.h>
#include "CPlayerTrailScript.h"
#include "CAttackScript.h"

CPlayerDash::CPlayerDash()	:
	m_fDashSpeed(800.f),
	m_fWaitTime(0.15f),
	m_fAccWaitTime(0.f),
	m_eFaceDir(EFACE_DIR::SOUTH),
	m_bDashEnd(false),
	m_pCurAnim(nullptr),
	m_bPhantomSlash(false),
	m_pTrail(nullptr),
	m_fTime(0.05f),
	m_fAccTime(0.f),
	m_bBackFace(false)
{
}

CPlayerDash::~CPlayerDash()
{
}

void CPlayerDash::tick()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	Vec3 vCurPos = pPlayer->Transform()->GetRelativePos();

	if (!m_bDashEnd)
	{
		if (m_pCurAnim->GetCurFrameNotify() == L"End_Dash")
		{
			if (m_bPhantomSlash)
			{
				PhantomSlashAttack();
			}
			m_bDashEnd = true;
		}

		vCurPos += m_vecEnterDir * m_fDashSpeed * DT;
		pPlayer->Transform()->SetRelativePos(vCurPos);
		pPlayer->RoomCollisionCheck(2);

		if (m_fAccTime >= m_fTime)
		{
			m_fAccTime -= m_fTime;
			CGameObject* pTrail = CResMgr::GetInst()->FindRes<CPrefab>(L"AnimTrail")->Instantiate();

			pTrail->GetScript<CPlayerTrailScript>()->SetFrame(m_pCurAnim->GetCurFrame());
			pTrail->GetScript<CPlayerTrailScript>()->SetLifeTime(0.5f);
			pTrail->GetScript<CPlayerTrailScript>()->SetBackSize(m_pCurAnim->GetBackSize());
			pTrail->GetScript<CPlayerTrailScript>()->SetTex(m_pCurAnim->GetAtlasTex());
			pTrail->Transform()->SetRelativeScale(pPlayer->Transform()->GetRelativeScale());

			if (m_bBackFace)
			{
				pTrail->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
			}

			Vec3 vPos = pPlayer->Transform()->GetRelativePos();
			vPos.z = 500.f;
			SpawnGameObject(pTrail, vPos, L"PlayerProjectile");
		}

		m_fAccTime += DT;
	}
	else
	{
		if (m_fAccWaitTime <= m_fWaitTime)
		{
			m_fAccWaitTime += DT;
		}
		else
		{
			if (KEY_PRESSED(KEY::W) || KEY_PRESSED(KEY::S) || KEY_PRESSED(KEY::A) || KEY_PRESSED(KEY::D))
			{
				ChangeState(L"Move");
				return;
			}
			else if(KEY_PRESSED(KEY::LBTN))
			{
				ChangeState(L"Attack");
				return;
			}
		}

		if (m_pCurAnim->IsFinish())
		{
			ChangeState(L"Idle");
		}
	}
}

void CPlayerDash::Enter()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());

	if (!pPlayer->Dash())
	{
		ChangeState(L"Idle");
		return;
	}
	pPlayer->SetInvin(true);

	m_pTrail = CResMgr::GetInst()->FindRes<CPrefab>(L"Dash_Trail")->Instantiate();
	AddChildObject(GetOwnerObj(), m_pTrail);

	pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	m_bDashEnd = false;

	m_vDashStartPos = pPlayer->Transform()->GetRelativePos();

	if (m_bPhantomSlash)
	{
		PhantomSlash();
		CResMgr::GetInst()->FindRes<CSound>(L"snd_ScytheThrow")->Play(1, 10.f, true);
	}
	else
	{
		SetDashDir();
		CResMgr::GetInst()->FindRes<CSound>(L"snd_Dash")->Play(1, 10.f, true);
	}

	pPlayer->SetSafePos(pPlayer->Transform()->GetRelativePos());
}

void CPlayerDash::Exit()
{
	m_fAccWaitTime = 0.f;
	m_bPhantomSlash = false;
	m_fAccTime = 0.f;
	m_bBackFace = false;

	if (IsValid(m_pTrail))
	{
		DestoryObject(m_pTrail);
	}

	m_pTrail = nullptr;

	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	pPlayer->SetInvin(false);

	CPlayerIdle* pIdle = (CPlayerIdle*)pPlayer->FindState(L"Idle");
	pIdle->SetFaceDir(m_eFaceDir);

	// 만약 낭떠러지에서 종료 됐으면 fall 시켜야함
	if (1 == pPlayer->RoomCollisionCheckType())
		ChangeState(L"Fall");
}

void CPlayerDash::SetDashDir()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	m_vecEnterDir = pPlayer->GetFaceDir();
	m_vecEnterDir.Normalize();
	float fCos = m_vecEnterDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// 오른쪽 보기
	if (45.f >= fAngle)
	{
		GetOwnerAI()->Transform()->SetRelativeScale(Vec3(43.f, 29.f, 1.f) * 2.f);
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_chardashside");
		m_pCurAnim->Reset();
		pPlayer->Animator2D()->Play(L"spr_chardashside", false);
		m_eFaceDir = EFACE_DIR::EAST;
	}
	else if (135.f >= fAngle)
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		if (0 <= m_vecEnterDir.y)
		{
			GetOwnerAI()->Transform()->SetRelativeScale(Vec3(23.f, 44.f, 1.f) * 2.f);
			m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_chardashback");
			m_pCurAnim->Reset();
			pPlayer->Animator2D()->Play(L"spr_chardashback", false);
			m_eFaceDir = EFACE_DIR::NORTH;
		}
		else
		{
			GetOwnerAI()->Transform()->SetRelativeScale(Vec3(20.f, 38.f, 1.f) * 2.f);
			m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_chardashfront");
			m_pCurAnim->Reset();
			pPlayer->Animator2D()->Play(L"spr_chardashfront", false);
			m_eFaceDir = EFACE_DIR::SOUTH;
		}
	}
	// 왼쪽 보기
	else
	{
		GetOwnerAI()->Transform()->SetRelativeScale(Vec3(43.f, 29.f, 1.f) * 2.f);
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_chardashside");
		m_pCurAnim->Reset();
		pPlayer->Animator2D()->Play(L"spr_chardashside", false);
		m_eFaceDir = EFACE_DIR::WEST;
		m_bBackFace = true;
	}
}

void CPlayerDash::PhantomSlash()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	m_vecEnterDir = pPlayer->GetFaceDir();
	m_vecEnterDir.Normalize();
	float fCos = m_vecEnterDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	GetOwnerAI()->Transform()->SetRelativeScale(Vec3(33.f, 38.f, 1.f) * 2.f);
	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharPhantomSlashE");
		m_pCurAnim->Reset();
		pPlayer->Animator2D()->Play(L"spr_CharPhantomSlashE", false);
		m_eFaceDir = EFACE_DIR::EAST;
	}
	// 왼쪽 보기
	else
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharPhantomSlashE");
		m_pCurAnim->Reset();
		pPlayer->Animator2D()->Play(L"spr_CharPhantomSlashE", false);
		m_eFaceDir = EFACE_DIR::WEST;
	}
}

void CPlayerDash::PhantomSlashAttack()
{
	Vec3 vDashEndPos = GetOwnerAI()->Transform()->GetRelativePos();

	float fDist = Vector3::Distance(m_vDashStartPos, vDashEndPos);

	CGameObject* pAttack = CResMgr::GetInst()->FindRes<CPrefab>(L"Attack")->Instantiate();
	pAttack->Transform()->SetRelativeScale(Vec3(fDist, 30.f, 1.f));
	pAttack->GetScript<CAttackScript>()->SetDamage(2.f);

	Vec3 vDir = m_vecEnterDir;
	vDir.Normalize();
	float fDir = 1.f;
	if (vDir.y < 0.f)
	{
		fDir = -1.f;
	}

	float fAngle = vDir.Dot(Vec3(1.0, 0.f, 0.f));
	fAngle = acosf(fAngle) * fDir;
	pAttack->Transform()->SetRelativeRot(Vec3(0.f, 0.f, fAngle));

	Vec3 vPos = (vDashEndPos + m_vDashStartPos) / 2.f;

	pAttack->SetLifeSpan(0.1f);
	SpawnGameObject(pAttack, vPos, L"PlayerAttack");
}

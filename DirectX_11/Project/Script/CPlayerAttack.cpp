#include "pch.h"
#include "CPlayerAttack.h"

#include "CPlayerScript.h"
#include <Engine\CAnim2D.h>
#include "CPlayerDash.h"
#include "CAttackScript.h"

CPlayerAttack::CPlayerAttack()	:
	m_eFaceDir(EFACE_DIR::SOUTH),
	m_pCurAnim(nullptr),
	m_bComboOn(false),
	m_bAttackEnd(false),
	m_bMoveEnd(false),
	m_fMoveSpeed(300.f),
	m_bChargeAttack(false),
	m_bWaitEnd(false),
	m_fAccTime(0.f),
	m_fWaitTime(0.3f),
	m_fAttackLife(1.f),
	m_bAttack1(false),
	m_bAttack2(false),
	m_bAttack3(false),
	m_iAttackCount(0)
{
}

CPlayerAttack::~CPlayerAttack()
{
}

void CPlayerAttack::tick()
{
	if (!m_bWaitEnd)
	{
		DetectHold();
	}
	else
	{
		if (!m_bChargeAttack)
			BasicAttack();
		else
			ChargeAttack();

		if (m_pCurAnim->GetCurFrameNotify() == L"Attack")
		{
			if (m_iAttackCount == 0 && !m_bAttack1)
			{
				Attack();
				m_bAttack1 = true;
			}
			else if (m_iAttackCount == 1 && !m_bAttack2)
			{
				Attack();
				m_bAttack2 = true;
			}
			else if (m_iAttackCount == 2 && !m_bAttack3)
			{
				Attack();
				m_bAttack3 = true;
			}
		}
	}
}

void CPlayerAttack::Enter()
{
	m_bAttackEnd = false;
	m_bComboOn = false;
	m_bMoveEnd = false;
	m_fAccTime = 0.f;
	m_bChargeAttack = false;
	m_bWaitEnd = false;
	m_bAttack1 = false;
	m_bAttack2 = false;
	m_bAttack3 = false;
	m_iAttackCount = 0;

	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
}

void CPlayerAttack::Exit()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));

	CPlayerIdle* pIdle = (CPlayerIdle*)pPlayer->FindState(L"Idle");
	pIdle->SetFaceDir(m_eFaceDir);
}

void CPlayerAttack::SetAttackDir()
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
		GetOwnerAI()->Transform()->SetRelativeScale(Vec3(54.f, 39.f, 1.f) * 2.f);
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharBasicSwordE");
		m_pCurAnim->Reset();
		pPlayer->Animator2D()->Play(L"spr_CharBasicSwordE", false);
		m_eFaceDir = EFACE_DIR::EAST;

		// 공격 충돌체 설정
		m_vAttackSize = Vec3(80.f, 80.f, 1.f);
		m_vAttackOffset = Vec3(10.f, 0.f, 0.f);
	}
	else if (135.f >= fAngle)
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		if (0 <= m_vecEnterDir.y)
		{
			GetOwnerAI()->Transform()->SetRelativeScale(Vec3(54.f, 49.f, 1.f) * 2.f);
			m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharBasicSwordN");
			m_pCurAnim->Reset();
			pPlayer->Animator2D()->Play(L"spr_CharBasicSwordN", false);
			m_eFaceDir = EFACE_DIR::NORTH;

			// 공격 충돌체 설정
			m_vAttackSize = Vec3(80.f, 80.f, 1.f);
			m_vAttackOffset = Vec3(0.f, 10, 0.f);
		}
		else
		{
			GetOwnerAI()->Transform()->SetRelativeScale(Vec3(52.f, 55.f, 1.f) * 2.f);
			m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharBasicSwordS");
			m_pCurAnim->Reset();
			pPlayer->Animator2D()->Play(L"spr_CharBasicSwordS", false);
			m_eFaceDir = EFACE_DIR::SOUTH;

			// 공격 충돌체 설정
			m_vAttackSize = Vec3(80.f, 80.f, 1.f);
			m_vAttackOffset = Vec3(0.f, -10.f, 0.f);
		}
	}
	// 왼쪽 보기
	else
	{
		GetOwnerAI()->Transform()->SetRelativeScale(Vec3(54.f, 39.f, 1.f) * 2.f);
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharBasicSwordE");
		m_pCurAnim->Reset();
		pPlayer->Animator2D()->Play(L"spr_CharBasicSwordE", false);
		m_eFaceDir = EFACE_DIR::WEST;

		// 공격 충돌체 설정
		m_vAttackSize = Vec3(80.f, 80.f, 1.f);
		m_vAttackOffset = Vec3(-10.f, 0.f, 0.f);
	}
	m_fAttackLife = 0.1f;
}

void CPlayerAttack::SetChargeAttackDir()
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
		GetOwnerAI()->Transform()->SetRelativeScale(Vec3(91.f, 71.f, 1.f) * 2.f);
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharHeavySwordE");
		m_pCurAnim->Reset();
		pPlayer->Animator2D()->Play(L"spr_CharHeavySwordE", false);
		m_eFaceDir = EFACE_DIR::EAST;

		// 공격 충돌체 설정
		m_vAttackSize = Vec3(80.f, 120.f, 1.f);
		m_vAttackOffset = Vec3(50.f, 0.f, 0.f);
	}
	else if (135.f >= fAngle)
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		if (0 <= m_vecEnterDir.y)
		{
			GetOwnerAI()->Transform()->SetRelativeScale(Vec3(61.f, 71.f, 1.f) * 2.f);
			m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharHeavySwordN");
			m_pCurAnim->Reset();
			pPlayer->Animator2D()->Play(L"spr_CharHeavySwordN", false);
			m_eFaceDir = EFACE_DIR::NORTH;

			// 공격 충돌체 설정
			m_vAttackSize = Vec3(120.f, 60.f, 1.f);
			m_vAttackOffset = Vec3(0.f, 50.f, 0.f);
		}
		else
		{
			GetOwnerAI()->Transform()->SetRelativeScale(Vec3(62.f, 78.f, 1.f) * 2.f);
			m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharHeavySwordS");
			m_pCurAnim->Reset();
			pPlayer->Animator2D()->Play(L"spr_CharHeavySwordS", false);
			m_eFaceDir = EFACE_DIR::SOUTH;

			// 공격 충돌체 설정
			m_vAttackSize = Vec3(120.f, 60.f, 1.f);
			m_vAttackOffset = Vec3(0.f, -50.f, 0.f);
		}
	}
	// 왼쪽 보기
	else
	{
		GetOwnerAI()->Transform()->SetRelativeScale(Vec3(91.f, 71.f, 1.f) * 2.f);
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharHeavySwordE");
		m_pCurAnim->Reset();
		pPlayer->Animator2D()->Play(L"spr_CharHeavySwordE", false);
		m_eFaceDir = EFACE_DIR::WEST;

		// 공격 충돌체 설정
		m_vAttackSize = Vec3(80.f, 120.f, 1.f);
		m_vAttackOffset = Vec3(-50.f, 0.f, 0.f);
	}
	m_fAttackLife = 0.15f;
}

void CPlayerAttack::BasicAttack()
{
	if (KEY_TAP(KEY::LBTN))
	{
		m_bComboOn = true;
	}
	
	// 이동 관련 로직
	if (m_pCurAnim->GetCurFrameNotify() == L"End_Move")
	{
		m_bMoveEnd = true;
	}

	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	Vec3 vCurPos = pPlayer->Transform()->GetRelativePos();

	if (!m_bMoveEnd)
	{
		vCurPos += m_vecEnterDir * m_fMoveSpeed * DT;
		pPlayer->Transform()->SetRelativePos(vCurPos);
		pPlayer->RoomCollisionCheck();
	}

	// 대쉬 공격 관련 로직
	if (KEY_TAP(KEY::SPACE) && m_pCurAnim->GetCurFrameIdx() < 2)
	{
		((CPlayerDash*)pPlayer->FindState(L"Dash"))->UsePhantomSlash();
		ChangeState(L"Dash");
		return;
	}

	// 콤보 관련 로직
	if (m_iAttackCount == 0 && m_pCurAnim->GetCurFrameNotify() == L"Start_Combo2")
	{
		if (m_bComboOn)
		{
			m_bComboOn = false;
			m_bMoveEnd = false;
			SetAttackDir();
			m_pCurAnim->SetCurFrameByNotify(L"Start_Combo2");
			m_iAttackCount++;
		}
		else
			m_bAttackEnd = true;
	}
	else if (m_iAttackCount == 1 && m_pCurAnim->GetCurFrameNotify() == L"Start_Combo3")
	{
		if (m_bComboOn)
		{
			m_bComboOn = false;
			m_bMoveEnd = false;
			SetAttackDir();
			m_pCurAnim->SetCurFrameByNotify(L"Start_Combo3");
			m_iAttackCount++;
		}
		else
			m_bAttackEnd = true;
	}
	else if (m_pCurAnim->IsFinish())
		m_bAttackEnd = true;

	if (m_bAttackEnd)
		ChangeState(L"Idle");
}

void CPlayerAttack::ChargeAttack()
{
	if (m_pCurAnim->IsFinish())
	{
		ChangeState(L"Idle");
	}
}

void CPlayerAttack::DetectHold()
{
	if (m_fAccTime < m_fWaitTime)
	{
		if (KEY_PRESSED(KEY::LBTN))
		{
			m_fAccTime += DT;
		}
		else
		{
			m_bWaitEnd = true;
			m_bChargeAttack = false;
			SetAttackDir();
		}
	}
	else
	{
		m_bWaitEnd = true;
		m_bChargeAttack = true;
		SetChargeAttackDir();
		CResMgr::GetInst()->FindRes<CSound>(L"snd_SwordCharge")->Play(1, 10.f, true);
	}
}

void CPlayerAttack::Attack()
{
	CGameObject* pAttack = CResMgr::GetInst()->FindRes<CPrefab>(L"Attack")->Instantiate();
	pAttack->Transform()->SetRelativeScale(m_vAttackSize);
	Vec3 vPos = m_vAttackOffset + GetOwnerAI()->Transform()->GetRelativePos();
	pAttack->SetLifeSpan(m_fAttackLife);

	if (m_bChargeAttack)
	{
		CResMgr::GetInst()->FindRes<CSound>(L"snd_SwordHeavy_v5")->Play(1, 10.f, true);;
		pAttack->GetScript<CAttackScript>()->SetDamage(5.f);
	}
	else
	{
		int iRand = GetRandInt(1);

		switch (iRand)
		{
		case 0:
			CResMgr::GetInst()->FindRes<CSound>(L"s_sword1")->Play(1, 10.f, true);
			break;
		case 1:
			CResMgr::GetInst()->FindRes<CSound>(L"s_sword1")->Play(1, 10.f, true);
			break;
		}
		pAttack->GetScript<CAttackScript>()->SetDamage(1.f);
	}

	pAttack->GetScript<CAttackScript>()->AddDynamicRegain(10.f, GetOwnerAI(), (SCRIPT_DELEGATE_1)&CPlayerScript::AmmoRegain);
	SpawnGameObject(pAttack, vPos, L"PlayerAttack");
}

#include "pch.h"
#include "CGhostBeamBirdAttack.h"

#include "CGhostBeamBirdScript.h"
#include <Engine\CAnim2D.h>
#include "CProjectileScript.h"

CGhostBeamBirdAttack::CGhostBeamBirdAttack()	:
	m_pCurAnim(nullptr),
	m_bIsSlam(true),
	m_pChargeEffect(nullptr),
	m_bEnd(false),
	m_bShoot(false)
{
}

CGhostBeamBirdAttack::~CGhostBeamBirdAttack()
{
}

void CGhostBeamBirdAttack::tick()
{
	if (m_bIsSlam)
	{
		Slam();
	}
	else
	{
		Invoke();
	}

	if (m_pCurAnim->IsFinish())
	{
		ChangeState(L"Idle");

		if(IsValid(m_pChargeEffect))
			DestoryObject(m_pChargeEffect);
	}
}

void CGhostBeamBirdAttack::Enter()
{
	CGhostBeamBirdScript* pMon = ((CGhostBeamBirdScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// ���� Ÿ�� ���ϱ�
	wstring strAnim;
	int iRand = GetRandInt(1);
	if (iRand)
	{
		m_bIsSlam = true;
		strAnim = L"spr_GhostBeamBirdSlam";
	}
	else
	{
		m_bIsSlam = false;
		strAnim = L"spr_GhostBeamBirdInvoke";
		m_pChargeEffect = CResMgr::GetInst()->FindRes<CPrefab>(L"Summon_fx")->Instantiate();
		Vec3 vPos = pMon->Transform()->GetRelativePos();
		vPos.z -= 1.f;
		SpawnGameObject(m_pChargeEffect, vPos, L"MonsterProjectile");
	}

	// ������ ����
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(strAnim);
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(strAnim, false);
	}
	// ���� ����
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(strAnim);
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(strAnim, false);
	}
}

void CGhostBeamBirdAttack::Exit()
{
	m_pCurAnim = nullptr;
	m_bIsSlam = true;
	m_pChargeEffect = nullptr;
	m_bEnd = false;
	m_bShoot = false;
}

void CGhostBeamBirdAttack::Slam()
{
	if (!m_bEnd && m_pCurAnim->GetCurFrameNotify() == L"Slam")
	{
		CGhostBeamBirdScript* pMon = ((CGhostBeamBirdScript*)GetOwnerAI());

		CGameObject* pEffect = CResMgr::GetInst()->FindRes<CPrefab>(L"SlamSquare")->Instantiate();
		SpawnGameObject(pEffect, pMon->Transform()->GetRelativePos(), L"Effect");
		
		CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
		SpawnGameObject(pProj, pMon->GetTarget()->Transform()->GetRelativePos(), L"MonsterAttack");

		m_bEnd = true;

		CResMgr::GetInst()->FindRes<CSound>(L"snd_JerkPopeSlamStaff")->Play(1, 10.f, true);
	}
}

void CGhostBeamBirdAttack::Invoke()
{
	CGhostBeamBirdScript* pMon = ((CGhostBeamBirdScript*)GetOwnerAI());

	// �׻� �÷��̾� ������ �ٶ󺸰��ϱ�
	Vec3 vMonPos = pMon->Transform()->GetRelativePos();
	vMonPos.z = 0.f;
	Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
	vPlayerPos.z = 0.f;

	Vec3 vDir = vPlayerPos - vMonPos;
	vDir.Normalize();

	pMon->SetFaceDir(vDir);

	vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// ������ ����
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	}
	// ���� ����
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
	}

	if (m_pCurAnim->GetCurFrameNotify() == L"Shoot")
	{
		if (false == m_bShoot)
		{
			m_bShoot = true;
			CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"GhostBeam")->Instantiate();
			// �ӵ��� ��ũ��Ʈ ����
			CProjectileScript* pScript = pProj->GetScript<CProjectileScript>();
			if (nullptr != pScript)
			{
				pScript->SetSpeed(200.f);
				pScript->SetDir(pMon->GetFaceDir());
				pScript->SetDist(500.f);
				pScript->SetDamage(1.f);
				pScript->SetHomming(true);
				pScript->SetHommingRatio(0.2f);
				pScript->SetTarget(pMon->GetTarget());
			}

			// ������ �߰�
			SpawnGameObject(pProj, pMon->Transform()->GetRelativePos(), L"MonsterProjectile");
		}
	}
	else
		m_bShoot = false;
}
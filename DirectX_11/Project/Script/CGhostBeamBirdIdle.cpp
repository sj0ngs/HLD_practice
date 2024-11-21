#include "pch.h"
#include "CGhostBeamBirdIdle.h"

#include "CGhostBeamBirdScript.h"
#include <Engine\CAnim2D.h>

CGhostBeamBirdIdle::CGhostBeamBirdIdle()	:
	m_fDetectRadius(150.f),
	m_pCurAnim(nullptr),
	m_bDetected(false)
{
}

CGhostBeamBirdIdle::~CGhostBeamBirdIdle()
{
}

void CGhostBeamBirdIdle::tick()
{
	CGhostBeamBirdScript* pMon = ((CGhostBeamBirdScript*)GetOwnerAI());

	if (m_pCurAnim->IsFinish())
	{
		// �÷��̾� �߰���
		if (!m_bDetected)
		{
			// ����� ���� �������� �̵�
			SetRandomFaceDir();
			ChangeState(L"Walk");
		}
		// �÷��̾� �߰���
		else
		{
			// ����� ����
			ChangeState(L"Attack");

			// �Ǵ� �÷��̿��� �־����� �������� �̵�
		}
	}

	if (!m_bDetected)
	{
		Vec3 vMonPos = pMon->Transform()->GetRelativePos();
		vMonPos.z = 0.f;
		Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
		vPlayerPos.z = 0.f;

		float fDist = Vec3::Distance(vMonPos, vPlayerPos);

		if (fDist <= m_fDetectRadius)
		{
			m_bDetected = true;
		}
	}
	else
	{
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
	}
}

void CGhostBeamBirdIdle::Enter()
{
	GetOwnerAI()->Transform()->SetRelativeScale(Vec3(76.f, 70.f, 1.f) * 2.f);
	SetStandAnim();
}

void CGhostBeamBirdIdle::Exit()
{
	m_pCurAnim = nullptr;
}

void CGhostBeamBirdIdle::SetRandomFaceDir()
{
	CGhostBeamBirdScript* pMon = ((CGhostBeamBirdScript*)GetOwnerAI());

	int iRand = GetRandInt(360);
	float fAngle = XMConvertToRadians((float)iRand);

	Matrix matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(fAngle);
	Vec4 vDir = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), matRot);

	pMon->SetFaceDir(vDir);
}

void CGhostBeamBirdIdle::SetStandAnim()
{
	CGhostBeamBirdScript* pMon = ((CGhostBeamBirdScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// ������ ����
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_GhostBeamBirdIdle");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_GhostBeamBirdIdle", false);
	}
	// ���� ����
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_GhostBeamBirdIdle");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_GhostBeamBirdIdle", false);
	}
}

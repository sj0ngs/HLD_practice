#include "pch.h"
#include "CJerkPopeTeleport.h"

#include <Engine\CAnim2D.h>

#include "CJerkPopeScript.h"

CJerkPopeTeleport::CJerkPopeTeleport()	:
	m_pCurAnim(nullptr),
	m_bTeleport(false),
	m_iBtwFrm(6),
	m_vMoveDir{},
	m_fSpeed(0.f),
	m_fTimePerFrame(0.05f)
{
}

CJerkPopeTeleport::~CJerkPopeTeleport()
{
}

void CJerkPopeTeleport::tick()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());

	if (!m_bTeleport && m_pCurAnim->GetCurFrameNotify() == L"Start")
	{
		CResMgr::GetInst()->FindRes<CSound>(L"snd_JerkPopeTeleport")->Play(1, 10.f, true);
		m_bTeleport = true;
		pMon->SetInvincibility(true);
	}
	else if (m_bTeleport && m_pCurAnim->GetCurFrameNotify() == L"End")
	{
		m_bTeleport = false;
		pMon->SetInvincibility(false);
	}

	if (m_bTeleport)
	{
		Move();
	}

	// 항상 플레이어 바라보기
	Vec3 vDir = pMon->GetFaceDir();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	}
	// 왼쪽 보기
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
	}

	if (m_pCurAnim->IsFinish())
	{
		if (pMon->IsPhase2())
		{
			pMon->SetAttackCool(5.f);
			ChangeState(L"Attack");
			return;
		}

		ChangeState(L"Idle");
	}
}

void CJerkPopeTeleport::Enter()
{
	SetTeleportPoint();

	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_JerkPopeTeleport");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_JerkPopeTeleport", false);
	}
	// 왼쪽 보기
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_JerkPopeTeleport");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_JerkPopeTeleport", false);
	}

}

void CJerkPopeTeleport::Exit()
{
	m_pCurAnim = nullptr;
	m_bTeleport = false;
	m_vMoveDir = Vec3(0.f, 0.f, 0.f);
	m_fSpeed = 0.f;
}

void CJerkPopeTeleport::SetTeleportPoint()
{
	// 사용중이 아닌 네 좌표중 하나를 지정
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());
	Vec3 vMovePos = Vec3(0.f, 0.f, 0.f);

	// 중앙으로 이동
	if (pMon->GetMoveCenter())
	{
		vMovePos = Vec3(0.f, -178.f, 600.f);
	}
	// 네점중 하나로 이동
	else
	{
		do
		{
			int iRand = GetRandInt(3);
			switch (iRand)
			{
			case 0:
				if (!pMon->m_matTile[1][1].bUse)
				{
					pMon->ClearMatrix();
					vMovePos = pMon->m_matTile[1][1].vPos;
					pMon->m_matTile[1][1].bUse = true;
				}
				break;
			case 1:
				if (!pMon->m_matTile[1][pMon->GetGridCol() - 2].bUse)
				{
					pMon->ClearMatrix();
					vMovePos = pMon->m_matTile[1][pMon->GetGridCol() - 2].vPos;
					pMon->m_matTile[1][pMon->GetGridCol() - 2].bUse = true;
				}
				break;
			case 2:
				if (!pMon->m_matTile[pMon->GetGridRow() - 2][1].bUse)
				{
					pMon->ClearMatrix();
					vMovePos = pMon->m_matTile[pMon->GetGridRow() - 2][1].vPos;
					pMon->m_matTile[pMon->GetGridRow() - 2][1].bUse = true;
				}
				break;
			case 3:
				if (!pMon->m_matTile[pMon->GetGridRow() - 2][pMon->GetGridCol() - 2].bUse)
				{
					pMon->ClearMatrix();
					vMovePos = pMon->m_matTile[pMon->GetGridRow() - 2][pMon->GetGridCol() - 2].vPos;
					pMon->m_matTile[pMon->GetGridRow() - 2][pMon->GetGridCol() - 2].bUse = true;
				}
				break;
			}
		} while (vMovePos == Vec3(0.f, 0.f, 0.f));
	}

	Vec3 vPos = pMon->Transform()->GetRelativePos();
	vMovePos.z = vPos.z;

	float fDist = Vec3::Distance(vPos, vMovePos);
	m_fSpeed = fDist / (m_iBtwFrm * m_fTimePerFrame);

	m_vMoveDir = vMovePos - vPos;
	m_vMoveDir.Normalize();
}

void CJerkPopeTeleport::Move()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());
	Vec3 vPos = pMon->Transform()->GetRelativePos();
	vPos += m_vMoveDir * m_fSpeed * DT;
	pMon->Transform()->SetRelativePos(vPos);
}

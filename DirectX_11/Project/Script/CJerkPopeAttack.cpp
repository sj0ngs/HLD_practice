#include "pch.h"
#include "CJerkPopeAttack.h"

#include "CJerkPopeScript.h"

#include <Engine\CAnim2D.h>
#include <Engine\CRenderMgr.h>

#include "CCameraMoveScript.h"

CJerkPopeAttack::CJerkPopeAttack()	:
	m_pCurAnim(nullptr),
	m_iTileType(0),
	m_iAttackType(0),
	m_bAttackAnimEnd(false),
	m_bAttackEnd(false),
	m_iTraceAttack(0),
	m_fAccTime(0.f),
	m_fTraceTime(0.3f),
	m_bPrevRow(0),
	m_bPrevCol(0),
	m_bCast(false)
{
}

CJerkPopeAttack::~CJerkPopeAttack()
{
}

void CJerkPopeAttack::tick()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());

	if (m_bCast)
	{
		// 1������
		if (!pMon->IsPhase2())
		{
			if (m_iAttackType == 0)
			{
				m_fAccTime += DT;
				if (m_fAccTime >= m_fTraceTime)
				{
					m_fAccTime -= m_fTraceTime;
					TraceAttack();
				}
			}
			else if (m_iAttackType == 1)
			{
				TileAttackP1();
			}
		}
		// 2������
		else
		{
			m_fAccTime += DT;
			if (m_fAccTime >= m_fTraceTime)
			{
				m_fAccTime -= m_fTraceTime;
				TileAttackP2();
			}
		}
	}

	if (!m_bCast && m_pCurAnim->GetCurFrameNotify() == L"Cast")
	{
		m_bCast = true;
	}

	if (!m_bAttackAnimEnd && m_pCurAnim->IsFinish())
	{
		m_bAttackAnimEnd = true;
		pMon->Animator2D()->Play(L"spr_JerkPopeIdle", true);
	}

	if (m_bAttackAnimEnd && m_bAttackEnd)
	{
		if (pMon->IsPhase2() && 2 == pMon->GetTileAttack())
		{
			ChangeState(L"Idle");
			return;
		}

		if (0 == m_iAttackType && 2 == pMon->GetTraceAttack())
		{
			pMon->SetMoveCenter(false);
			ChangeState(L"Teleport");
			return;
		}

		if (1 == m_iAttackType && 2 == pMon->GetTileAttack())
		{
			pMon->SetMoveCenter(true);
			ChangeState(L"Teleport");
			return;
		}
		
		ChangeState(L"Idle");
	}
}

void CJerkPopeAttack::Enter()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());

	if (!pMon->IsPhase2())
	{
		// ���� ���� ����
		if (pMon->GetTraceAttack() < 2)
		{
			m_iAttackType = 0;
			pMon->SetTraceAttack(pMon->GetTraceAttack() + 1);
		}
		// Ÿ�� ���� ����
		else
		{
			m_iAttackType = 1;
			pMon->SetTileAttack(pMon->GetTileAttack() + 1);

			int iRand = GetRandInt(1);
			if (iRand)
			{
				m_iTileType = 0;
			}
			else
			{
				m_iTileType = 1;
			}
		}
	}
	else
	{
		m_iAttackType = 1;
		pMon->SetTileAttack(pMon->GetTileAttack() + 1);

		m_fAccTime = m_fTraceTime;
		int iRand = GetRandInt(1);
		if (iRand)
		{
			m_iTileType = 0;
		}
		else
		{
			m_iTileType = 1;
		}
	}


	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	// ������ ����
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_JerkPopeCast");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_JerkPopeCast", false);
	}
	// ���� ����
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_JerkPopeCast");
		m_pCurAnim->Reset();
		pMon->Animator2D()->Play(L"spr_JerkPopeCast", false);
	}
}

void CJerkPopeAttack::Exit()
{
	if (0 == m_iAttackType)
	{
		CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());
		pMon->ClearMatrix();
	}

	m_pCurAnim = nullptr;
	m_iTileType = 0;
	m_iAttackType = 0;
	m_bAttackAnimEnd = false;
	m_bAttackEnd = false;
	m_iTraceAttack = 0;
	m_fAccTime = 0.f;
}

void CJerkPopeAttack::TraceAttack()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());
	Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
	Vec3 vPos = pMon->Transform()->GetRelativePos();
	vPos.y -= 70.f;

	float fDist = 10000000000.f;
	JerkTile tJerk = {};

	// ó�� ������
	if (m_iTraceAttack == 0)
	{	
		for (int i = 0; i < pMon->GetGridRow(); i++)
		{
			for (int j = 0; j < pMon->GetGridCol(); j++)
			{
				float fTemp = Vec3::Distance(pMon->m_matTile[i][j].vPos, vPos);

				if (!pMon->m_matTile[i][j].bUse && fTemp < fDist)
				{
					fDist = fTemp;
					m_bPrevRow = i;
					m_bPrevCol = j;
				}
			}
		}

		CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
		SpawnGameObject(pProj, pMon->m_matTile[m_bPrevRow][m_bPrevCol].vPos, L"MonsterAttack");
		pMon->m_matTile[m_bPrevRow][m_bPrevCol].bUse = true;
	}
	// bUse �� ���������� �ȵǰ� ��ó�����ϸ� �÷��̾�� ���� ���������
	else
	{
		JerkTile arrTile[4] = {};
		bool arrUse[4] = { false, false, false, false };
		// �¿� ����
		if (0 < m_bPrevRow && pMon->GetGridRow() -1 > m_bPrevRow)
		{
			// ��
			if (!pMon->m_matTile[m_bPrevRow][m_bPrevCol - 1].bUse)
			{
				arrUse[0] = true;
				arrTile[0] = pMon->m_matTile[m_bPrevRow][m_bPrevCol - 1];
			}

			// ��
			if (!pMon->m_matTile[m_bPrevRow][m_bPrevCol + 1].bUse)
			{
				arrUse[1] = true;
				arrTile[1] = pMon->m_matTile[m_bPrevRow][m_bPrevCol + 1];
			}
		}

		// ���� ����
		if (0 < m_bPrevCol && pMon->GetGridCol() > m_bPrevCol)
		{
			// ��
			if (!pMon->m_matTile[m_bPrevRow - 1][m_bPrevCol].bUse)
			{
				arrUse[2] = true;
				arrTile[2] = pMon->m_matTile[m_bPrevRow - 1][m_bPrevCol];
			}

			// ��
			if (!pMon->m_matTile[m_bPrevRow + 1][m_bPrevCol].bUse)
			{
				arrUse[3] = true;
				arrTile[3] = pMon->m_matTile[m_bPrevRow + 1][m_bPrevCol];
			}
		}

		for (int i = 0; i < 4; i++)
		{	
			if (!arrUse[i])
				continue;

			float fTemp = Vec3::Distance(arrTile[i].vPos, vPlayerPos);

			if (fTemp < fDist)
			{
				fDist = fTemp;
				tJerk = arrTile[i];
			}
		}

		int addRow = 0;
		int addCol = 0;
		if (tJerk.vPos.x > pMon->m_matTile[m_bPrevRow][m_bPrevCol].vPos.x)
		{
			addCol = 1;
		}
		else if(tJerk.vPos.x < pMon->m_matTile[m_bPrevRow][m_bPrevCol].vPos.x)
		{
			addCol = -1;
		}

		if (tJerk.vPos.y > pMon->m_matTile[m_bPrevRow][m_bPrevCol].vPos.y)
		{
			addRow = -1;
		}
		else if(tJerk.vPos.y < pMon->m_matTile[m_bPrevRow][m_bPrevCol].vPos.y)
		{
			addRow = 1;
		}

		m_bPrevRow += addRow;
		m_bPrevCol += addCol;

		CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
		SpawnGameObject(pProj, pMon->m_matTile[m_bPrevRow][m_bPrevCol].vPos, L"MonsterAttack");
		pMon->m_matTile[m_bPrevRow][m_bPrevCol].bUse = true;
	}
	
	if (++m_iTraceAttack >= 10)
		m_bAttackEnd = true;

	CResMgr::GetInst()->FindRes<CSound>(L"snd_JerkPopeSlamStaff")->Play(1, 10.f, true);
	CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();
	pCam->GetOwner()->GetScript<CCameraMoveScript>()->BeginShake(Vec3(0.f, 1.f, 0.f), 0.1f, 100.f);
}

void CJerkPopeAttack::TileAttackP1()
{
	if (m_bAttackEnd)
		return;

	CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();
	pCam->GetOwner()->GetScript<CCameraMoveScript>()->BeginShake(Vec3(0.f, 1.f, 0.f), 0.1f, 100.f);

	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());
	Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();

	float fDist = 10000000000.f;

	int iRow = 0;
	int iCol = 0;
	for (int i = 0; i < pMon->GetGridRow(); i++)
	{
		for (int j = 0; j < pMon->GetGridCol(); j++)
		{
			float fTemp = Vec3::Distance(pMon->m_matTile[i][j].vPos, vPlayerPos);

			if (fTemp < fDist)
			{
				fDist = fTemp;
				iRow = i;
				iCol = j;
			}
		}
	}

	// �߽ɿ� ����
	CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
	SpawnGameObject(pProj, pMon->m_matTile[iRow][iCol].vPos, L"MonsterAttack");

	// x �� ����
	if (0 == m_iTileType)
	{
		// ��
		if (iRow > 0)
		{
			CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
			SpawnGameObject(pProj, pMon->m_matTile[iRow - 1][iCol].vPos, L"MonsterAttack");
		}
		// �Ʒ�
		if (iRow < pMon->GetGridRow() - 1)
		{
			CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
			SpawnGameObject(pProj, pMon->m_matTile[iRow + 1][iCol].vPos, L"MonsterAttack");
		}
		// ����
		if (iCol > 0)
		{
			CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
			SpawnGameObject(pProj, pMon->m_matTile[iRow][iCol - 1].vPos, L"MonsterAttack");
		}
		// ������
		if (iCol < pMon->GetGridCol() - 1)
		{
			CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
			SpawnGameObject(pProj, pMon->m_matTile[iRow][iCol + 1].vPos, L"MonsterAttack");
		}
	}
	// ���� ����
	else if (1 == m_iTileType)
	{
		// ���
		if (iRow > 0)
		{
			// �� ���
			if (iCol > 0)
			{
				CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
				SpawnGameObject(pProj, pMon->m_matTile[iRow - 1][iCol - 1].vPos, L"MonsterAttack");
			}
			// �� ���
			if (iCol < pMon->GetGridCol() - 1)
			{
				CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
				SpawnGameObject(pProj, pMon->m_matTile[iRow - 1][iCol + 1].vPos, L"MonsterAttack");
			}
		}
		// �ϴ�
		if (iRow < pMon->GetGridRow() - 1)
		{
			// �� �ϴ�
			if (iCol > 0)
			{
				CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
				SpawnGameObject(pProj, pMon->m_matTile[iRow + 1][iCol - 1].vPos, L"MonsterAttack");
			}
			// �� �ϴ�
			if (iCol < pMon->GetGridCol() - 1)
			{
				CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
				SpawnGameObject(pProj, pMon->m_matTile[iRow + 1][iCol + 1].vPos, L"MonsterAttack");
			}
		}
	}

	m_bAttackEnd = true;

	CResMgr::GetInst()->FindRes<CSound>(L"snd_JerkPopeSlamStaff")->Play(1, 10.f, true);
}

void CJerkPopeAttack::TileAttackP2()
{
	CJerkPopeScript* pMon = ((CJerkPopeScript*)GetOwnerAI());
	Vec3 vPos = pMon->GetTarget()->Transform()->GetRelativePos();

	int i = m_iTraceAttack++ * 2;

	switch (m_iTileType)
	{	
	case 0: // ���� ��
	{
		if (i >= pMon->GetGridCol())
		{
			m_bAttackEnd = true;
			return;
		}

		for (int j = 0; j < pMon->GetGridRow(); j++)
		{
			CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
			SpawnGameObject(pProj, pMon->m_matTile[j][i].vPos, L"MonsterAttack");
		}
	}
		break;
	case 1: // ���� ��
	{
		if (i >= pMon->GetGridRow())
		{
			m_bAttackEnd = true;
			return;
		}

		for (int j = 0; j < pMon->GetGridCol(); j++)
		{
			CGameObject* pProj = CResMgr::GetInst()->FindRes<CPrefab>(L"WarpHammer")->Instantiate();
			SpawnGameObject(pProj, pMon->m_matTile[i][j].vPos, L"MonsterAttack");
		}
	}
		break;
	}

	CResMgr::GetInst()->FindRes<CSound>(L"snd_JerkPopeSlamStaff")->Play(1, 10.f, true);
	CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();
	pCam->GetOwner()->GetScript<CCameraMoveScript>()->BeginShake(Vec3(0.f, 1.f, 0.f), 0.1f, 200.f);
}

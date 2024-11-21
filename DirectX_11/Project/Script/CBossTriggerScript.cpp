#include "pch.h"
#include "CBossTriggerScript.h"

#include <Engine\CRenderMgr.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>

#include "CCameraMoveScript.h"
#include "CJerkPopeScript.h"
#include "CRisingBlockScript.h"
#include "CSpectatorScript.h"
#include "CPlayerScript.h"

CBossTriggerScript::CBossTriggerScript() :
	CScript(SCRIPT_TYPE::BOSSTRIGGERSCRIPT),
	m_bPlayerEnter(false),
	m_bBossClear(false),
	m_pBoss(nullptr),
	m_pPlayer(nullptr),
	m_bIntroEnd(false),
	m_fAccTime(0.f),
	m_fIntroTime(3.f),
	m_fOutroTime(3.f),
	m_bOutroEnd(false)
{
	m_vLetterBoxCol = Vec4(0.f, 0.f, 0.f, 1.f);
}

CBossTriggerScript::~CBossTriggerScript()
{
}

void CBossTriggerScript::begin()
{
	Vec4 vCol = Vec4(0.f, 0.f, 0.f, 1.f);
	CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->BeginFade(0.5f, vCol, true);

	CGameObject* pJerkPope = CLevelMgr::GetInst()->GetCurLevel()->FindObjectByScript(SCRIPT_TYPE::JERKPOPESCRIPT);
	if (pJerkPope)
		m_pBoss = dynamic_cast<CJerkPopeScript*>(pJerkPope->GetScript(SCRIPT_TYPE::JERKPOPESCRIPT));

	CLevelMgr::GetInst()->GetCurLevel()->FindObjectsByScript(SCRIPT_TYPE::RISINGBLOCKSCRIPT, m_vecRisingBlocks);
	CLevelMgr::GetInst()->GetCurLevel()->FindObjectsByScript(SCRIPT_TYPE::SPECTATORSCRIPT, m_vecSpectators);

	CGameObject* pPlayer = CLevelMgr::GetInst()->GetCurLevel()->FindObjectByScript(SCRIPT_TYPE::PLAYERSCRIPT);
	if (pPlayer)
		m_pPlayer = dynamic_cast<CPlayerScript*>(pPlayer->GetScript(SCRIPT_TYPE::PLAYERSCRIPT));
}

void CBossTriggerScript::tick()
{
	if (false == m_bPlayerEnter)
		return;

	if (false == m_bIntroEnd)
	{
		m_fAccTime += DT;
		if (m_fAccTime >= m_fIntroTime)
		{
			m_bIntroEnd = true;
			m_fAccTime = 0.f;
			CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->BeginLetterBox(1.f, m_vLetterBoxCol, false, 0.1f);
			CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->SetTarget(nullptr);
		}
	}

	// 보스 죽으면 
	// 플레이어 승리 애니메이션 실행
	if (!m_bBossClear && m_pBoss->IsDead())
	{
		m_bBossClear = true;
		// 장애물들 삭제
		for (size_t i = 0; i < m_vecRisingBlocks.size(); i++)
		{
			CRisingBlockScript* pScript = dynamic_cast<CRisingBlockScript*>(m_vecRisingBlocks[i]->GetScript(SCRIPT_TYPE::RISINGBLOCKSCRIPT));
			if (nullptr == pScript)
				continue;
			
			pScript->DropBlock();
		}

		CResMgr::GetInst()->FindRes<CSound>(L"MUS_N_WR_X")->Stop();

		// 구경꾼 도망
		for (size_t i = 0; i < m_vecSpectators.size(); i++)
		{
			CSpectatorScript* pScript = dynamic_cast<CSpectatorScript*>(m_vecSpectators[i]->GetScript(SCRIPT_TYPE::SPECTATORSCRIPT));
			if (nullptr == pScript)
				continue;

			pScript->Runaway();
		}
		CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->BeginLetterBox(1.f, m_vLetterBoxCol, true, 0.1f);
	}

	if (m_bBossClear && !m_bOutroEnd)
	{
		m_fAccTime += DT;
		if (m_fAccTime >= m_fOutroTime)
		{
			m_bOutroEnd = true;
			m_fAccTime = 0.f;
			CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->BeginLetterBox(1.f, m_vLetterBoxCol, false, 0.1f);
			if (m_pPlayer)
				m_pPlayer->Victory();
		}
	}
}

void CBossTriggerScript::BeginOverlap(CCollider2D* _Other)
{
	if (true == m_bPlayerEnter)
		return;

	// 처음 부딪혔을때
	// 1. 장애물들 생성
	m_bPlayerEnter = true;
	CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->BeginLetterBox(1.f, m_vLetterBoxCol, true, 0.1f);
	if (nullptr != m_pBoss)
	{
		CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>()->SetTarget(m_pBoss->GetOwner());
		m_pBoss->Invoke();
	}

	for (size_t i = 0; i < m_vecRisingBlocks.size(); i++)
	{
		CRisingBlockScript* pScript = dynamic_cast<CRisingBlockScript*>(m_vecRisingBlocks[i]->GetScript(SCRIPT_TYPE::RISINGBLOCKSCRIPT));
		if (nullptr == pScript)
			continue;

		pScript->RiseBlock();
	}

	CResMgr::GetInst()->FindRes<CSound>(L"MUS_N_WR_X")->Play(0, 1.f, true);
}

void CBossTriggerScript::OnOverlap(CCollider2D* _Other)
{
}

void CBossTriggerScript::EndOverlap(CCollider2D* _Other)
{
}
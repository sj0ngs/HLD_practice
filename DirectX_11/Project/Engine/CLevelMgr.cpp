#include "pch.h"

#include "CDevice.h"
#include "CLevelMgr.h"
#include "CResMgr.h"
#include "CCollisionMgr.h"
#include "CRenderMgr.h"

#include "components.h"

#include "CMesh.h"
#include "CGraphicsShader.h"
#include "CMaterial.h"

#include "CGameObject.h"
#include "CLevel.h"
#include "CLayer.h"

#include "CSetColorShader.h"

#include "CStructuredBuffer.h"

#include "CEventMgr.h"

CLevelMgr::CLevelMgr()	:
	m_pCurLevel(nullptr),
	m_bKeepPlay(false)
{
}

CLevelMgr::~CLevelMgr()
{	
	Safe_Del_Map(m_mapLevel);
	
	if (m_pCurLevel)
	{
		delete m_pCurLevel;
		m_pCurLevel = nullptr;
	}
}

void CLevelMgr::init()
{
	m_pCurLevel = new CLevel;
	m_pCurLevel->ChangeState(LEVEL_STATE::STOP);
}

void CLevelMgr::tick()
{
	m_pCurLevel->Clear();

	if (LEVEL_STATE::PLAY == m_pCurLevel->GetState())
	{
		m_pCurLevel->tick();
	}
	m_pCurLevel->finaltick();

	if (m_bKeepPlay)
	{
		m_pCurLevel->ChangeState(LEVEL_STATE::PLAY);
		m_bKeepPlay = false;
		CRenderMgr::GetInst()->LevelChangePlaying();
	}
}

CGameObject* CLevelMgr::FindObjectByName(const wstring& _strName)
{
	return m_pCurLevel->FindObjectByName(_strName);
}

void CLevelMgr::FindObjectByName(const wstring& _strName, _Inout_ vector<CGameObject*>& _vec)
{
	m_pCurLevel->FindObjectByName(_strName, _vec);
}

void CLevelMgr::ChangeLevel(CLevel* _pNextLevel)
{
	bool bIsPlay = false;
	if (nullptr != m_pCurLevel)
	{
		if (m_pCurLevel->GetState() == LEVEL_STATE::PLAY)
		{
			m_bKeepPlay = true;
			m_pCurLevel->end();
		}

		delete m_pCurLevel;
	}

	m_pCurLevel = _pNextLevel;
}

int CLevelMgr::GetLayerIdx(const wstring& _strLayer)
{
	if (nullptr != m_pCurLevel)
		return m_pCurLevel->GetLayerIndex(_strLayer);
	
	return -1;
}

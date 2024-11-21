#include "pch.h"
#include "CEventMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"

#include "CResMgr.h"
#include "CRes.h"

#include "CRenderMgr.h"

#include "CGameObject.h"
#include "CScript.h"

#include "CAI.h"


CEventMgr::CEventMgr()	:
	m_bLevelChanged(false)
{

}

CEventMgr::~CEventMgr()
{

}

void CEventMgr::tick()
{
	m_bLevelChanged = false;

	GC_Clear();

	for (size_t i = 0; i < m_vecEvent.size(); i++)
	{
		switch (m_vecEvent[i].Type)
		{
			// wParam : GameObject, lParam : Layer Index
		case EVENT_TYPE::CREATE_OBJECT:
		{
			CGameObject* NewObject = (CGameObject*)m_vecEvent[i].wParam;
			int iLayerIdx = (int)m_vecEvent[i].lParam;

			CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
			pCurLevel->AddGameObject(NewObject, iLayerIdx, false);
			NewObject->InitAllScript();

			if (LEVEL_STATE::STOP != pCurLevel->GetState())
			{
				NewObject->begin();
			}

			m_bLevelChanged = true;
		}
		break;
		case EVENT_TYPE::DELETE_OBJECT:
		{
			CGameObject* DeleteObject = (CGameObject*)m_vecEvent[i].wParam;

			if (false == DeleteObject->m_bDead)
			{
				DeleteObject->m_bDead = true;
				m_vecGC.push_back(DeleteObject);
			}

			m_bLevelChanged = true;
		}
		break;
		case EVENT_TYPE::ADD_CHILD:
		{
			CGameObject* pDestObj = (CGameObject*)m_vecEvent[i].wParam;
			CGameObject* pSrcObj = (CGameObject*)m_vecEvent[i].lParam;

			if (nullptr == pDestObj)
			{
				if (pSrcObj->GetParent())
				{
					pSrcObj->DisconnectFromParent();
					pSrcObj->AddParentList();
				}
			}
			else
			{
				pDestObj->AddChild(pSrcObj);
			}

			m_bLevelChanged = true;
		}
		break;
		case EVENT_TYPE::INIT_SCRIPT:
		{
			if (nullptr == CLevelMgr::GetInst()->GetCurLevel())
				return;

			CScript* pScript = (CScript*)m_vecEvent[i].wParam;

			if (nullptr == pScript)
				continue;

			pScript->init();

			m_bLevelChanged = true;
		}
		break;
		case EVENT_TYPE::DELETE_RESOURCE:
		{
			// wParam : RES_TYPE, lParam : Resource Adress

			RES_TYPE type = (RES_TYPE)m_vecEvent[i].wParam;
			wstring strKey = ((CRes*)m_vecEvent[i].lParam)->GetKey();

			CResMgr::GetInst()->DeleteRes(type, strKey);
		}
		break;
		case EVENT_TYPE::LEVEL_CHANGE:
		{
			// wParam : Level Adress
			CLevel* pLevel = (CLevel*)m_vecEvent[i].wParam;

			CLevelMgr::GetInst()->ChangeLevel(pLevel);
			CRenderMgr::GetInst()->ClearCamera();
			m_bLevelChanged = true;

			CRenderMgr::GetInst()->ClearDebugShapeInfo();
		}
		break;
		case EVENT_TYPE::CHANGE_AI_STATE:
		{
			// wParam  : AI Address			
			//lParam : State Type
			AI* pAI = (AI*)m_vecEvent[i].wParam;
			const wchar_t* pName = (const wchar_t*)m_vecEvent[i].lParam;
			pAI->ChangeState(pName);
		}
		break;
		case EVENT_TYPE::LAYER_CHANGE:
		{
			// wParam : GameObject
			// lParam : Move Layer Idx
			CGameObject* pObj = (CGameObject*)m_vecEvent[i].wParam;
			int iLayer = (int)m_vecEvent[i].lParam;
			CLevelMgr::GetInst()->GetCurLevel()->MoveObjectLayer(pObj, iLayer);
		}
		break;
		}
	}

	m_vecEvent.clear();
}

void CEventMgr::GC_Clear()
{
	for (size_t i = 0; i < m_vecGC.size(); i++)
	{
		if (nullptr != m_vecGC[i])
		{
			if (m_vecGC[i]->GetParent())
			{
				m_vecGC[i]->DisconnectFromParent();
			}
			delete m_vecGC[i];
			m_bLevelChanged = true;
		}
	}

	m_vecGC.clear();
}
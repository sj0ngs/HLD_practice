#include "pch.h"
#include "CLevel.h"

#include "CLayer.h"
#include "CGameObject.h"

#include "CRenderMgr.h"
#include "CCollisionMgr.h"

#include "../Script/CScriptMgr.h"

CLevel::CLevel()	:
	m_arrLayer{},
	m_eState(LEVEL_STATE::STOP)
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i] = new CLayer;
		m_arrLayer[i]->m_iLayerIdx = i;
	}
}

CLevel::~CLevel()
{
	Safe_Del_Array(m_arrLayer);
}


void CLevel::begin()
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i]->begin();
	}
}

void CLevel::end()
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i]->end();
	}
}

void CLevel::tick()
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i]->tick();
	}
}

void CLevel::finaltick()
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i]->finaltick();
	}
}

CLayer* CLevel::FindLayerByName(const wstring& _strName)
{
	for (int i = 0; i < MAX_LAYER; i++)
	{
		if (m_arrLayer[i]->GetName() == _strName)
		{
			return m_arrLayer[i];
		}
	}
	return nullptr;
}

int CLevel::GetLayerIndex(const wstring& _strName)
{
	CLayer* pLayer = FindLayerByName(_strName);

	if (nullptr != pLayer)
		return pLayer->GetLayerIdx();
	
	return -1;
}

void CLevel::AddGameObject(CGameObject* _Object, int _iLayerIdx, bool _bMove)
{
	assert(0 <= _iLayerIdx && MAX_LAYER > _iLayerIdx);

	m_arrLayer[_iLayerIdx]->AddGameObject(_Object, _bMove);
}

void CLevel::AddGameObject(CGameObject* _Object, const wstring& _LayerName, bool _bMove)
{
	CLayer* pLayer = FindLayerByName(_LayerName);

	assert(pLayer);

	pLayer->AddGameObject(_Object, _bMove);
}

CGameObject* CLevel::FindObjectByName(const wstring& _strName)
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();
		for (size_t j = 0; j < vecObjects.size(); j++)
		{
			if (_strName == vecObjects[j]->GetName())
			{
				return vecObjects[j];
			}
		}
	}

	return nullptr;
}

void CLevel::FindObjectByName(const wstring& _strName, _Inout_ vector<CGameObject*>& _vec)
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();
		for (size_t i = 0; i < vecObjects.size(); i++)
		{
			if (_strName == vecObjects[i]->GetName())
			{
				_vec.push_back(vecObjects[i]);
			}
		}
	}
}

void CLevel::AddCollisionPair(UINT _iLeftLayer, UINT _iRightLayer)
{
	if (_iLeftLayer >= MAX_LAYER || _iRightLayer >= MAX_LAYER)
	{
		MessageBox(nullptr, L"최대 레이어 수치를 넘길수 없습니다.", L"레이어 충돌 설정 오류", MB_OK);
		return;
	}

	UINT iRow = _iLeftLayer;
	UINT iCol = _iRightLayer;

	if (iCol < iRow)
	{
		UINT iTemp = iRow;
		iRow = iCol;
		iCol = iTemp;
	}

	pair<UINT, UINT> pPair = make_pair(iRow, iCol);
	vector<pair<UINT, UINT>>::iterator iter = find(m_vecCollisionPair.begin(), m_vecCollisionPair.end(), pPair);

	if (iter != m_vecCollisionPair.end())
	{
		MessageBox(nullptr, L"이미 존재하는 충돌 설정입니다.", L"레이어 충돌 설정 오류", MB_OK);
		return;
	}

	m_vecCollisionPair.push_back(pPair);
	UpdateCollisionPair();
}

void CLevel::DeleteCollisionPair(size_t _iPairIdx)
{
	vector<pair<UINT, UINT>>::iterator iter = m_vecCollisionPair.erase(m_vecCollisionPair.begin() + _iPairIdx);
	
	UpdateCollisionPair();
}

void CLevel::MoveObjectLayer(CGameObject* _pObj, int _iLayer)
{
	if (_iLayer == _pObj->GetLayerIndex())
		return;

	m_arrLayer[_pObj->GetLayerIndex()]->RemoveFromParentList(_pObj);
	AddGameObject(_pObj, _iLayer, true);
}

CGameObject* CLevel::FindObjectByScript(UINT _iScriptType)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (vecObjects[j]->GetScript(_iScriptType))
			{
				return vecObjects[j];
			}
		}
	}

	return nullptr;
}

void CLevel::FindObjectsByScript(UINT _iScriptType, vector<CGameObject*>& _vecObj)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetParentObject();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (vecObjects[j]->GetScript(_iScriptType))
				_vecObj.push_back(vecObjects[j]);
		}
	}
}

void CLevel::UpdateCollisionPair()
{
	CCollisionMgr::GetInst()->Clear();

	for (const auto& pPair : m_vecCollisionPair)
	{
		CCollisionMgr::GetInst()->LayerCheck(pPair.first, pPair.second);
	}
}

void CLevel::ChangeState(LEVEL_STATE _eState)
{
	m_eState = _eState;

	if (LEVEL_STATE::PLAY == m_eState)
	{
		begin();
		CRenderMgr::GetInst()->SetRenderFunc(true);
	}
	else
	{
		CRenderMgr::GetInst()->SetRenderFunc(false);
	}
}


void CLevel::Clear()
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i]->Clear();
	}
}



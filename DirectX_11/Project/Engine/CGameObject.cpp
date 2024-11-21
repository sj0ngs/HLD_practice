#include "pch.h"
#include "CGameObject.h"
#include "CComponent.h"

#include "CRenderComponent.h"

#include "CScript.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"

CGameObject::CGameObject()	:
	m_arrCom{},
	m_RenderCom(nullptr),
	m_Parent(nullptr),
	m_iLayerIdx(-1),
	m_bDead(false),
	m_LifeTime(0.f),
	m_bLifeSpan(false),
	m_CurLifeTime(0.f),
	m_bIsScirptObj(false)
{
}

CGameObject::CGameObject(const CGameObject& _Other)	:
	CEntity(_Other),
	m_arrCom{},
	m_RenderCom(nullptr),
	m_Parent(nullptr),
	m_iLayerIdx(-1),
	m_bDead(false),
	m_LifeTime(0.f),
	m_bLifeSpan(false),
	m_CurLifeTime(0.f),
	m_bIsScirptObj(_Other.m_bIsScirptObj)
{
	// Component Clone
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (_Other.m_arrCom[i])
		{
			AddComponent(_Other.m_arrCom[i]->Clone());
		}
	}

	// Script Clone
	for (size_t i = 0; i < _Other.m_vecScript.size(); i++)
	{
		AddComponent(_Other.m_vecScript[i]->Clone());
	}

	// Child Clone
	for (size_t i = 0; i < _Other.m_vecChild.size(); i++)
	{
		AddChild(_Other.m_vecChild[i]->Clone());
	}
}

CGameObject::~CGameObject()
{
	Safe_Del_Array(m_arrCom);
	Safe_Del_Vec(m_vecScript);
	Safe_Del_Vec(m_vecChild);
}

void CGameObject::begin()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->begin();
	}

	for (size_t i = 0; i < m_vecScript.size(); i++)
	{
		m_vecScript[i]->begin();
	}
	
	for (size_t i = 0; i < m_vecChild.size(); i++)
	{
		m_vecChild[i]->begin();
	}
}

void CGameObject::end()
{
	for (size_t i = 0; i < m_vecScript.size(); i++)
	{
		m_vecScript[i]->end();
	}

	for (size_t i = 0; i < m_vecChild.size(); i++)
	{
		m_vecChild[i]->end();
	}
}

void CGameObject::tick()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->tick();
	}

	for (size_t i = 0; i < m_vecScript.size(); i++)
	{
		m_vecScript[i]->tick();
	}

	for (size_t i = 0; i < m_vecChild.size(); i++)
	{
		m_vecChild[i]->tick();
	}
}

void CGameObject::finaltick()
{
	if (m_bLifeSpan)
	{
		m_CurLifeTime += DT;

		if (m_LifeTime <= m_CurLifeTime)
		{
			DestoryObject(this);
		}
	}

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::SCRIPT; i++)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finaltick();
	}

	for (size_t i = 0; i < m_vecChild.size(); i++)
	{
		m_vecChild[i]->finaltick();
	}

	// 레이어 등록
	if (-1 == m_iLayerIdx)
	{
		if (GetParent())
		{
			m_iLayerIdx = GetParent()->GetLayerIndex();
		}
	}

	assert(-1 != m_iLayerIdx);
	CLayer* pCurlayer = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(m_iLayerIdx);

	pCurlayer->RegisterObject(this);
}

void CGameObject::render()
{
	if (nullptr != m_RenderCom)
	{
		m_RenderCom->render();
	}
}

void CGameObject::AddComponent(CComponent* _Component)
{
	_Component->m_pOwner = this;

	if (COMPONENT_TYPE::SCRIPT == _Component->GetType())
	{
		m_vecScript.push_back((CScript*)_Component);
	}
	else
	{
		// 이미 보유중인 컴포넌트인 경우
		assert(!m_arrCom[(UINT)_Component->GetType()]);

		m_arrCom[(UINT)_Component->GetType()] = _Component;

		// RenderComponent 
		if (COMPONENT_TYPE::MESHRENDER <= _Component->GetType() &&
			_Component->GetType() <= COMPONENT_TYPE::DECAL)
		{
			assert(!m_RenderCom);
			m_RenderCom = (CRenderComponent*)_Component;
		}
	}
}

void CGameObject::AddChild(CGameObject* _Object)
{
	if (_Object->m_Parent)
	{
		_Object->DisconnectFromParent();
	}
	else
	{
		_Object->ChangeToChildType();
	}

	_Object->m_Parent = this;
	m_vecChild.push_back(_Object);
}

bool CGameObject::IsAncestor(CGameObject* _Target)
{
	CGameObject* pParent = m_Parent;
	
	while (pParent)
	{
		if (pParent == _Target)
		{
			return true;
		}
		pParent = pParent->m_Parent;
	}

	return false;
}

void CGameObject::DeleteComponent(COMPONENT_TYPE _Type)
{
	if (m_arrCom[(UINT)_Type])
	{
		delete m_arrCom[(UINT)_Type];
		m_arrCom[(UINT)_Type] = nullptr;
	}
}

void CGameObject::InitAllScript()
{
	for (int i = 0; i < (int)m_vecScript.size(); i++)
	{
		if (nullptr == m_vecScript[i])
			continue;

		InitScript((CScript*)m_vecScript[i]);
	}
}

void CGameObject::DisconnectFromParent()
{
	if (!m_Parent)
	{
		return;
	}

	vector<CGameObject*>::iterator iter = m_Parent->m_vecChild.begin();
	for (; iter != m_Parent->m_vecChild.end(); ++iter)
	{
		if (this == *iter)
		{
			m_Parent->m_vecChild.erase(iter);
			m_Parent = nullptr;
			return;
		}
	}

	assert(nullptr);
}

void CGameObject::ChangeToChildType()
{
	assert(-1 <= m_iLayerIdx && MAX_LAYER > m_iLayerIdx);
	
	if (-1 != m_iLayerIdx)
	{
		CLayer* pLayer = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(m_iLayerIdx);
		pLayer->RemoveFromParentList(this);
	}
}

void CGameObject::AddParentList()
{
	CLayer* pLayer = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(m_iLayerIdx);
	pLayer->AddParentList(this);
}

CScript* CGameObject::GetScript(UINT _iScriptType)
{
	for (size_t i = 0; i < m_vecScript.size(); i++)
	{
		if (m_vecScript[i]->GetScriptType() == _iScriptType)
			return m_vecScript[i];
	}

	return nullptr;
}

void CGameObject::DeleteScript(UINT _iScriptType)
{
	vector<CScript*>::iterator iter = m_vecScript.begin();

	for(iter; iter != m_vecScript.end(); iter++)
	{
		if ((*iter)->GetScriptType() == _iScriptType)
		{
			delete (*iter);
			m_vecScript.erase(iter);
			break;
		}
	}
}

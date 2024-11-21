#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"

CLayer::CLayer()	:
	m_iLayerIdx(-1)
{
}

CLayer::~CLayer()
{
	Safe_Del_Vec(m_vecParentObj);
}

void CLayer::begin()
{
	for (size_t i = 0; i < m_vecParentObj.size(); i++)
	{
		m_vecParentObj[i]->begin();
	}
}

void CLayer::end()
{
	for (size_t i = 0; i < m_vecParentObj.size(); i++)
	{
		m_vecParentObj[i]->end();
	}
}

void CLayer::tick()
{
	for (size_t i = 0; i < m_vecParentObj.size(); i++)
	{
		m_vecParentObj[i]->tick();
	}
}

void CLayer::finaltick()
{
	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();

	for (; iter != m_vecParentObj.end();)
	{
		(*iter)->finaltick();

		if ((*iter)->IsDead())
		{
			iter = m_vecParentObj.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLayer::AddGameObject(CGameObject* _Object, bool _bMove)
{
	m_vecParentObj.push_back(_Object);

	// �����ϰ� �ִ� �ڽ� ������Ʈ�� �ִ��� �˻�
	static list<CGameObject*> queue;
	queue.clear();

	queue.push_back(_Object);

	while (!queue.empty())
	{
		CGameObject* pObject = queue.front();
		queue.pop_front();

		for (size_t i = 0; i < pObject->m_vecChild.size(); i++)
		{
			queue.push_back(pObject->m_vecChild[i]);
		}

		// �θ�Ÿ�� or �Ҽ� ���̾ ���°�� or �θ�� ���� �̵��ϴ� ���
		if (nullptr == pObject->m_Parent || -1 == pObject->m_iLayerIdx || _bMove)
		{
			pObject->m_iLayerIdx = m_iLayerIdx;
		}
	}
}

void CLayer::RegisterObject(CGameObject* _Object)
{
	m_vecObject.push_back(_Object);
}

void CLayer::AddParentList(CGameObject* _Object)
{
	assert(!_Object->m_Parent);

	m_vecParentObj.push_back(_Object);
}

void CLayer::RemoveFromParentList(CGameObject* _Object)
{
	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();

	for (; iter != m_vecParentObj.end(); ++iter)
	{
		if ((*iter) == _Object)
		{
			m_vecParentObj.erase(iter);
			return;
		}
	}
}

void CLayer::Clear()
{
	m_vecObject.clear();
}

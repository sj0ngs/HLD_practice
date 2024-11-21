#include "pch.h"
#include "CAstar.h"

#include "CResMgr.h"

CAstar::CAstar()	:
	m_matGrid(nullptr),
	m_pStartNode(nullptr),
	m_pDestNode(nullptr),
	m_iGridSizeX(0),
	m_iGridSizeY(0)
{
}

CAstar::~CAstar()
{
	ClearGrid();
}

void CAstar::SetGridSize(int _iX, int _iY)
{
	ClearGrid();

	m_iGridSizeX = _iX;
	m_iGridSizeY = _iY;

	m_matGrid = new tGridNode*[m_iGridSizeY];
	for (int i = 0; i < m_iGridSizeY; i++)
	{
		m_matGrid[i] = new tGridNode[m_iGridSizeX];
	}
}

void CAstar::SetStartPos(Vec2 _vPos)
{
	Vec2 vGridScale = NodeSize * Vec2(m_iGridSizeX, m_iGridSizeY);

	Vec2 vLeftTop = m_vGridCenterPos;
	vLeftTop.x -= vGridScale.x / 2.f;
	vLeftTop.y += vGridScale.y / 2.f;

	float fXDist = _vPos.x - vLeftTop.x;
	float fYDist = vLeftTop.y - _vPos.y;

	int iIdxX = (int)roundf(fXDist / NodeSize);	
	int iIdxY = (int)roundf(fYDist / NodeSize);

	m_pStartNode = &m_matGrid[iIdxY][iIdxX];
}

void CAstar::SetDestPos(Vec2 _vPos)
{
	Vec2 vGridScale = NodeSize * Vec2(m_iGridSizeX, m_iGridSizeY);

	Vec2 vLeftTop = m_vGridCenterPos;
	vLeftTop.x -= vGridScale.x / 2.f;
	vLeftTop.y += vGridScale.y / 2.f;

	float fXDist = _vPos.x - vLeftTop.x;
	float fYDist = vLeftTop.y - _vPos.y;

	int iIdxX = (int)roundf(fXDist / NodeSize);
	int iIdxY = (int)roundf(fYDist / NodeSize);

	m_pStartNode = &m_matGrid[iIdxY][iIdxX];
}

void CAstar::CalculatePath(list<Vec2>& _listPath)
{
	assert(m_pStartNode);
	assert(m_pDestNode);

	m_pqOpenList.emplace();
	tGridNode* pNode = m_pStartNode;
	AddToOpenList(pNode);

	// 우선순위 큐에 아무것도 없을때까지 돌림
	while (!m_pqOpenList.empty())
	{
		pNode = m_pqOpenList.top();
		m_pqOpenList.pop();
		pNode->bClosed = true;

		if (pNode == m_pDestNode)
			break;

		// 인근 8노드 openlist 에 집어넣기
		// 상 
		AddToOpenList(CheckNextNode(pNode, 0, -1));

		// 우상
		AddToOpenList(CheckNextNode(pNode, 1, -1));

		// 우
		AddToOpenList(CheckNextNode(pNode, 1, 0));

		// 우하
		AddToOpenList(CheckNextNode(pNode, 1, 1));

		// 하
		AddToOpenList(CheckNextNode(pNode, 0, 1));

		// 좌하
		AddToOpenList(CheckNextNode(pNode, -1, 1));

		// 좌
		AddToOpenList(CheckNextNode(pNode, -1, 0));

		// 좌상
		AddToOpenList(CheckNextNode(pNode, -1, -1));
	}

	while (pNode)
	{
		_listPath.push_front(pNode->vWorldPos);
		pNode = pNode->pPrevNode;
	}
}

void CAstar::ClearGrid()
{
	if (m_matGrid)
	{
		for (int i = 0; i < m_iGridSizeY; i++)
		{
			delete[] m_matGrid[i];
		}

		delete[] m_matGrid;
	}

	m_matGrid = nullptr;
}

void CAstar::ResetGrid()
{
	assert(m_matGrid);

	for (int iRow = 0; iRow < m_iGridSizeY; iRow++)
	{
		for (int iCol = 0; iCol < m_iGridSizeX; iCol++)
		{
			m_matGrid[iRow][iCol].pPrevNode = nullptr;
			m_matGrid[iRow][iCol].bOpen = false;
			m_matGrid[iRow][iCol].bClosed = false;
		}
	}
}

void CAstar::BuildGridMap()
{
	// 장애물 텍스쳐를 이용해서 그리드를 생성함
	// 장애물 텍스쳐에는 벽과 충돌체 등등의 정보가 그림으로 담겨 있음
}

tGridNode* CAstar::CheckNextNode(tGridNode* _pCurNode, int _iXOffset, int _iYOffset)
{
	int iX = _pCurNode->iIdxX + _iXOffset;
	int iY = _pCurNode->iIdxY + _iYOffset;

	// 범위 내의 인덱스인지 확인
	if (0 > iX || m_iGridSizeX <= iX || 0 > iY || m_iGridSizeY <= iY)
		return nullptr;

	tGridNode* pNext = &m_matGrid[iY][iX];

	// 갈 수 없는 노드인지 확인
	if (false == pNext->bAble)
		return nullptr;

	pNext->pPrevNode = _pCurNode;

	return pNext;
}

void CAstar::AddToOpenList(tGridNode* _pNode)
{
	_pNode->fFromStart = GetDistFromStart(_pNode);
	_pNode->fToDest = Vec2::Distance(_pNode->vWorldPos, m_pDestNode->vWorldPos);
	_pNode->fPriority = _pNode->fFromStart + _pNode->fToDest;
	_pNode->bOpen = true;
	m_pqOpenList.push(_pNode);
}

float CAstar::GetDistFromStart(tGridNode* _pNode)
{
	tGridNode* pPrevNode = _pNode->pPrevNode;
	if (pPrevNode)
		return pPrevNode->fFromStart + Vec2::Distance(_pNode->vWorldPos, pPrevNode->vWorldPos);
	else
		return 0.f;
}

#pragma once

#define NodeSize 16.f

struct tGridNode
{
	tGridNode*	pPrevNode = nullptr;
	float		fFromStart;		// g(x)
	float		fToDest;		// h(x)
	float		fPriority;		// f(x) = g(x) + h(x)

	int			iIdxX;
	int			iIdxY;
	Vec2		vWorldPos;
	bool		bAble;
	bool		bOpen;
	bool		bClosed;
};

class Compare
{
public:
	bool operator() (tGridNode* _pLeft, tGridNode* _pRight)
	{
		return _pLeft->fPriority > _pRight->fPriority;
	}
};

class CTexture;
class CAstar
{
private:
	tGridNode** m_matGrid;
	tGridNode*	m_pStartNode;
	tGridNode*	m_pDestNode;

	int			m_iGridSizeX;
	int			m_iGridSizeY;
	
	priority_queue<tGridNode*, vector<tGridNode*>, Compare> m_pqOpenList;

	Vec2 m_vGridCenterPos;

	Ptr<CTexture> m_pObstacleTex;

public:
	void SetGridSize(int _iX, int _iY);
	void SetStartPos(Vec2 _vPos);
	void SetDestPos(Vec2 _vPos);
	void SetGridCenterPos(Vec2 _vPos) { m_vGridCenterPos = _vPos; }
	void SetObstacleTex(Ptr<CTexture> _pTex) { m_pObstacleTex = _pTex; }

public:
	CAstar();
	~CAstar();

public:
	void CalculatePath(list<Vec2>& _listPath);
	void ClearGrid();
	void ResetGrid();
	// Compute Shader 를 이용해서 그리드 맵 빌드
	void BuildGridMap();

private:
	tGridNode* CheckNextNode(tGridNode* _pCurNode, int _iXOffset, int _iYOffset);
	void AddToOpenList(tGridNode* _pNode);
	float GetDistFromStart(tGridNode* _pNode);
};


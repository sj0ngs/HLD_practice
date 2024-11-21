#include "pch.h"
#include "CCollisionMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"

#include "CGameObject.h"

#include "CTransform.h"
#include "CCollider2D.h"

CCollisionMgr::CCollisionMgr() :
	m_matrix{}
{

}

CCollisionMgr::~CCollisionMgr()
{

}

void CCollisionMgr::tick()
{
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT iRow = 0; iRow < MAX_LAYER; iRow++)
	{
		for (UINT iCol = iRow; iCol < MAX_LAYER; iCol++)
		{
			if (!(m_matrix[iRow] & (1 << iCol)))
				continue;

			CollisionBtwLayer(pLevel->GetLayer(iRow), pLevel->GetLayer(iCol));
		}
	}
}

void CCollisionMgr::CollisionBtwLayer(CLayer* _LeftLayer, CLayer* _RightLayer)
{
	const vector<CGameObject*>& vecLeft = _LeftLayer->GetObjects();
	const vector<CGameObject*>& vecRight = _RightLayer->GetObjects();

	if (_LeftLayer == _RightLayer)
	{
		for (size_t i = 0; i < vecLeft.size(); i++)
		{
			for (size_t j = i + 1; j < vecRight.size(); j++)
			{
				CollisionBtwObj(vecLeft[i], vecRight[j]);
			}
		}
	}
	else
	{
		for (size_t i = 0; i < vecLeft.size(); i++)
		{
			for (size_t j = 0; j < vecRight.size(); j++)
			{
				CollisionBtwObj(vecLeft[i], vecRight[j]);
			}
		}
	}
}

void CCollisionMgr::CollisionBtwObj(CGameObject* _LeftObj, CGameObject* _RightObj)
{
	CCollider2D* pLeft = _LeftObj->Collider2D();
	CCollider2D* pRight = _RightObj->Collider2D();

	if (!(pLeft && pRight))
		return;

	CollisionID id = {};
	id.leftID = pLeft->GetID();
	id.rightID = pRight->GetID();

	map<UINT_PTR, bool>::iterator iter = m_mapColID.find(id.id);

	if (m_mapColID.end() == iter)
	{
		m_mapColID.insert(make_pair(id.id, false));
		iter = m_mapColID.find(id.id);
	}

	bool bDead = false;
	if (_LeftObj->IsDead() || _RightObj->IsDead())
	{
		bDead = true;
	}

	if (CollisionBtwCollider(pLeft, pRight))
	{
		if (bDead && iter->second)
		{
			pLeft->EndOverlap(pRight);
			pRight->EndOverlap(pLeft);
		}
		else if (iter->second)
		{
			pLeft->OnOverlap(pRight);
			pRight->OnOverlap(pLeft);
		}
		else
		{
			if (!bDead)
			{
				pLeft->BeginOverlap(pRight);
				pRight->BeginOverlap(pLeft);
				iter->second = true;
			}
		}
	}
	else
	{
		if (iter->second)
		{
			pLeft->EndOverlap(pRight);
			pRight->EndOverlap(pLeft);

			iter->second = false;
		}
	}
}

bool CCollisionMgr::CollisionBtwCollider(CCollider2D* _LeftCol, CCollider2D* _RightCol)
{
	COLLIDER2D_TYPE eLeft = _LeftCol->GetCollider2DType();
	COLLIDER2D_TYPE eRight = _RightCol->GetCollider2DType();

	bool result = false;

	if (!_LeftCol->IsOnCollision() || !_RightCol->IsOnCollision())
		return false;

	if (eLeft == eRight)
	{
		switch (eLeft)
		{
		case COLLIDER2D_TYPE::RECT:
			result = CollisionBtB(_LeftCol, _RightCol);
			break;
		case COLLIDER2D_TYPE::CIRCLE:
			result = CollisionCtC(_LeftCol, _RightCol);
			break;
		case COLLIDER2D_TYPE::TRI:
			result = CollisionTtT(_LeftCol, _RightCol);
			break;
		}
	}
	else
	{
		CCollider2D* pLeft = _LeftCol;
		CCollider2D* pRight = _RightCol;

		if (COLLIDER2D_TYPE::RECT == eLeft)
		{
			CCollider2D* pTemp = pLeft;
			pLeft = pRight;
			pRight = pTemp;
		}

		result = CollisionCtB(pLeft, pRight);
	}

	return result;
}

bool CCollisionMgr::CollisionBtB(CCollider2D* _LeftCol, CCollider2D* _RightCol)
{
	// 0 - 1
	// |   |
	// 3 - 2
	Vec3 arrLocal[4] =
	{
		Vec3(-0.5f, 0.5f, 0.f),
		Vec3(0.5f, 0.5f, 0.f),
		Vec3(0.5f, -0.5f, 0.f),
		Vec3(-0.5f, -0.5f, 0.f)
	};

	// 두 사각형의 평행하지 않는 두 변을 골라 그와 평행한 벡터로 단위 벡터 생성
	Vec3 arrProj[4] = {};

	// LeftRect
	// 0 -> 1
	Matrix matLeft = _LeftCol->GetColliderWorldMatrix();
	arrProj[0] = XMVector3TransformCoord(arrLocal[1], matLeft) - XMVector3TransformCoord(arrLocal[0], matLeft);
	// 0 -> 3
	arrProj[1] = XMVector3TransformCoord(arrLocal[3], matLeft) - XMVector3TransformCoord(arrLocal[0], matLeft);

	// RightRect
	// 0 -> 1
	Matrix matRight = _RightCol->GetColliderWorldMatrix();
	arrProj[2] = XMVector3TransformCoord(arrLocal[1], matRight) - XMVector3TransformCoord(arrLocal[0], matRight);
	// 0 -> 3
	arrProj[3] = XMVector3TransformCoord(arrLocal[3], matRight) - XMVector3TransformCoord(arrLocal[0], matRight);

	// 두 사각형의 중심을 이은 벡터 생성
	Vec3 vCenter = Vec3(0.f, 0.f, 0.f);
	vCenter = XMVector3TransformCoord(vCenter, matRight) - XMVector3TransformCoord(vCenter, matLeft);

	for (int i = 0; i < 4; i++)
	{
		Vec3 vProj = arrProj[i];
		vProj.Normalize();

		float fProjDist = 0.f;
		for (int j = 0; j < 4; j++)
		{
			fProjDist += fabsf(arrProj[j].Dot(vProj));
		}
		fProjDist /= 2.f;

		float fProjCenter = fabsf(vCenter.Dot(vProj));

		if (fProjDist < fProjCenter)
		{
			return false;
		}
	}

	return true;
}

bool CCollisionMgr::CollisionCtC(CCollider2D* _LeftCol, CCollider2D* _RightCol)
{
	float fRadius = _LeftCol->GetRadius() + _RightCol->GetRadius();

	Vec3 vCenter = {};
	
	vCenter = XMVector3TransformCoord(vCenter, _LeftCol->GetColliderWorldMatrix())
			- XMVector3TransformCoord(vCenter, _RightCol->GetColliderWorldMatrix());

	vCenter.z = 0.f;

	float fDist = vCenter.Length();

	if (fDist > fRadius)
		return false;

	return true;
}

bool CCollisionMgr::CollisionTtT(CCollider2D* _LeftCol, CCollider2D* _RightCol)
{
	Vec3 arrLocal[3] =
	{
		Vec3(0.f, (float)(sqrt(3.f) / 3.f), 0.f),
		Vec3(0.5f, (float)(sqrt(3.f) / -6.f), 0.f),
		Vec3(-0.5f, (float)(sqrt(3.f) / -6.f), 0.f)
	};

	Vec3 arrLeft[3] = {};
	Vec3 arrRight[3] = {};
	Matrix matLeft = _LeftCol->GetColliderWorldMatrix();
	Matrix matRight = _RightCol->GetColliderWorldMatrix();

	for (int i = 0; i < 3; i++)
	{
		arrLeft[i] = XMVector3TransformCoord(arrLocal[i], matLeft);
		arrLeft[i].z = 0.f;
		arrRight[i] = XMVector3TransformCoord(arrLocal[i], matRight);
		arrRight[i].z = 0.f;
	}
	
	Vec3 arrProj[6] = {};
	for (int i = 0; i < 3; i++)
	{
		arrProj[i] = arrLeft[(i + 1) % 3] - arrLeft[i];
		arrProj[i + 3] = arrRight[(i + 1) % 3] - arrRight[i];
	}

	float fLeftMax = (float)INT_MIN;

	for (int i = 0; i < 3; i++)
	{
		Vec3 vProj = arrProj[i];
		vProj = arrProj[i].Cross(Vec3(0.f, 0.f, -1.f));
		vProj.Normalize();

		
		float fMin = (float)INT_MAX;
		for (int j = 0; j < 3; j++)
		{
			Vec3 Vec = arrRight[j] - arrLeft[i];
			float fDot = Vec.Dot(vProj);

			if (fMin > fDot)
				fMin = fDot;
		}

		if (fLeftMax < fMin)
			fLeftMax = fMin;
	}

	if (0.f < fLeftMax)
		return false;

	float fRightMax = (float)INT_MIN;

	for (int i = 0; i < 3; i++)
	{
		Vec3 vProj = arrProj[i + 3];
		vProj = arrProj[i + 3].Cross(Vec3(0.f, 0.f, -1.f));
		vProj.Normalize();

		float fMin = (float)INT_MAX;
		for (int j = 0; j < 3; j++)
		{
			Vec3 Vec = arrLeft[j] - arrRight[i];
			float fDot = Vec.Dot(vProj);

			if (fMin > fDot)
				fMin = fDot;
		}

		if (fRightMax < fMin)
			fRightMax = fMin;
	}

	if (0.f < fRightMax)
		return false;

	return true;
}

bool CCollisionMgr::CollisionCtB(CCollider2D* _LeftCol, CCollider2D* _RightCol)
{
	// 월드 스케일을 유지하면서 박스 기준 좌표계로 이동하기 위한 RT 역행렬
	Matrix matBoxScale = _RightCol->GetColliderWorldScale();
	Matrix matBoxRTInv = XMMatrixInverse(nullptr, matBoxScale);
	matBoxRTInv *= _RightCol->GetColliderWorldMatrix();
	matBoxRTInv = XMMatrixInverse(nullptr, matBoxRTInv);

	// 원의 원점 이동
	Vec3 vCenter = {};
	vCenter = XMVector3TransformCoord(vCenter, _LeftCol->GetColliderWorldMatrix());
	vCenter = XMVector3TransformCoord(vCenter, matBoxRTInv);
	float fRadius = _LeftCol->GetRadius();

	Vec3 arrLocal[4] =
	{
		Vec3(-0.5f, 0.5f, 0.f),
		Vec3(0.5f, 0.5f, 0.f),
		Vec3(0.5f, -0.5f, 0.f),
		Vec3(-0.5f, -0.5f, 0.f)
	};

	for (int i = 0; i < 4; i++)
	{
		arrLocal[i] = XMVector3TransformCoord(arrLocal[i], matBoxScale);
	}

	// 반지름의 크기만큼 확장된 사각형의 영역안에 원의 중심이 존재하는지 확인
	if ((arrLocal[0].x - fRadius) <= vCenter.x && (arrLocal[2].x + fRadius) >= vCenter.x &&
		(arrLocal[0].y + fRadius) >= vCenter.y && (arrLocal[2].y - fRadius) <= vCenter.y)
	{
		Vec3 vDist = {};
		if (arrLocal[0].x > vCenter.x && arrLocal[0].y < vCenter.y)
		{
			vDist = arrLocal[0] - vCenter;
		}
		else if (arrLocal[1].x < vCenter.x && arrLocal[1].y < vCenter.y)
		{
			vDist = arrLocal[1] - vCenter;
		}
		else if (arrLocal[2].x < vCenter.x && arrLocal[2].y > vCenter.y)
		{
			vDist = arrLocal[2] - vCenter;
		}
		else if (arrLocal[3].x > vCenter.x && arrLocal[3].y > vCenter.y)
		{
			vDist = arrLocal[3] - vCenter;	
		}

		vDist.z = 0.f;
		if (fRadius < vDist.Length())
			return false;

		return true;
	}

	return false;
}

void CCollisionMgr::LayerCheck(UINT _left, UINT _right)
{
	UINT iRow = _left;
	UINT iCol = _right;

	if (iCol < iRow)
	{
		UINT iTemp = iRow;
		iRow = iCol;
		iCol = iTemp;
	}

	m_matrix[iRow] |= (1 << iCol);
}

void CCollisionMgr::LayerCheck(const wstring& _strLeftLayer, const wstring& _strRightLayer)
{
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();

	CLayer* pLeft = pLevel->FindLayerByName(_strLeftLayer);
	CLayer* pRight = pLevel->FindLayerByName(_strRightLayer);
	
	LayerCheck(pLeft->GetLayerIdx(), pRight->GetLayerIdx());
}

CGameObject* CCollisionMgr::GetPickedObject(Vec3 _vPickPos)
{
	CGameObject* pPickedObj = nullptr;

	Vec3 vPickPos = _vPickPos;

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (int i = 0; i < MAX_LAYER; i++)
	{
		CLayer* pLayer = pCurLevel->GetLayer(i);
		const vector<CGameObject*>& vecObj = pLayer->GetObjects();

		for (size_t j = 0; j < vecObj.size(); j++)
		{
			CCollider2D* pCol = vecObj[j]->Collider2D();

			if (nullptr == pCol)
				continue;

			switch (pCol->GetCollider2DType())
			{
			case COLLIDER2D_TYPE::RECT:
			{
				// 월드 스케일을 유지하면서 박스 기준 좌표계로 이동하기 위한 RT 역행렬
				Matrix matBoxScale = pCol->GetColliderWorldScale();
				Matrix matBoxRTInv = XMMatrixInverse(nullptr, matBoxScale);
				matBoxRTInv *= pCol->GetColliderWorldMatrix();
				matBoxRTInv = XMMatrixInverse(nullptr, matBoxRTInv);

				Vec3 vRotPickPos = XMVector3TransformCoord(vPickPos, matBoxRTInv);

				Vec3 arrLocal[2] =
				{
					Vec3(-0.5f, 0.5f, 0.f),
					Vec3(0.5f, -0.5f, 0.f)
				};

				for (int k = 0; k < 2; k++)
				{
					arrLocal[k] = XMVector3TransformCoord(arrLocal[k], matBoxScale);
				}

				if (arrLocal[0].x <= vRotPickPos.x && arrLocal[1].x >= vRotPickPos.x &&
					arrLocal[0].y >= vRotPickPos.y && arrLocal[1].y <= vRotPickPos.y)
				{
					if (nullptr != pPickedObj)
					{
						if (pPickedObj->Transform()->GetWorldPos().z > pCol->Transform()->GetWorldPos().z)
						{
							pPickedObj = pCol->GetOwner();
						}
					}
					else
					{
						pPickedObj = pCol->GetOwner();
					}
				}
			}
				break;
			case COLLIDER2D_TYPE::CIRCLE:
			{
				Vec3 vColPos = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), pCol->GetColliderWorldMatrix());
				float fRadius = pCol->GetRadius();

				Vec3 vAlignPickPos = Vec3(vPickPos.x, vPickPos.y, vColPos.z);

				if(fRadius >= Vec3::Distance(vColPos, vAlignPickPos))
				{
					if (nullptr != pPickedObj)
					{
						if (pPickedObj->Transform()->GetWorldPos().z > pCol->Transform()->GetWorldPos().z)
						{
							pPickedObj = pCol->GetOwner();
						}
					}
					else
					{
						pPickedObj = pCol->GetOwner();
					}
				}
			}
				break;
			}
		}
	}

	return pPickedObj;
}
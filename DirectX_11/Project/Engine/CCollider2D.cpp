#include "pch.h"
#include "CCollider2D.h"

#include "components.h"
#include "CScript.h"

CCollider2D::CCollider2D() :
	CComponent(COMPONENT_TYPE::COLLIDER2D),
	m_vOffsetPos{},
	m_vOffsetScale(1.f, 1.f, 1.f),
	m_bAbsolute(false),
	m_Shape(COLLIDER2D_TYPE::RECT),
	m_iCollisionCount(0),
	m_bOn(true)
{
	SetName(L"Collider2D");
}

CCollider2D::CCollider2D(const CCollider2D& _Other)	:
	CComponent(_Other),
	m_vOffsetPos(_Other.m_vOffsetPos),
	m_vOffsetScale(_Other.m_vOffsetScale),
	m_bAbsolute(_Other.m_bAbsolute),
	m_Shape(_Other.m_Shape),
	m_iCollisionCount(0),
	m_bOn(_Other.m_bOn)
{
}

CCollider2D::~CCollider2D()
{
}

void CCollider2D::finaltick()
{
	assert(!(0 > m_iCollisionCount));

	m_matWorldScale = XMMatrixScaling(m_vOffsetScale.x, m_vOffsetScale.y, m_vOffsetScale.z);
	m_matCollider2D = m_matWorldScale;
	m_matCollider2D *= XMMatrixTranslation(m_vOffsetPos.x, m_vOffsetPos.y, m_vOffsetPos.z);
	
	const Matrix& matOwnerWorld = Transform()->GetWorldMat();
	if (m_bAbsolute)
	{
		Matrix matOwnerScaleInv = XMMatrixInverse(nullptr, Transform()->GetWorldScale());
		m_matCollider2D = m_matCollider2D * matOwnerScaleInv * matOwnerWorld;
	}
	else
	{
		m_matCollider2D *= matOwnerWorld;
		m_matWorldScale *= Transform()->GetWorldScale();
	}

	Vec4 vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	if (0 < m_iCollisionCount)
	{
		vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	}
	
	switch (m_Shape)
	{
	case COLLIDER2D_TYPE::RECT:
		DrawDebugRect(m_matCollider2D, vColor, 0.f);
		break;
	case COLLIDER2D_TYPE::CIRCLE:
		DrawDebugCircle(m_matCollider2D, vColor, 0.f);
		break;
	case COLLIDER2D_TYPE::TRI:
		DrawDebugTri(m_matCollider2D, vColor, 0.f);
		break;
	}
 }

Vec2 CCollider2D::GetWorldPos()
{
	if (m_bAbsolute)
		return m_vOffsetPos.XY() + Transform()->GetRelativePos().XY();
	else
		return (m_vOffsetPos + Transform()->GetRelativePos()).XY();
}

Vec2 CCollider2D::GetWorldScale()
{
	if (m_bAbsolute)
		return m_vOffsetScale.XY();
	else
		return (m_vOffsetScale * Transform()->GetRelativeScale()).XY();
}


void CCollider2D::BeginOverlap(CCollider2D* _Other)
{
	++m_iCollisionCount;

	const vector<CScript*>& vecScript = GetOwner()->GetScripts();

	for (size_t i = 0; i < vecScript.size(); i++)
	{
		vecScript[i]->BeginOverlap(_Other);
	}
}

void CCollider2D::OnOverlap(CCollider2D* _Other)
{
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();

	for (size_t i = 0; i < vecScript.size(); i++)
	{
		vecScript[i]->OnOverlap(_Other);
	}
}

void CCollider2D::EndOverlap(CCollider2D* _Other)
{
	--m_iCollisionCount;

	const vector<CScript*>& vecScript = GetOwner()->GetScripts();

	for (size_t i = 0; i < vecScript.size(); i++)
	{
		vecScript[i]->EndOverlap(_Other);
	}
}

float CCollider2D::GetRadius() const
{
	assert(COLLIDER2D_TYPE::CIRCLE == m_Shape);

	if (m_bAbsolute)
	{
		return m_vOffsetScale.x * 0.5f;
	}

	return m_vOffsetScale.x * Transform()->GetWorldScale()._11 * 0.5f;
}

void CCollider2D::SetRadius(float _fRadius)
{
	assert(COLLIDER2D_TYPE::CIRCLE == m_Shape);

	float fRadius = _fRadius * 2.f;

	if(!m_bAbsolute)
		fRadius /= Transform()->GetWorldScale()._11;
	
	SetOffsetScale(Vec2(fRadius, fRadius));
}

void CCollider2D::SaveToLevelFile(FILE* _pFile)
{
	fwrite(&m_vOffsetPos, sizeof(m_vOffsetPos), 1, _pFile);
	fwrite(&m_vOffsetScale, sizeof(m_vOffsetScale), 1, _pFile);
	fwrite(&m_bAbsolute, sizeof(m_bAbsolute), 1, _pFile);
	fwrite(&m_Shape, sizeof(m_Shape), 1, _pFile);
}

void CCollider2D::LoadFromLevelFile(FILE* _pFile)
{
	fread(&m_vOffsetPos, sizeof(m_vOffsetPos), 1, _pFile);
	fread(&m_vOffsetScale, sizeof(m_vOffsetScale), 1, _pFile);
	fread(&m_bAbsolute, sizeof(m_bAbsolute), 1, _pFile);
	fread(&m_Shape, sizeof(m_Shape), 1, _pFile);
}

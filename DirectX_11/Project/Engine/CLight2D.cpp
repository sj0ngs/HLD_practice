#include "pch.h"
#include "CLight2D.h"

#include "CRenderMgr.h"
#include "CTransform.h"

CLight2D::CLight2D()	:
	CComponent(COMPONENT_TYPE::LIGHT2D),
	m_LightInfo{},
	m_vOffsetPos{},
	m_bAbsolute(false)
{
}

CLight2D::~CLight2D()
{
}

void CLight2D::finaltick()
{
	Matrix matWorld = XMMatrixTranslation(m_vOffsetPos.x, m_vOffsetPos.y, m_vOffsetPos.z);
										
	m_LightInfo.vWorldPos = (matWorld * Transform()->GetWorldMat()).Translation();
	m_LightInfo.vWorldDir = Transform()->GetWorldDir(DIR_TYPE::RIGHT);

	CRenderMgr::GetInst()->RegisterLight2D(m_LightInfo);

	if (LIGHT_TYPE::POINT <= (LIGHT_TYPE)m_LightInfo.LightType)
	{
		DrawDebugCircle(m_LightInfo.vWorldPos, m_LightInfo.Radius, Vec3(0.f, 0.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f));
	}
}

void CLight2D::SaveToLevelFile(FILE* _pFile)
{
	fwrite(&m_LightInfo, sizeof(m_LightInfo), 1, _pFile);
	fwrite(&m_vOffsetPos, sizeof(m_vOffsetPos), 1, _pFile);
	fwrite(&m_bAbsolute, sizeof(m_bAbsolute), 1, _pFile);
}

void CLight2D::LoadFromLevelFile(FILE* _pFile)
{
	fread(&m_LightInfo, sizeof(m_LightInfo), 1, _pFile);
	fread(&m_vOffsetPos, sizeof(m_vOffsetPos), 1, _pFile);
	fread(&m_bAbsolute, sizeof(m_bAbsolute), 1, _pFile);
}

#include "pch.h"
#include "CObstacleMapShader.h"

#include "CStructuredBuffer.h"

CObstacleMapShader::CObstacleMapShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup)	:
	CComputeShader(_iThreadXPerGroup, _iThreadYPerGroup, _iThreadZPerGroup),
	m_bColliderDraw(false)
{
}

CObstacleMapShader::~CObstacleMapShader()
{
}

void CObstacleMapShader::UpdateData()
{
	m_pMapTex->UpdateDate_CS(0, false);

	if(nullptr != m_pEdgeTex)
		m_pEdgeTex->UpdateDate_CS(0, true);
	if(nullptr != m_pWallTex)
		m_pWallTex->UpdateDate_CS(1, true);

	// 콜라이더의 텍스쳐 상에서의 위치와 사이즈를 미리 계산해서 넘김
	m_Const.arrInt[0] = m_pMapTex->Width();
	m_Const.arrInt[1] = m_pMapTex->Height();
	m_Const.arrInt[2] = (int)m_bColliderDraw;

	Vec2 vTexSize = Vec2(m_pMapTex->Width(), m_pMapTex->Height());
	m_Const.arrV2[0] = (m_vCollPos - Vec2(m_vRoomPos.x - m_vRoomSize.x / 2.f, m_vRoomPos.y + m_vRoomSize.y / 2.f)) / m_vRoomSize * vTexSize;
	m_Const.arrV2[0].y *= -1.f;
	m_Const.arrV2[1] = m_vCollSize / m_vRoomSize * vTexSize;

	// 그룹 수
	m_iGroupX = (UINT)m_pMapTex->Width() / m_iThreadXPerGroup;
	m_iGroupY = (UINT)m_pMapTex->Height() / m_iThreadYPerGroup;
	m_iGroupZ = 1;

	if (0 < (UINT)m_pMapTex->Width() / m_iThreadXPerGroup)
		m_iGroupX++;

	if (0 < (UINT)m_pMapTex->Height() / m_iThreadYPerGroup)
		m_iGroupY++;
}

void CObstacleMapShader::Clear()
{
	m_pMapTex->Clear_CS(false);

	if (nullptr != m_pEdgeTex)
		m_pEdgeTex->Clear_CS (true);
	if (nullptr != m_pWallTex)
		m_pWallTex->Clear_CS(true);
}
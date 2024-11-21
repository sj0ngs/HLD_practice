#include "pch.h"
#include "CRoomShadowShader.h"

CRoomShadowShader::CRoomShadowShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup)	:
	CComputeShader(_iThreadXPerGroup, _iThreadYPerGroup, _iThreadZPerGroup)
{
}

CRoomShadowShader::~CRoomShadowShader()
{
}

void CRoomShadowShader::UpdateData()
{
	m_pDrawTex->UpdateDate_CS(0, false);
	m_pLayer1Tex->UpdateDate_CS(0, true);

	if (nullptr != m_pShadowTex)
	{
		m_pShadowTex->UpdateDate_CS(1, true);
		m_Const.arrV2[1] = Vec2((float)m_pShadowTex->Width(), (float)m_pShadowTex->Height());
	}

	Vec2 vDrawResol = Vec2((float)m_pDrawTex->Width(), (float)m_pDrawTex->Height());
	Vec2 vID = m_vShadowPos / 2.f;
	vID -= Vec2(vDrawResol.x / -2.f, vDrawResol.y / 2.f);
	vID.y *= -1.f;

	m_Const.arrV2[0] = vDrawResol;
	m_Const.arrV2[2] = vID;
	m_Const.arrInt[0] = m_iEraseMode;

	// ±×·ì ¼ö
	m_iGroupX = (UINT)vDrawResol.x / m_iThreadXPerGroup;
	m_iGroupY = (UINT)vDrawResol.y / m_iThreadYPerGroup;
	m_iGroupZ = 1;

	if (0 < (UINT)vDrawResol.x % m_iThreadXPerGroup)
		m_iGroupX++;

	if (0 < (UINT)vDrawResol.y % m_iThreadYPerGroup)
		m_iGroupY++;
}

void CRoomShadowShader::Clear()
{
	m_pDrawTex->Clear_CS(false);
	m_pLayer1Tex->Clear_CS(true);

	if (nullptr != m_pShadowTex)
		m_pShadowTex->Clear_CS(true);
}

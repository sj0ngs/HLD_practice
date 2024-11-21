#include "pch.h"
#include "CSetColorShader.h"

CSetColorShader::CSetColorShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup)	:
	CComputeShader(_iThreadXPerGroup, _iThreadYPerGroup, _iThreadZPerGroup)
{
}

CSetColorShader::~CSetColorShader()
{
}

void CSetColorShader::UpdateData()
{
	m_OutTex->UpdateDate_CS(0, false);

	// 그룹 개수 계산
	m_iGroupX = (UINT)ceilf((float)m_OutTex->Width() / (float)m_iThreadXPerGroup);
	m_iGroupY = (UINT)ceilf((float)m_OutTex->Width() / (float)m_iThreadYPerGroup);
	m_iGroupZ = 1;

	m_Const.arrInt[0] = m_OutTex->Width();
	m_Const.arrInt[1] = m_OutTex->Height();
}

void CSetColorShader::Clear()
{
	m_OutTex->Clear();
}
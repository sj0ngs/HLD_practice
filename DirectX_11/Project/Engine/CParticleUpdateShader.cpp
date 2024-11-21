#include "pch.h"
#include "CParticleUpdateShader.h"

#include "CStructuredBuffer.h"

CParticleUpdateShader::CParticleUpdateShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup) :
	CComputeShader(_iThreadXPerGroup, _iThreadYPerGroup, _iThreadZPerGroup),
	m_ParticleBuffer(nullptr),
	m_RWBuffer(nullptr),
	m_ModuleData(nullptr)
{
}

CParticleUpdateShader::~CParticleUpdateShader()
{
}

void CParticleUpdateShader::SetParticleBuffer(CStructuredBuffer* _Buffer)
{
	m_ParticleBuffer = _Buffer;
	m_Const.arrInt[0] = m_ParticleBuffer->GetElementCount();
}

void CParticleUpdateShader::UpdateData()
{
	m_ParticleBuffer->UpdateData_CS(0, false);
	m_RWBuffer->UpdateData_CS(1, false);
	m_ModuleData->UpdateData_CS(20, true);
	m_NoiseTex->UpdateDate_CS(21, true);

	// ±×·ì ¼ö
	m_iGroupX = (UINT)ceilf((float)m_ParticleBuffer->GetElementCount() / (float)m_iThreadXPerGroup);
}

void CParticleUpdateShader::Clear()
{
	m_ParticleBuffer->Clear_CS(false);
	m_RWBuffer->Clear_CS(false);
	m_ModuleData->Clear_CS(true);
}

#pragma once
#include "CComputeShader.h"

#include "ptr.h"
#include "CTexture.h"

class CStructuredBuffer;

class CParticleUpdateShader :
    public CComputeShader
{
private:
    CStructuredBuffer*  m_ParticleBuffer;
    CStructuredBuffer*  m_RWBuffer;
    CStructuredBuffer*  m_ModuleData;
    Ptr<CTexture>       m_NoiseTex;

public:
    void SetParticleBuffer(CStructuredBuffer* _Buffer);
    void SetRWParticleBuffer(CStructuredBuffer* _Buffer) { m_RWBuffer = _Buffer; }
    void SetModuleData(CStructuredBuffer* _Buffer) { m_ModuleData = _Buffer; }
    void SetNoiseTexture(Ptr<CTexture> _pTex) 
    { 
        assert(nullptr != _pTex);
        m_NoiseTex = _pTex; 
        m_Const.arrV2[0] = Vec2(m_NoiseTex->Width(), m_NoiseTex->Height());
    }

    void SetParticleObjectPos(Vec3 _vPos)
    {
        m_Const.arrV4[0] = _vPos;
    }

public:
    virtual void UpdateData() override;
    virtual void Clear() override;

public:
    CParticleUpdateShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup);
    ~CParticleUpdateShader();
};


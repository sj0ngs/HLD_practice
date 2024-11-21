#pragma once
#include "CRenderComponent.h"

#include "CParticleUpdateShader.h"

class CStructuredBuffer;

class CParticleSystem :
    public CRenderComponent
{
private:
    CStructuredBuffer*          m_ParticleBuffer;
    CStructuredBuffer*          m_RWBuffer;
    CStructuredBuffer*          m_ModuleDataBuffer;
    tParticleModule             m_ModuleData;
    
    Ptr<CParticleUpdateShader>  m_UpdateCS;

    float                       m_fAccTime;

    wstring                     m_strRelativePath;

public:
    const tParticleModule& GetModuleData() const { return m_ModuleData; }
    void SetModuleData(tParticleModule& _ModuleData);

    void SetBufferMaxCount(UINT _MaxCount);
    
public:
    virtual void finaltick() override;
    virtual void render() override;

    CLONE(CParticleSystem);

public:
    virtual void SaveToLevelFile(FILE* _pFile);
    virtual void LoadFromLevelFile(FILE* _pFile);

public:
    CParticleSystem();
    CParticleSystem(const CParticleSystem& _Origin);
    ~CParticleSystem();

public:
    void Save(const wstring& _strPath);
    void Load(const wstring& _strPath);
};


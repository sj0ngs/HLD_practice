#pragma once
#include "CRes.h"

class CMesh :
    public CRes
{
private:
    ComPtr<ID3D11Buffer>    m_VB;	// 정점 정보 버퍼
    D3D11_BUFFER_DESC       m_tVBDesc;
    UINT                    m_VtxCount;

    ComPtr<ID3D11Buffer>    m_IB;	// 인덱스 버퍼
    D3D11_BUFFER_DESC       m_tIBDesc;
    UINT                    m_IdxCount;

    void*                   m_pVtxSys;
    void*                   m_pIdxSys;

public:
    void Create(void* _VtxSystem, UINT _iVtxCount, void* _IdxSystem, UINT _iIdxCount);

private:
    virtual int Load(const wstring& _strPath) { return S_OK; }

public:
    virtual int Save(const wstring& _strRelativePath) { return S_OK; }

    void render();
    void render_particle(UINT _iParticleCount);

private:
    virtual void UpdateData() override;

public:
    CMesh(bool _bEngine = false);
    ~CMesh();
};


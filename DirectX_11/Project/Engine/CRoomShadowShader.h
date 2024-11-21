#pragma once
#include "CComputeShader.h"

#include "ptr.h"
#include "CTexture.h"

class CRoomShadowShader :
    public CComputeShader
{
private:
    Ptr<CTexture> m_pDrawTex;
    Ptr<CTexture> m_pLayer1Tex;
    Ptr<CTexture> m_pShadowTex;

    Vec2    m_vShadowPos;
    int     m_iEraseMode;

public: 
    void SetDrawTex(Ptr<CTexture>& _pTex) { m_pDrawTex = _pTex; }
    void SetLayer1Tex(Ptr<CTexture>& _pTex) { m_pLayer1Tex = _pTex; }
    void SetShadowTex(Ptr<CTexture>& _pTex) { m_pShadowTex = _pTex; }

    void SetShadowPos(Vec2 _vPos) { m_vShadowPos = _vPos; }
    void SetEraseMode(int _iMode) { m_iEraseMode = _iMode; }

public:
    virtual void UpdateData() override;
    virtual void Clear() override;

public:
    CRoomShadowShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup);
    ~CRoomShadowShader();
};


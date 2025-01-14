#pragma once
#include "CComputeShader.h"

#include "ptr.h"
#include "CTexture.h"

class CSetColorShader :
    public CComputeShader
{
private:
    Ptr<CTexture>       m_OutTex;

public:
    void SetTargetTexture(Ptr<CTexture> _Tex) { m_OutTex = _Tex; }
    void SetColor(Vec3 _RGB) { m_Const.arrV4[0] = _RGB; }

public:
    virtual void UpdateData() override;
    virtual void Clear() override;

public:
    CSetColorShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup);
    ~CSetColorShader();
};


#pragma once
#include "CComputeShader.h"

#include "ptr.h"
#include "CTexture.h"

class CStructuredBuffer;
class CRoomCollisionShader :
    public CComputeShader
{
private:
    CStructuredBuffer* m_pCollisionBuffer;  // 충돌 결과 버퍼

    Ptr<CTexture> m_pLayer1Tex;
    Ptr<CTexture> m_pLayer2Tex;

    Vec2    m_vObjPos;
    float   m_fObjSize;   // 원 모양으로 체크

    Vec2    m_vRoomSize;

public:
    void SetCollisionBuffer(CStructuredBuffer* _pBuff) { m_pCollisionBuffer = _pBuff; }
    void SetLayer1Tex(Ptr<CTexture>& _pTex) { m_pLayer1Tex = _pTex; }
    void SetLayer2Tex(Ptr<CTexture>& _pTex) { m_pLayer2Tex = _pTex; }

    void SetObjPos(Vec2 _vPos) { m_vObjPos = _vPos; }
    void SetObjSize(float _fSize) { m_fObjSize = _fSize; }
    void SetRoomSize(Vec2 _vSize) { m_vRoomSize = _vSize; }

public:
    virtual void UpdateData() override;
    virtual void Clear() override;

public:
    CRoomCollisionShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup);
    ~CRoomCollisionShader();
};


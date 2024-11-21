#pragma once
#include "CComputeShader.h"

#include "ptr.h"
#include "CTexture.h"

class CStructuredBuffer;
class CCollider2D;

class CObstacleMapShader :
    public CComputeShader
{
private:
    Ptr<CTexture> m_pMapTex;
    Ptr<CTexture> m_pEdgeTex;
    Ptr<CTexture> m_pWallTex;

    Vec2 m_vRoomPos;
    Vec2 m_vRoomSize;
    Vec2 m_vCollPos;
    Vec2 m_vCollSize;

    bool m_bColliderDraw;

public:
    void SetDrawTex(Ptr<CTexture> _pTex) { m_pMapTex = _pTex; }
    void SetEdgeTex(Ptr<CTexture> _pTex) { m_pEdgeTex = _pTex; }
    void SetWallTex(Ptr<CTexture> _pTex) { m_pWallTex = _pTex; }
    void SetRoomPos(Vec2 _vPos) { m_vRoomPos = _vPos; }
    void SetRoomSize(Vec2 _vScale) { m_vRoomSize = _vScale; }
    void SetColliderPos(Vec2 _vPos) { m_vCollPos = _vPos; }
    void SetColliderSize(Vec2 _vScale) { m_vCollSize = _vScale; }
    void SetColliderDraw(bool _bSet) { m_bColliderDraw = _bSet; }

public:
    virtual void UpdateData() override;
    virtual void Clear() override;

public:
    CObstacleMapShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup);
    ~CObstacleMapShader();
};


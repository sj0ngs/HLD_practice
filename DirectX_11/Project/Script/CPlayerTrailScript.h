#pragma once
#include <Engine\CScript.h>
#include <Engine\CAnim2D.h>

class CPlayerTrailScript :
    public CScript
{
private:
    tAnim2DFrm  m_tFrm;
    Vec2        m_vBackSize;
    Ptr<CTexture> m_pTex;

    float       m_fLifeTime;
    float       m_fAccTime;

public:
    void SetFrame(tAnim2DFrm _tFrm) { m_tFrm = _tFrm; }
    void SetTex(const Ptr<CTexture>& _Tex) { m_pTex = _Tex; }
    void SetLifeTime(float _fTime) { m_fLifeTime = _fTime; }
    void SetBackSize(Vec2 _vSize) { m_vBackSize = _vSize; }
   
public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CPlayerTrailScript);
public:
    CPlayerTrailScript();
    ~CPlayerTrailScript();
};


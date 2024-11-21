#pragma once
#include <Engine\CScript.h>


class CMissileScript :
    public CScript
{
private:
    Vec3    m_vDir;     // 이동 방향
    float   m_fSpeed;   // 이동 속도

    Vec3    m_vStartPos;
    float   m_fDist;
    float   m_faccDist;

public:
    void SetDir(Vec3 _vDir) { m_vDir = _vDir; }
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }

    void SetStartPos(Vec3 _vStartPos) { m_vStartPos = _vStartPos; };
    void SetDist(float _fDist) { m_fDist = _fDist; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CMissileScript);
public:
    CMissileScript();
    ~CMissileScript();
};


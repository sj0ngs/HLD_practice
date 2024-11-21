#pragma once
#include <Engine\CAI.h>

class CMonsterScript :
    public AI
{
protected:
    Vec3            m_vFaceDir;
    CGameObject*    m_pTarget;

    float           m_fHP;
    bool            m_bHit;
    float           m_fAccTime;
    Vec3            m_vHitBoxSize;

    bool            m_bInvin;

public:
    void SetTarget(CGameObject* _pTarget) { m_pTarget = _pTarget; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void TakeDamage(CCollider2D* _pOther) override;
    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CMonsterScript);
public:
    CMonsterScript();
    CMonsterScript(UINT _iScriptType);
    ~CMonsterScript();

protected:
    void SpawnHitBox();
    void HitBlink();
};


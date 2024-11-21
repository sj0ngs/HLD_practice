#pragma once
#include "CMonsterScript.h"

class CBirdManScript :
    public CMonsterScript
{
private:
    bool        m_bFly;

    bool        m_bSpawned;

    bool        m_bSound;

public:
    void SetFaceDir(Vec3 _vFaceDir) { m_vFaceDir = _vFaceDir; }
    const Vec3& GetFaceDir() const { return m_vFaceDir; }
    CGameObject* GetTarget() { return m_pTarget; }

    void SetFly(bool _bFly) { m_bFly = _bFly; };
    bool IsFly() const { return m_bFly; }

    void SetSpawned(bool _bSpawned) { m_bSpawned = _bSpawned; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void TakeDamage(CCollider2D* _pOther) override;
    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CBirdManScript);
public:
    CBirdManScript();
    ~CBirdManScript();
};


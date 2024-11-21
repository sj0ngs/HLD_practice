#pragma once

#include "CMonsterScript.h"

class CCultBirdScript :
    public CMonsterScript
{
private:
    // ÄðÅ¸ÀÓ
    float           m_fAttackCool;
    float           m_fDodgeCool;

public:
    void SetFaceDir(Vec3 _vFaceDir) { m_vFaceDir = _vFaceDir; }
    const Vec3& GetFaceDir() const { return m_vFaceDir; }
    CGameObject* GetTarget() { return m_pTarget; }

    void SetAttackCool(float _fCool) { m_fAttackCool = _fCool; }
    bool IsAttackAvail() { return m_fAttackCool <= 0; }

    void SetDodgeCool(float _fCool) { m_fDodgeCool = _fCool; }
    bool IsDodgeAvail() { return m_fDodgeCool <= 0; }

    float GetHP() { return m_fHP; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void TakeDamage(CCollider2D* _pOther) override;
    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CCultBirdScript);
public:
    CCultBirdScript();
    ~CCultBirdScript();
};


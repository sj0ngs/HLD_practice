#pragma once
#include <Engine\CScript.h>

class CAttackScript :
    public CScript
{
private:
    float               m_fDmg;
    float               m_fGain;

    CScript*            m_pRegainInst;
    SCRIPT_DELEGATE_1   m_pRegainFunc;

    CGameObject*        m_pTarget;
    Vec3                m_vOffset;

public:
    void SetDamage(float _fDmg) { m_fDmg = _fDmg; }
    float GetDamage() { return m_fDmg; }

    void SetFollowingObj(CGameObject* _pTarget) { m_pTarget = _pTarget; }
    void SetOffset(Vec3 _vOffset) { m_vOffset = _vOffset; }

    void SetGain(float _fGain) { m_fGain = _fGain; }
    void AddDynamicRegain(float _fGain, CScript* _pInst, SCRIPT_DELEGATE_1 _pFunc) { m_fGain = _fGain; m_pRegainInst = _pInst; m_pRegainFunc = _pFunc; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CAttackScript);
public:
    CAttackScript();
    ~CAttackScript();
};


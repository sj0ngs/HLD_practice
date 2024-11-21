#pragma once
#include <Engine\CState.h>
#include "CPlayerIdle.h"

class CPlayerAttack :
    public CState
{
private:
    EFACE_DIR   m_eFaceDir;
    CAnim2D*    m_pCurAnim;

    bool        m_bComboOn;
    bool        m_bAttackEnd;
    bool        m_bMoveEnd;

    Vec3        m_vecEnterDir;
    float       m_fMoveSpeed;
    
    bool        m_bChargeAttack;
    bool        m_bWaitEnd;

    float       m_fAccTime;
    float       m_fWaitTime;

    bool        m_bAttack1;
    bool        m_bAttack2;
    bool        m_bAttack3;

    int         m_iAttackCount;

    // 공격 충돌체 설정
    Vec3        m_vAttackOffset;
    Vec3        m_vAttackSize;
    float       m_fAttackLife;

public:
    CPlayerAttack();
    ~CPlayerAttack();

public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void SetAttackDir();
    void SetChargeAttackDir();
    void BasicAttack();
    void ChargeAttack();
    void DetectHold();
    void Attack();  // 공격용 충돌 오브젝트 생성
};


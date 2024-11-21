#pragma once
#include <Engine\CState.h>

class CGhostBeamBirdAttack :
    public CState
{
public:
    CAnim2D*    m_pCurAnim;
    bool        m_bIsSlam;

    CGameObject* m_pChargeEffect;

    bool        m_bEnd;

    bool        m_bShoot;

public:
    CGhostBeamBirdAttack();
    ~CGhostBeamBirdAttack();

    CLONE(CGhostBeamBirdAttack);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void Slam();
    void Invoke();
};


#pragma once
#include <Engine\CState.h>

class CAnim2D;
class CCultBirdAttack :
    public CState
{
public:
    CAnim2D* m_pCurAnim;
    bool    m_bEnd;

public:
    CCultBirdAttack();
    ~CCultBirdAttack();

    CLONE(CCultBirdAttack);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void Attack();
};


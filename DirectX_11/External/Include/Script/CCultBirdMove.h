#pragma once
#include <Engine\CState.h>

class CCultBirdMove :
    public CState
{
private:
    float   m_fMoveSpeed;
    float   m_fAttackRadius;
    float   m_fDodgeRadius;

public:
    CCultBirdMove();
    ~CCultBirdMove();

    CLONE(CCultBirdMove);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void WatchPlayer();
};


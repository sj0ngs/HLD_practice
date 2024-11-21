#pragma once
#include <Engine\CState.h>

class CGhostBeamBirdWalk :
    public CState
{
private:
    float       m_fWalkSpeed;

    float       m_fWalkTime;
    float       m_fAccWalkTime;
public:
    CGhostBeamBirdWalk();
    ~CGhostBeamBirdWalk();

    CLONE(CGhostBeamBirdWalk);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


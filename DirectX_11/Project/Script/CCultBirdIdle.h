#pragma once
#include <Engine\CState.h>

class CCultBirdIdle :
    public CState
{
public:
    float       m_fDetectRadius;

public:
    CCultBirdIdle();
    ~CCultBirdIdle();

    CLONE(CCultBirdIdle);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


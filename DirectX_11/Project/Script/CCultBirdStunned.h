#pragma once
#include <Engine\CState.h>

class CAnim2D;
class CCultBirdStunned :
    public CState
{
private:
    CAnim2D* m_pCurAnim;

public:
    CCultBirdStunned();
    ~CCultBirdStunned();

    CLONE(CCultBirdStunned);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


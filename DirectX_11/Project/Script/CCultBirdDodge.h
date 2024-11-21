#pragma once
#include <Engine\CState.h>

class CAnim2D;
class CCultBirdDodge :
    public CState
{
private:
    float       m_fDodgeSpeed;
    CAnim2D*    m_pCurAnim;

public:
    CCultBirdDodge();
    ~CCultBirdDodge();

    CLONE(CCultBirdDodge);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


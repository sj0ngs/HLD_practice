#pragma once
#include <Engine\CState.h>

class CPlayerStunned :
    public CState
{
private:
    CAnim2D*    m_pCurAnim;

public:
    CPlayerStunned();
    ~CPlayerStunned();

public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


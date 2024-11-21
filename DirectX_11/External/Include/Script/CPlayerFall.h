#pragma once
#include <Engine\CState.h>

class CPlayerFall :
    public CState
{
private:
    CAnim2D* m_pCurAnim;

    float m_fFallTime;
    float m_fAccFallTime;
    float m_fFallSpeed;

public:
    CPlayerFall();
    ~CPlayerFall();

public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


#pragma once
#include <Engine\CState.h>

class CPlayerHeal :
    public CState
{
private:
    CAnim2D*    m_pCurAnim;
    bool        m_bHealed;

public:
    CPlayerHeal();
    ~CPlayerHeal();

public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


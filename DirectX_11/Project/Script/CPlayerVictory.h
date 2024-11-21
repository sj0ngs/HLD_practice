#pragma once
#include <Engine\CState.h>

class CPlayerVictory :
    public CState
{
private:
    CAnim2D* m_pCurAnim;

public:
    CPlayerVictory();
    ~CPlayerVictory();

public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


#pragma once
#include <Engine\CState.h>

class CPlayerMove :
    public CState
{
private:
    KEY m_eEntered;

public:
    CPlayerMove();
    ~CPlayerMove();

public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


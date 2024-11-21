#pragma once
#include <Engine\CState.h>

class CJerkPopeIdle :
    public CState
{
public:
    float       m_fDetectRadius;
    bool        m_bDetected;

public:
    CJerkPopeIdle();
    ~CJerkPopeIdle();

    CLONE(CJerkPopeIdle);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


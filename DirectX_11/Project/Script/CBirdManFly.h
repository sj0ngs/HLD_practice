#pragma once
#include <Engine\CState.h>

class CAnim2D;
class CBirdManFly :
    public CState
{
public:
    float       m_fFlySpeed;
    CAnim2D*    m_pCurAnim;

    float       m_fFlyTime;
    float       m_fAccFlyTime;

    Vec3        m_vEnterPos;

    float       m_fHeight;

public:
    CBirdManFly();
    ~CBirdManFly();

    CLONE(CBirdManFly);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void SetFlyAnim();
    void WatchPlayer();
};


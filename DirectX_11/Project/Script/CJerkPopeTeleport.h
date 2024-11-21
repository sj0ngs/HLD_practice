#pragma once
#include <Engine\CState.h>

class CAnim2D;
class CJerkPopeTeleport :
    public CState
{
private:
    CAnim2D*    m_pCurAnim;
    bool        m_bTeleport;
    int         m_iBtwFrm;

    Vec3        m_vMoveDir;
    float       m_fSpeed;
    float       m_fTimePerFrame;

public:
    CJerkPopeTeleport();
    ~CJerkPopeTeleport();

    CLONE(CJerkPopeTeleport);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void SetTeleportPoint();
    void Move();
};


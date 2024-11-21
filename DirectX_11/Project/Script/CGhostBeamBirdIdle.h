#pragma once
#include <Engine\CState.h>

class CGhostBeamBirdIdle :
    public CState
{
public:
    float       m_fDetectRadius;
    CAnim2D*    m_pCurAnim;
    bool        m_bDetected;

public:
    CGhostBeamBirdIdle();
    ~CGhostBeamBirdIdle();

    CLONE(CGhostBeamBirdIdle);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void SetRandomFaceDir();
    void SetStandAnim();
};


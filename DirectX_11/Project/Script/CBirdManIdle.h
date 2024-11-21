#pragma once
#include <Engine\CState.h>

class CAnim2D;
class CBirdManIdle :
    public CState
{
public:
    float       m_fWalkSpeed;
    float       m_fDetectRadius;
    float       m_fDetectAngle;
    CAnim2D*    m_pCurAnim;

    float       m_fWalkTime;
    float       m_fAccWalkTime;
    float       m_fStandTime;
    float       m_fAccStandTime;

    bool        m_bWalk;

public:
    CBirdManIdle();
    ~CBirdManIdle();

    CLONE(CBirdManIdle);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void SetRandomFaceDir();
    void SetStandAnim();
    void SetWalkAnim();
};


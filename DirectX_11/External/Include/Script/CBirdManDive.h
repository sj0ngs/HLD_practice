#pragma once
#include <Engine\CState.h>

class CAnim2D;
class CBirdManDive :
    public CState
{
public:
    float       m_fDiveSpeed;
    CAnim2D*    m_pCurAnim;

    float       m_fDiveTime;
    float       m_fAccDiveTime;

    float       m_fHeight;
    float       m_fDist;

    CGameObject* m_pAttack;

public:
    CBirdManDive();
    ~CBirdManDive();

    CLONE(CBirdManDive);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};


#pragma once
#include <Engine\CScript.h>

class CGameObject;
class CAimScript :
    public CScript
{
private:
    CGameObject*    m_pPlayer;
    float           m_fDist;

public:
    void SetDistance(float _fDist) { m_fDist = _fDist; }

    float GetDistance() const { return m_fDist; }

public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CAimScript);

public:
    CAimScript();
    ~CAimScript();
};


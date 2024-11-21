#pragma once
#include <Engine/CScript.h>

class CBackGroundScript :
    public CScript
{
private:
    CGameObject* m_pCamObj;
    Vector3 m_vCamPrevPos;

public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CBackGroundScript);
public:
    CBackGroundScript();
    CBackGroundScript(const CBackGroundScript& _pOrigin);
    ~CBackGroundScript();
};


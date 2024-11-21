#pragma once
#include <Engine\CScript.h>

class CSpectatorScript :
    public CScript
{
private:
    bool m_bRun;
    bool m_bTurned;

    float m_fAccTime; 
    float m_fTime;

public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CSpectatorScript);
public:
    CSpectatorScript();
    ~CSpectatorScript();

public:
    void Runaway();
};


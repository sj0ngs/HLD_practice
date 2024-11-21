#pragma once
#include <Engine\CScript.h>

class CLevel1Script :
    public CScript
{
private:
public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CLevel1Script);
public:
    CLevel1Script();
    ~CLevel1Script();

private:
    void SavePlayerData();
};


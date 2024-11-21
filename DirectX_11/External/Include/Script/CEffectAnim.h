#pragma once
#include <Engine\CScript.h>

class CEffectAnim :
    public CScript
{
private:

public:

public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CEffectAnim);
public:
    CEffectAnim();
    ~CEffectAnim();
};


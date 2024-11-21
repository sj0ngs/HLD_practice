#pragma once
#include <Engine\CScript.h>

class CEffectAnimScript :
    public CScript
{
private:
    bool    m_bRepeat;

public:
    void SetRepeat(bool _bRepeat) { m_bRepeat = _bRepeat; }

public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CEffectAnimScript);
public:
    CEffectAnimScript();
    ~CEffectAnimScript();
};


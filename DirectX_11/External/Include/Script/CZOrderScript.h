#pragma once
#include <Engine\CScript.h>

class CZOrderScript :
    public CScript
{
private:
    bool m_bOrderingOn;

public:
    void SetOrdering(bool _bSet) { m_bOrderingOn = _bSet; }

public:
    virtual void tick() override;

    CLONE(CZOrderScript);
public:
    CZOrderScript();
    ~CZOrderScript();
};


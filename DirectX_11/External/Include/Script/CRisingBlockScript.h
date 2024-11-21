#pragma once
#include <Engine\CScript.h>

class CRisingBlockScript :
    public CScript
{
private:

public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CRisingBlockScript);
public:
    CRisingBlockScript();
    ~CRisingBlockScript();

public:
    void RiseBlock();
    void DropBlock();
};


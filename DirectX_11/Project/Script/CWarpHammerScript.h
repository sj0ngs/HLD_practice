#pragma once
#include <Engine\CScript.h>

class CWarpHammerScript :
    public CScript
{
private:
public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CWarpHammerScript);
public:
    CWarpHammerScript();
    ~CWarpHammerScript();
};


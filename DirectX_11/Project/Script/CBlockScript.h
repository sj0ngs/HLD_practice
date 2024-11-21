#pragma once
#include <Engine\CScript.h>

class CBlockScript :
    public CScript
{
private:
    float m_fPush;

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CBlockScript);
public:
    CBlockScript();
    ~CBlockScript();

private:
    void BlockObj(CCollider2D* _Other);
};


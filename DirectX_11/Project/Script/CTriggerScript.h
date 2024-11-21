#pragma once
#include <Engine\CScript.h>

class CTriggerScript :
    public CScript
{
private:
    bool    m_bLevelChange;
    float   m_fAccTime;
    float   m_fTime;

    Ptr<CSound> m_pBGM;

public:
    virtual void begin() override;
    virtual void end() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CTriggerScript);
public:
    CTriggerScript();
    ~CTriggerScript();
};


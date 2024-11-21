#pragma once
#include <Engine/CScript.h>

class CHitDetectorScript :
    public CScript
{
private:
    AI* m_pOwnerScript;

public:
    void SetOwnerAI(AI* _pScript) { m_pOwnerScript = _pScript; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CHitDetectorScript);
public:
    CHitDetectorScript();
    CHitDetectorScript(const CHitDetectorScript*& _pOrigin);
    ~CHitDetectorScript();

};


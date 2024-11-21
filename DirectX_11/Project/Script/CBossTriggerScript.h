#pragma once
#include <Engine\CScript.h>

class CJerkPopeScript;
class CPlayerScript;
class CBossTriggerScript :
    public CScript
{
private:
    bool m_bPlayerEnter;
    bool m_bBossClear;
    Vec4 m_vLetterBoxCol;

    CJerkPopeScript* m_pBoss;
    CPlayerScript* m_pPlayer;

    bool  m_bIntroEnd;
    float m_fAccTime;
    float m_fIntroTime;
    float m_fOutroTime;
    bool m_bOutroEnd;

    vector<CGameObject*> m_vecRisingBlocks;
    vector<CGameObject*> m_vecSpectators;

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CBossTriggerScript);
public:
    CBossTriggerScript();
    ~CBossTriggerScript();
};


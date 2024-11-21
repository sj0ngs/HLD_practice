#pragma once
#include <Engine\CState.h>

class CAnim2D;
class CJerkPopeAttack :
    public CState
{
private:
    CAnim2D*    m_pCurAnim;
    int         m_iAttackType;  // 0 : trace, 1 : tile
    int         m_iTileType;    // 0 : x(vertical), 1 : t(horizental)

    bool        m_bAttackAnimEnd;
    bool        m_bAttackEnd;

    int         m_iTraceAttack;
    float       m_fAccTime;
    float       m_fTraceTime;

    int         m_bPrevRow;
    int         m_bPrevCol;

    bool        m_bCast;

public:
    CJerkPopeAttack();
    ~CJerkPopeAttack();

    CLONE(CJerkPopeAttack);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void TraceAttack();
    void TileAttackP1();
    void TileAttackP2();
};


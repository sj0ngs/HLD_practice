#pragma once
//#include <Engine\CAI.h>
#include "CMonsterScript.h"

class CJerkPopeAttack;
struct JerkTile
{
    Vec3 vPos;
    bool bUse = false;
};

class CJerkPopeScript :
    public CMonsterScript
{
private:
    JerkTile        m_matTile[6][10];

    // ÄðÅ¸ÀÓ
    float           m_fAttackCool;
    int             m_iTraceAttack;
    int             m_iTileAttack;
    bool            m_bMoveCenter;

    bool            m_bAttackAvail;
    bool            m_bPhase2;
    bool            m_bInvoke;

    bool            m_bSound;

    int         m_iGridRow;
    int         m_iGridCol;

public:
    void SetFaceDir(Vec3 _vFaceDir) { m_vFaceDir = _vFaceDir; }
    const Vec3& GetFaceDir() const { return m_vFaceDir; }
    CGameObject* GetTarget() { return m_pTarget; }

    float GetHP() { return m_fHP; }

    void SetAttackCool(float _bTime) { m_fAttackCool = _bTime; m_bAttackAvail = false; }
    bool IsAttackAvail() { return m_bAttackAvail; }
    bool IsPhase2() { return m_bPhase2; }

    int GetTraceAttack() { return m_iTraceAttack; }
    int GetTileAttack() { return m_iTileAttack; }

    void SetTraceAttack(int _iAtk) { m_iTraceAttack = _iAtk; }
    void SetTileAttack(int _iAtk) { m_iTileAttack = _iAtk; }

    void SetMoveCenter(bool _bMove) { m_bMoveCenter = _bMove; }
    bool GetMoveCenter() { return m_bMoveCenter; }

    bool GetInvincibility() { return m_bInvin; }
    void SetInvincibility(bool _bInvin) { m_bInvin = _bInvin; }

    void ClearMatrix();

    int GetGridRow() { return m_iGridRow; }
    int GetGridCol() { return m_iGridCol; }

    void Invoke() { m_bInvoke = true; }
    bool IsInvoke() { return m_bInvoke; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CJerkPopeScript);
public:
    CJerkPopeScript();
    ~CJerkPopeScript();

    friend class CJerkPopeAttack;
    friend class CJerkPopeTeleport;
    friend class CJerkPopeSpawn;

private:
    void MakeGridMap(Vec3 _vCenterPos, int _iRow, int _iCol, float _fTileSize);
};


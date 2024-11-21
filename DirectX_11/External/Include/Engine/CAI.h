#pragma once
#include "CScript.h"

class CState;
class AI :
    public CScript
{
private:
    map<wstring, CState*>   m_mapState;
    CState* m_pCurState;

    bool    m_bIsDead;

public:
    CState* GetCurState() const { return m_pCurState; }
    void SetDead(bool _bSet) { m_bIsDead = _bSet; }
    bool IsDead() { return m_bIsDead; }

public:
    AI(UINT _iScriptType);
    AI(const AI& _pOrigin);
    ~AI();

public:
    virtual void tick() override;
    virtual void TakeDamage(CCollider2D* _pOther) {};

public:
    void AddState(const wstring& _strKey, CState* _pState);
    CState* FindState(const wstring& _strKey);

    void ChangeState(const wstring& _strKey);
};
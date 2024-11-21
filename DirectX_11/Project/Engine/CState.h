#pragma once

#include "CGameObject.h"
#include "CAI.h"

class CState
{
private:
    AI* m_pOwnerAI;

public:
    AI* GetOwnerAI() const { return m_pOwnerAI; }
    CGameObject* GetOwnerObj() const { return m_pOwnerAI->GetOwner(); }

public:
    CState();
    ~CState();

    CLONE(CState);
public:
    virtual void tick() {};
    virtual void Enter() {};
    virtual void Exit() {};

public:
    void ChangeState(const wchar_t* _pStateName);

    friend class AI;
};


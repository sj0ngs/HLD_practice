#include "pch.h"
#include "CAI.h"

#include "CState.h"

AI::AI(UINT _iScriptType) :
    CScript(_iScriptType),
    m_pCurState(nullptr),
    m_bIsDead(false)
{
}

AI::AI(const AI& _pOrigin)  :
    CScript(_pOrigin.GetScriptType()),
    m_pCurState(nullptr),
    m_bIsDead(_pOrigin.m_bIsDead)
{
    wstring strCurState;

    for (const auto& state : _pOrigin.m_mapState)
    {
        CState* pState = state.second->Clone();
        AddState(state.first, pState);

        if (state.second == _pOrigin.m_pCurState)
            strCurState = state.first;
    }

    const map<wstring, CState*>::iterator iter = m_mapState.find(strCurState);

    if (iter != m_mapState.end())
    {
        m_pCurState = iter->second;
    }
}

AI::~AI()
{
    Safe_Del_Map(m_mapState);
}

void AI::tick()
{
    m_pCurState->tick();
}

void AI::AddState(const wstring& _strKey, CState* _pState)
{
    _pState->m_pOwnerAI = this;
    m_mapState.insert(make_pair(_strKey, _pState));
}

CState* AI::FindState(const wstring& _strKey)
{
    map<wstring, CState*>::iterator iter = m_mapState.find(_strKey);

    if (iter != m_mapState.end())
        return iter->second;

    return nullptr;
}

void AI::ChangeState(const wstring& _strKey)
{
    CState* pNextState = FindState(_strKey);
    assert(pNextState);

    if (nullptr != m_pCurState)
        m_pCurState->Exit();

    m_pCurState = pNextState;

    pNextState->Enter();
}
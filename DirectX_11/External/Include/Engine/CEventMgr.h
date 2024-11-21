#pragma once

class CGameObject;

class CEventMgr :
    public CSingleton<CEventMgr>
{
    SINGLE(CEventMgr);
private:
    vector<tEvent>          m_vecEvent;
    vector<CGameObject*>    m_vecGC;

    bool                    m_bLevelChanged;

public:
    bool IsLevelChanged() const { return m_bLevelChanged; }

public:
    void tick();

public:
    void AddEvent(const tEvent& _evn) { m_vecEvent.push_back(_evn); }

private:
    void GC_Clear();
};


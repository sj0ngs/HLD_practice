#pragma once
#include "CSingleton.h"

class CLevel;
class CGameObject;

class CLevelMgr :
    public CSingleton<CLevelMgr>
{
    SINGLE(CLevelMgr);
private:
    CLevel*                 m_pCurLevel;
    map<wstring, CLevel*>   m_mapLevel;
    bool                    m_bKeepPlay;

public:
    CLevel* GetCurLevel() const { return m_pCurLevel; }
    CLevel* GetLevel(const wstring& _strLevel) const
    {
        map<wstring, CLevel*>::const_iterator iter = m_mapLevel.find(_strLevel);
        assert(m_mapLevel.end() != iter);
        return iter->second;
    }

    CGameObject* FindObjectByName(const wstring& _strName);
    void FindObjectByName(const wstring& _strName, _Inout_ vector<CGameObject*>& _vec);

    void ChangeLevel(CLevel* _pNextLevel);
    int GetLayerIdx(const wstring& _strLayer);

public:
    void init();
    void tick();
};


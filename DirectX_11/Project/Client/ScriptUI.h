#pragma once
#include "ComponentUI.h"

#include <Engine\CScript.h>

class ScriptUI :
    public ComponentUI
{
private:
    CScript*    m_pTargetScript;
    string      m_strScriptName;
    string      m_pSelectedDesc;

public:
    void SetScript(CScript* _pScript);

public:
    virtual int render_update() override;

public:
    ScriptUI();
    ~ScriptUI();

public:
    void SelectTexParam(DWORD_PTR _strKey);
};


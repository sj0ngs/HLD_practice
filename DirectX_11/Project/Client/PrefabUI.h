#pragma once
#include "ResUI.h"

class ComponentUI;
class ScriptUI;
class PrefabUI :
    public ResUI
{
private:
    ComponentUI*        m_arrComUI[(UINT)COMPONENT_TYPE::END];
    vector<ScriptUI*>   m_vecScriptUI;

    string  m_arrLayer[MAX_LAYER];
    int     m_iLayerSelected;

public:
    virtual int render_update() override;
    virtual void SetTargetRes(Ptr<CRes> _Res) override;

public:
    PrefabUI();
    ~PrefabUI();

private:
    void ClearTargetObject();
    void ClearScriptUI();
};


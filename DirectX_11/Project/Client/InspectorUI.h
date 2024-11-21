#pragma once
#include "UI.h"

class CGameObject;
class TransformUI;
class MeshRenderUI;
class ComponentUI;
class ResUI;
class ScriptUI;

#include <Engine\ptr.h>
#include <Engine\CRes.h>

class InspectorUI :
    public UI
{
private:
    CGameObject*        m_pTargetObject;
    Ptr<CRes>           m_pTargetRes;

    vector<ScriptUI*>   m_vecScriptUI;

    ComponentUI*        m_arrComUI[(UINT)COMPONENT_TYPE::END];
    ResUI*              m_arrResUI[(UINT)RES_TYPE::END];

    string              m_arrLayer[MAX_LAYER];

public:
    void SetTargetObject(CGameObject* _pTarget);
    void SetTargetRes(Ptr<CRes> _Res);

    CGameObject* GetTargetObject() { return m_pTargetObject; }

public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update() override;

private:
    void ClearTargetObject();
    void ClearTargetRes();
    void ClearScriptUI();

public:
    InspectorUI();
    ~InspectorUI();
};


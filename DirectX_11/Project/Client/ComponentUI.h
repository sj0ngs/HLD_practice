#pragma once
#include "UI.h"

#include <Engine\CGameObject.h>

#include <Engine\ptr.h>
#include <Engine\CRes.h>

class ComponentUI :
    public UI
{
private:
    CGameObject*    m_pTarget;

    const COMPONENT_TYPE  m_Type;

public:
    CGameObject* GetTarget() { IsValid(m_pTarget); return m_pTarget; }
    void SetTargetObject(CGameObject* _pTarget);

    COMPONENT_TYPE GetType() const { return m_Type; }

    void GetResKey(Ptr<CRes> _Res, char* _Buff, size_t _BuffSize);

public:
    virtual int render_update() override;
    
public:
    ComponentUI(const string& _Name, COMPONENT_TYPE _Type);
    ~ComponentUI();
};


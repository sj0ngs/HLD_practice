#pragma once
#include "UI.h"

#include <Engine\ptr.h>
#include <Engine\CRes.h>

class ResUI :
    public UI
{
private:
    Ptr<CRes>   m_TargetRes;
    const RES_TYPE    m_ResType;

public:
    virtual void SetTargetRes(Ptr<CRes> _Res) { m_TargetRes = _Res; }
    Ptr<CRes> GetTargetRes() const { return m_TargetRes; }

public:
    virtual int render_update() override;

public:
    ResUI(const string& _strName, RES_TYPE _Type);
    ~ResUI();
};


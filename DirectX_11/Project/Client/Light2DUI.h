#pragma once
#include "ComponentUI.h"

class Light2DUI :
    public ComponentUI
{
private:
    string      m_arrLightType[(UINT)LIGHT_TYPE::END];

public:
    virtual int render_update() override;

public:
    Light2DUI();
    ~Light2DUI();
};


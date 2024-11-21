#pragma once
#include "ComponentUI.h"

class CamaraUI :
    public ComponentUI
{
public:
    virtual int render_update() override;

public:
    CamaraUI();
    ~CamaraUI();
};


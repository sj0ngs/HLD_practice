#pragma once
#include "ComponentUI.h"

class Collider2DUI :
    public ComponentUI
{
private:
    string      m_arrCollType[2];

public:
    virtual int render_update() override;

public:
    Collider2DUI();
    ~Collider2DUI();
};


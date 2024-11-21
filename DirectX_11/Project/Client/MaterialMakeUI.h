#pragma once
#include "UI.h"

class MaterialMakeUI :
    public UI
{
private:
    string m_strText;

public:
    MaterialMakeUI();
    ~MaterialMakeUI();

public:
    virtual void finaltick() override;
    virtual int render_update() override;
};


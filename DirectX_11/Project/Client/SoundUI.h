#pragma once
#include "ResUI.h"

class SoundUI :
    public ResUI
{
private:

public:
    virtual int render_update() override;

public:
    SoundUI();
    ~SoundUI();
};


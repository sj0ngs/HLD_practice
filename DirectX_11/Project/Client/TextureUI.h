#pragma once
#include "ResUI.h"

class TextureUI :
    public ResUI
{
private:

public:
    virtual int render_update() override;

public:
    TextureUI();
    ~TextureUI();
};


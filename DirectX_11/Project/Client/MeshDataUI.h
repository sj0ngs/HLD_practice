#pragma once
#include "ResUI.h"

class MeshDataUI :
    public ResUI
{
private:

public:
    virtual int render_update() override;

public:
    MeshDataUI();
    ~MeshDataUI();
};


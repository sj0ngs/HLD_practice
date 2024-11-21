#pragma once
#include "ComponentUI.h"

class MeshRenderUI :
    public ComponentUI
{
public:
    virtual int render_update() override;

public:
    MeshRenderUI();
    ~MeshRenderUI();

public:
    void SelectMesh(DWORD_PTR _Key);
    void SelectMaterial(DWORD_PTR _Key);
    void SelectTexture(DWORD_PTR _Key);
};


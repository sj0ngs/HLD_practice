#pragma once
#include "ResUI.h"

class MeshUI :
    public ResUI
{
private:
    Ptr<CTexture>   m_MeshPreview;

public:
    virtual int render_update() override;

public:
    virtual void SetTargetRes(Ptr<CRes> _Res) override;

public:
    MeshUI();
    ~MeshUI();
};


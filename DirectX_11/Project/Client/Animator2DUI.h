#pragma once
#include "ComponentUI.h"

class CAnim2D;

class Animator2DUI :
    public ComponentUI
{
private:
    CAnim2D*    m_pAnimSelected;

    // Preview
    UINT        m_iPreviewIdx;

public:
    virtual int render_update() override;

public:
    Animator2DUI();
    ~Animator2DUI();

private:
    void AnimPreview();
};


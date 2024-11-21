#pragma once
#include "UI.h"
class LevelEditorUI :
    public UI
{
private:
    string m_arrLayer[MAX_LAYER];
    int m_iLeftSelected;
    int m_iRightSelected;

public:
    LevelEditorUI();
    ~LevelEditorUI();

public:
    virtual int render_update() override;
};


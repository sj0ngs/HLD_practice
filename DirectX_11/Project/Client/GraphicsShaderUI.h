#pragma once
#include "ResUI.h"

class GraphicsShaderUI :
    public ResUI
{
private:
    const char* m_arrTopology[6];
    const char* m_arrRSType[(UINT)RS_TYPE::END];
    const char* m_arrDSType[(UINT)DS_TYPE::END];
    const char* m_arrBSType[(UINT)BS_TYPE::END];
    const char* m_arrDomain[(UINT)SHADER_DOMAIN::DOMAIN_UNDEFINED];

public:
    virtual int render_update() override;

public:
    GraphicsShaderUI();
    ~GraphicsShaderUI();
};


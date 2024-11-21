#include "pch.h"
#include "GraphicsShaderUI.h"

#include <Engine\CGraphicsShader.h>

GraphicsShaderUI::GraphicsShaderUI()    :
    ResUI("##GraphicsShader", RES_TYPE::GRAPHICS_SHADER),
    m_arrTopology
    {
        "Undefined",
        "Point List",
        "Line List",
        "Line Strip",
        "Triangle List",
        "Triangle Strip"
    },
    m_arrRSType
    {
        "Cull Back",
        "Cull Front",
        "Cull None",
        "Wire Frame"
    },
	m_arrDSType
    { 
        "Less",
		"Less Equal",
		"Greater",
		"Greater Equal",
		"No Write",
		"No Test, No Write" 
    },
	m_arrBSType
    {
		"Default",
		"Mask",
		"Alpah Blend",
		"One to One"
	},
	m_arrDomain
    {
		"Opaque",
		"Mask",
		"Transparent",
		"Post-Process",
		"UI"
	}
{
    SetName("GraphicsShader");
}

GraphicsShaderUI::~GraphicsShaderUI()
{
}

int GraphicsShaderUI::render_update()
{
    ResUI::render_update();

    Ptr<CGraphicsShader> pShader = (CGraphicsShader*)GetTargetRes().Get();

    ImGui::Text("Graphics Shader");
    ImGui::SameLine(120.f);
    string strName = string(pShader->GetKey().begin(), pShader->GetKey().end());
    ImGui::InputText("##GraphicsShader", (char*)strName.c_str(), strName.size(), ImGuiInputTextFlags_ReadOnly);

    ImGui::NewLine();

    ImGui::Text("Topology");
    ImGui::SameLine(120.f);

    int iTopology = (int)pShader->GetTopology();
    if (ImGui::Combo("##Topology", &iTopology, m_arrTopology, 6))
    {
        pShader->SetTopology((D3D11_PRIMITIVE_TOPOLOGY)iTopology);
    }

    ImGui::Text("Culling Mode");
    ImGui::SameLine(120.f);

    int iRSType = (int)pShader->GetRSType();
    if (ImGui::Combo("##RSType", &iRSType, m_arrRSType, (int)RS_TYPE::END))
    {
        pShader->SetRSType((RS_TYPE)iRSType);
    }
    
    ImGui::Text("Depth Mode");
    ImGui::SameLine(120.f);

    int iDSType = (int)pShader->GetDSType();
    if (ImGui::Combo("##DSType", &iDSType, m_arrDSType, (int)DS_TYPE::END))
    {
        pShader->SetDSType((DS_TYPE)iDSType);
    }

    ImGui::Text("Blend Mode");
    ImGui::SameLine(120.f);

    int iBSType = (int)pShader->GetBSType();
    if (ImGui::Combo("##BSType", &iBSType, m_arrBSType, (int)BS_TYPE::END))
    {
        pShader->SetBSType((BS_TYPE)iBSType);
    }

    ImGui::Text("Domain");
    ImGui::SameLine(120.f);

    int iDomain = (int)pShader->GetDomain();
    if (ImGui::Combo("##Domain", &iDomain, m_arrDomain, (int)SHADER_DOMAIN::DOMAIN_UNDEFINED))
    {
        pShader->SetDomain((SHADER_DOMAIN)iDomain);
    }

    return 0;
}


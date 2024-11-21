#include "pch.h"
#include "Light2DUI.h"

#include <Engine\CLight2D.h>
#include <limits>

Light2DUI::Light2DUI() :
    ComponentUI("##Light2D", COMPONENT_TYPE::LIGHT2D)
{
    SetName("Light2D");

    m_arrLightType[0] = "Directional";
    m_arrLightType[1] = "Point";
    m_arrLightType[2] = "Spot";
}

Light2DUI::~Light2DUI()
{
}

int Light2DUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    CLight2D* pLight = GetTarget()->Light2D();
    tLightInfo tInfo = pLight->GetLightInfo();
    
    // Light Type
    int iSelect = (UINT)tInfo.LightType;
    ImGui::Text("Type       ");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##Type", m_arrLightType[iSelect].c_str()))
    {
        for (int i = 0; i < (UINT)LIGHT_TYPE::END; i++)
        {
            const bool is_selected = (iSelect == i);
            if (ImGui::Selectable(m_arrLightType[i].c_str(), is_selected))
            {
                iSelect = i;
                pLight->SetLightType((LIGHT_TYPE)i);
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Light Color
    ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaPreview;

    Vec3 vColor = tInfo.Color.vDiffuse;
    ImGui::Text("Diffuse    ");
    ImGui::SameLine();
    ImGui::ColorEdit3("##Diffuse Color", vColor, misc_flags);
    pLight->SetLightDiffuse(vColor);

    vColor = tInfo.Color.vAmbient;
    ImGui::Text("Ambient    ");
    ImGui::SameLine();
    ImGui::ColorEdit3("##Ambient Color", vColor, misc_flags);
    pLight->SetLightAmbient(vColor);

    if ((UINT)LIGHT_TYPE::DIRECTIONAL < iSelect)
    {
        // Light Radius 
        float fRadius = tInfo.Radius;
        ImGui::Text("Radius     ");
        ImGui::SameLine();
        ImGui::DragFloat("##Radius", &fRadius, 1.f, 0.f, 1000.f);
        pLight->SetRadius(fRadius);

        if ((UINT)LIGHT_TYPE::SPOT == iSelect)
        {
            // Light Degree
            float fAngle = tInfo.Angle;
            ImGui::Text("Angle      ");
            ImGui::SameLine();
            ImGui::DragFloat("##Angle", &fAngle, 1.f, 0.f, 360.f);
            pLight->SetAngle(fAngle);
        }
    }

    return TRUE;
}
#include "pch.h"
#include "Collider2DUI.h"

#include <Engine\CCollider2D.h>

Collider2DUI::Collider2DUI()    :
    ComponentUI("##Collider2D", COMPONENT_TYPE::COLLIDER2D)
{
    SetName("Collider2D");

    m_arrCollType[0] = "Rectangle";
    m_arrCollType[1] = "Circle";
}

Collider2DUI::~Collider2DUI()
{
}

int Collider2DUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;
    
    CCollider2D* pColl = GetTarget()->Collider2D();

    // Collider Type
    int iSelect = (UINT)pColl->GetCollider2DType();
    ImGui::Text("Type");
    ImGui::SameLine(100.f);
    if (ImGui::BeginCombo("##Type", m_arrCollType[iSelect].c_str()))
    {
        for (int i = 0; i < 2; i++)
        {
            const bool is_selected = (iSelect == i);
            if (ImGui::Selectable(m_arrCollType[i].c_str(), is_selected))
            {
                iSelect = i;
                pColl->SetCollider2DType((COLLIDER2D_TYPE)i);
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Offset Position
    Vec2 vPos = pColl->GetOffsetPos();
    ImGui::Text("Offset Pos");
    ImGui::SameLine(100.f);
    ImGui::DragFloat2("##Offset Position", (float*)&vPos);
    pColl->SetOffsetPos(vPos);

    // Offset Scale
    if ((UINT)COLLIDER2D_TYPE::RECT == iSelect)
    {
        Vec2 vScale = pColl->GetOffsetScale();
        ImGui::Text("Offset Scale");
        ImGui::SameLine(100.f);
        ImGui::DragFloat2("##Offset Scale", (float*)&vScale);
        pColl->SetOffsetScale(vScale);
    }
    else
    {
        float fRadius = pColl->GetRadius();
        ImGui::Text("Radius");
        ImGui::SameLine(100.f);
        ImGui::DragFloat("##Radius", &fRadius, 1.f);
        pColl->SetRadius(fRadius);
    }

    // Absolute
    bool bAbsolute = pColl->IsAbsolute();
    ImGui::Text("Absolute");
    ImGui::SameLine(100.f);
    if (ImGui::Checkbox("##Absolute", &bAbsolute))
    {
        pColl->SetAbsolute(bAbsolute);
    }

    return TRUE;
}
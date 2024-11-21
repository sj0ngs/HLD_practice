#include "pch.h"
#include "MaterialMakeUI.h"

#include <Engine\CResMgr.h>

MaterialMakeUI::MaterialMakeUI()  :
    UI("##MaterialMake")
{
    SetModal(true);
}

MaterialMakeUI::~MaterialMakeUI()
{
}

void MaterialMakeUI::finaltick()
{
    UI::finaltick();
}

int MaterialMakeUI::render_update()
{
    ImGui::Text("Name : ");
    ImGui::SameLine();

    char szBuff[50] = {};
    memcpy(szBuff, m_strText.c_str(), m_strText.size());
    if (ImGui::InputText("##NewMaterialName", szBuff, 50))
        m_strText = szBuff;

    if (ImGui::Button("Make##MaterialMakeUI"))
    {
        Ptr<CMaterial> mtrl = new CMaterial();
   
        mtrl->SetName(wstring(m_strText.begin(), m_strText.end()));
        mtrl->Save(L"Material\\" + mtrl->GetName());
        m_strText = "";
        SetActive(false);
    }

    return 0;
}

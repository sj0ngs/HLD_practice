#include "pch.h"
#include "MaterialUI.h"

#include <Engine\CResMgr.h>
#include <Engine\CMaterial.h>
#include <Engine\CGraphicsShader.h>

#include "ParamUI.h"
#include "ListUI.h"

MaterialUI::MaterialUI()    :
    ResUI("##Material", RES_TYPE::MATERIAL),
    m_eSelected(TEX_END)
{
    SetName("Material");
}

MaterialUI::~MaterialUI()
{
}

int MaterialUI::render_update()
{
    ResUI::render_update();

    Ptr<CMaterial> pMtrl = (CMaterial*)GetTargetRes().Get();

    ImGui::SameLine();
    if (ImGui::Button("Save##"))
    {
        pMtrl->Save(L"material\\" + pMtrl->GetKey());
    }

    // Material Name
    ImGui::Text("Material");
    ImGui::SameLine(100.f);
    string strKey = string(pMtrl->GetKey().begin(), pMtrl->GetKey().end());
    ImGui::InputText("##MaterialName", (char*)strKey.c_str(), strKey.size(), ImGuiInputTextFlags_ReadOnly);

    // Graphics Shader
    ImGui::Text("Shader  ");
    ImGui::SameLine(100.f);

    Ptr<CGraphicsShader> pShader = pMtrl->GetShader();
    string strMaterial;
    if (nullptr != pShader)
    {
        strMaterial = string(pShader->GetKey().begin(), pShader->GetKey().end());
        ImGui::InputText("##ShaderName", (char*)strMaterial.c_str(), strMaterial.size(), ImGuiInputTextFlags_ReadOnly);
    }
    else
    {
        char szEmpty[50] = {};
        ImGui::InputText("##ShaderName", szEmpty, 50, ImGuiInputTextFlags_ReadOnly);
    }

    ImGui::SameLine();
    if (ImGui::Button("##ShaderSelectBtn", ImVec2(19, 19)))
    {
        const map<wstring, Ptr<CRes>>& mapMesh = CResMgr::GetInst()->GetResources(RES_TYPE::GRAPHICS_SHADER);

        ListUI* pList = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
        pList->SetName("Graphics Shader List");
        pList->Reset("Graphics Shader List", ImVec2(300.f, 500.f));

        for (const auto& pair : mapMesh)
        {
            pList->AddItem(string(pair.first.begin(), pair.first.end()));
        }
        pList->SetActive(true);
        pList->AddDynamic_Select(this, (UI_DELEGATE_1)&MaterialUI::SelectGraphicsShader);
    }


    ImGui::NewLine();

    if (nullptr == pShader)
        return 0;    

    const vector<tScalarParam>& vecScalarParam = pShader->GetScalarParam();
    
    if(!vecScalarParam.empty())
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.f, 1.f), "Parameters");

    for (size_t i = 0; i < vecScalarParam.size(); i++)
    {
        switch (vecScalarParam[i].eParam)
        {
        case INT_0:
            ImGui::Text("Int Param");
        case INT_1:
        case INT_2:
        case INT_3:
        {
            int data = 0;
            pMtrl->GetScalarParam(vecScalarParam[i].eParam, &data);
            if (ParamUI::Param_Int(vecScalarParam[i].strDesc, &data))
            {
                pMtrl->SetScalarParam(vecScalarParam[i].eParam, &data);
            }
        }
            break;
        case FLOAT_0:
        case FLOAT_1:
        case FLOAT_2:
        case FLOAT_3:
        {
            float data = 0.f;
            pMtrl->GetScalarParam(vecScalarParam[i].eParam, &data);
            if (ParamUI::Param_Float(vecScalarParam[i].strDesc, &data))
            {
                pMtrl->SetScalarParam(vecScalarParam[i].eParam, &data);
            }
        }
            break;
        case VEC2_0:
        case VEC2_1:
        case VEC2_2:
        case VEC2_3:
        {
            Vec2 data = {};
            pMtrl->GetScalarParam(vecScalarParam[i].eParam, &data);
            if (ParamUI::Param_Vec2(vecScalarParam[i].strDesc, &data))
            {
                pMtrl->SetScalarParam(vecScalarParam[i].eParam, &data);
            }
        }
            break;
        case VEC4_0:
        case VEC4_1:
        case VEC4_2:
        case VEC4_3:
        {
            Vec4 data = {};
            pMtrl->GetScalarParam(vecScalarParam[i].eParam, &data);
            if (ParamUI::Param_Vec4(vecScalarParam[i].strDesc, &data))
            {
                pMtrl->SetScalarParam(vecScalarParam[i].eParam, &data);
            }
        }
            break;
        case MAT_0:
        case MAT_1:
        case MAT_2:
        case MAT_3:
        {
            Matrix data = {};
            pMtrl->GetScalarParam(vecScalarParam[i].eParam, &data);
            if (ParamUI::Param_Mat(vecScalarParam[i].strDesc, &data))
            {
                pMtrl->SetScalarParam(vecScalarParam[i].eParam, &data);
            }
        }
            break;
        }
    }

    const vector<tTexParam>& vecTexParam = pShader->GetTexParam();
    for (size_t i = 0; i < vecTexParam.size(); i++)
    {  
        Ptr<CTexture> pTex = pMtrl->GetTexParam(vecTexParam[i].eParam);
        if (ParamUI::Param_Tex(vecTexParam[i].strDesc, pTex, this, (UI_DELEGATE_1)&MaterialUI::SelectTexture))
        {
            m_eSelected = vecTexParam[i].eParam;
        }
        pMtrl->SetTexParam(vecTexParam[i].eParam, pTex);
    }

    return 0;
}

void MaterialUI::SelectTexture(DWORD_PTR _Key)
{
    string strKey = (char*)_Key;

    Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstring(strKey.begin(), strKey.end()));
    Ptr<CMaterial> pMtrl = (CMaterial*)GetTargetRes().Get();
    pMtrl->SetTexParam(m_eSelected, pTex);
}

void MaterialUI::SelectGraphicsShader(DWORD_PTR _Key)
{
    string strKey = (char*)_Key;
    Ptr<CGraphicsShader> pGS = CResMgr::GetInst()->FindRes<CGraphicsShader>(wstring(strKey.begin(), strKey.end()));

    assert(nullptr != pGS);
    ((CMaterial*)GetTargetRes().Get())->SetShader(pGS);
}

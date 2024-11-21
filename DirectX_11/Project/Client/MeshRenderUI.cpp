#include "pch.h"
#include "MeshRenderUI.h"

#include <Engine\CMeshRender.h>
#include <Engine\CResMgr.h>

#include "ImGuiMgr.h"
#include "ListUI.h"
#include "TreeUI.h"
#include "ParamUI.h"

MeshRenderUI::MeshRenderUI()	:
	ComponentUI("##MeshRender", COMPONENT_TYPE::MESHRENDER)
{
    SetName("MeshRender");
}

MeshRenderUI::~MeshRenderUI()
{
}

int MeshRenderUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    char szBuff[50] = {};

    CMeshRender* pMeshRender = GetTarget()->MeshRender();
    Ptr<CMesh> pMesh = pMeshRender->GetMesh();
    Ptr<CMaterial> pMtrl = pMeshRender->GetMaterial();

    ImGui::Text("Mesh       ");
    ImGui::SameLine();
    GetResKey(pMesh.Get(), szBuff, sizeof(char) * 50);
    ImGui::InputText("##MeshName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
        if (pPayLoad)
        {
            TreeNode* pNode = (TreeNode*)pPayLoad->Data;
            CRes* pRes = (CRes*)pNode->GetData();
            if (RES_TYPE::MESH == pRes->GetType())
            {
                GetTarget()->MeshRender()->SetMesh((CMesh*)pRes);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();
    if (ImGui::Button("##MeshSelectBtn", ImVec2(19, 19)))
    {
        const map<wstring, Ptr<CRes>>& mapMesh = CResMgr::GetInst()->GetResources(RES_TYPE::MESH);

        ListUI* pList = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
        pList->Reset("Mesh List", ImVec2(300.f, 500.f));

        for (const auto& pair : mapMesh)
        {
            pList->AddItem(string(pair.first.begin(), pair.first.end()));
        }
        pList->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectMesh);
    }

    ImGui::Text("Material   ");
    ImGui::SameLine();
    GetResKey(pMtrl.Get(), szBuff, sizeof(char) * 50);
    ImGui::InputText("##MaterialName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
        if (pPayLoad)
        {
            TreeNode* pNode = (TreeNode*)pPayLoad->Data;
            CRes* pRes = (CRes*)pNode->GetData();
            if (RES_TYPE::MATERIAL == pRes->GetType())
            {
                GetTarget()->MeshRender()->SetMaterial((CMaterial*)pRes);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();
    if (ImGui::Button("##MaterialSelectBtn", ImVec2(19, 19)))
    {
        const map<wstring, Ptr<CRes>>& mapMesh = CResMgr::GetInst()->GetResources(RES_TYPE::MATERIAL);

        ListUI* pList = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
        pList->SetName("Material List");
        pList->Reset("Mesh List", ImVec2(300.f, 500.f));

        for (const auto& pair : mapMesh)
        {
            pList->AddItem(string(pair.first.begin(), pair.first.end()));
        }
        pList->SetActive(true);
        pList->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectMaterial);
    }

    bool bHasDm = pMeshRender->HasDynamciMaterial();
    if (ImGui::Checkbox("Using Dynamic Material", &bHasDm))
    {
        // 다이나믹 머티리얼을 제거
        if (bHasDm)
            pMeshRender->GetDynamicMaterial();
        // 다이나믹 머티리얼을 추가
        else if (!bHasDm)
            pMeshRender->RemoveDynamicMaterial();
    }

    if (bHasDm)
    {
        Ptr<CTexture> pTex = pMeshRender->GetDynamicMaterial()->GetTexParam(TEX_0);
        ParamUI::Param_Tex("Output Texture", pTex, this, (UI_DELEGATE_1)&MeshRenderUI::SelectTexture);
    }

    return TRUE;
}

void MeshRenderUI::SelectMesh(DWORD_PTR _Key)
{
    string strKey = (char*)_Key;

    Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(wstring(strKey.begin(), strKey.end()));
    GetTarget()->MeshRender()->SetMesh(pMesh);
}

void MeshRenderUI::SelectMaterial(DWORD_PTR _Key)
{
    string strKey = (char*)_Key;

    Ptr<CMaterial> pMaterial = CResMgr::GetInst()->FindRes<CMaterial>(wstring(strKey.begin(), strKey.end()));
    GetTarget()->MeshRender()->SetMaterial(pMaterial);
}

void MeshRenderUI::SelectTexture(DWORD_PTR _Key)
{
    string strKey = (char*)_Key;

    Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstring(strKey.begin(), strKey.end()));
    GetTarget()->MeshRender()->GetMaterial()->SetTexParam(TEX_0, pTex);
}

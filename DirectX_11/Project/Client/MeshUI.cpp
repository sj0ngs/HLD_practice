#include "pch.h"
#include "MeshUI.h"

#include <Engine\CDevice.h>
#include <Engine\CResMgr.h>
#include <Engine\CMesh.h>
#include <Engine\CLevel.h>
#include <Engine\CGameObject.h>
#include <Engine\components.h>

#include "CEditorObjMgr.h"
#include "CGameObjectEx.h"

MeshUI::MeshUI()  :
    ResUI("##Mesh", RES_TYPE::MESH)
{
    SetName("Mesh");

    m_MeshPreview = CResMgr::GetInst()->CreateTexture(L"MeshPreview", 300, 300, DXGI_FORMAT_R8G8B8A8_UNORM,
                                                        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, D3D11_USAGE_DEFAULT);
}

MeshUI::~MeshUI()
{
}

int MeshUI::render_update()
{
    ResUI::render_update();

    Ptr<CMesh> pMesh = (CMesh*)GetTargetRes().Get();

    ImGui::Text("Mesh Name");
    ImGui::SameLine();
    string strKey = string(pMesh->GetKey().begin(), pMesh->GetKey().end());
    ImGui::InputText("##MeshName", (char*)strKey.c_str(), strKey.size(), ImGuiInputTextFlags_ReadOnly);

    ImGui::NewLine();
    ImGui::Text("Mesh Preview");

    Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"SampleTexture");
    ImGui::Image(m_MeshPreview->GetSRV().Get(), ImVec2(150.f, 150.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), 
                ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f));

    return TRUE;
}

void MeshUI::SetTargetRes(Ptr<CRes> _Res)
{
    ResUI::SetTargetRes(_Res);

    if (nullptr == _Res.Get())
        return;

    Ptr<CMesh> pMesh = (CMesh*)GetTargetRes().Get();

    CGameObjectEx* pLocalObject = new CGameObjectEx;
    pLocalObject->AddComponent(new CTransform);
    pLocalObject->AddComponent(new CMeshRender);
    pLocalObject->Transform()->SetRelativeScale(Vec3(75.f, 75.f, 1.f));
    pLocalObject->MeshRender()->SetMesh(pMesh);
    pLocalObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"MeshMtrl"));

    CGameObjectEx* pLocalCamObj = CEditorObjMgr::GetInst()->GetLocalCam();
    pLocalCamObj->finaltick();

    g_transform.matView = pLocalCamObj->Camera()->GetViewMat();
    g_transform.matProj = XMMatrixPerspectiveFovLH(XM_PI / 2.f, 
                        (float)m_MeshPreview->Width() / (float)m_MeshPreview->Height(), 1.f, 10000.f);

    CDevice::GetInst()->SetViewPort(m_MeshPreview->Width(), m_MeshPreview->Height());
    Ptr<CTexture> pDSTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");
    CONTEXT->OMSetRenderTargets(1, m_MeshPreview->GetRTV().GetAddressOf(), pDSTex->GetDSV().Get());
    float arrColor[4] = { 0.1f, 0.1f, 0.1f, 1.f };
    CONTEXT->ClearRenderTargetView(m_MeshPreview->GetRTV().Get(), arrColor);
    CONTEXT->ClearDepthStencilView(pDSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    pLocalObject->finaltick();
    pLocalObject->render();

    delete pLocalObject;
    CDevice::GetInst()->SetViewPort();
    CDevice::GetInst()->OMSet();
}

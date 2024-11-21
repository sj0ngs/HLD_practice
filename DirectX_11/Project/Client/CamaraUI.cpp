#include "pch.h"
#include "CamaraUI.h"

#include <Engine/CRenderMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CCamera.h>

CamaraUI::CamaraUI() :
    ComponentUI("##Camera", COMPONENT_TYPE::CAMERA)
{
    SetName("Camera");
}

CamaraUI::~CamaraUI()
{
}

int CamaraUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    CCamera* pCam = GetTarget()->Camera();

    PROJ_TYPE ProjType = pCam->GetProjType();
    bool IsPerspective = ProjType == PROJ_TYPE::PERSPECTIVE;
    bool IsOrthographic = ProjType == PROJ_TYPE::ORTHOGRAPHIC;

    // PROJ_TYPE
    ImGui::Text("Projection Type");
    ImGui::SameLine(120);

    const char* items[2] = {};

    items[0] = "ORTHOGRAPHIC";
    items[1] = "PERSPECTIVE";

    if (ImGui::BeginCombo("##CameraProjType", items[(UINT)ProjType], 0))
    {
        for (int i = 0; i < 2; ++i)
        {
            const bool is_selected = ((UINT)ProjType == i);
            if (ImGui::Selectable(items[i], is_selected))
            {
                pCam->SetProjType((PROJ_TYPE)i);
            }
        }
        ImGui::EndCombo();
    }

    //// ī�޶� Priority
    //// CRenderMgr �κ��� ī�޶� ��� �ִ� Capacity�� �˾Ƴ���.
    //vector<CCamera*>& vecCam = CRenderMgr::GetInst()->GetRegisteredCamera();

    //ImGui::Text("Camera Priority");
    //ImGui::SameLine(120);
    //int iPriority = pCam->GetCameraIndex();
    //if (ImGui::InputInt("##CameraPriority", &iPriority))
    //{
    //    // ī�޶� �켱�������� �����ߴٸ�
    //    int CurPriority = pCam->GetCameraIndex();

    //    // �����Ϸ��� ī�޶� �켱���� ���̶� ������ ī�޶� �����ȿ� �ִ��� Ȯ�� �� ����
    //    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
    //    vector<CGameObject*> vecCamObj;
    //    pCurLevel->FindObjectByComponent(COMPONENT_TYPE::CAMERA, vecCamObj);

    //    // ������ �켱������ ������ �ִ� �ٸ� ī�޶� �ִٸ�
    //    // �����Ϸ��� ī�޶��� �켱�������� �ְ�
    //    for (size_t i = 0; i < vecCamObj.size(); ++i)
    //    {
    //        if (vecCamObj[i]->Camera()->GetCameraPriority() == iPriority)
    //        {
    //            vecCamObj[i]->Camera()->SetCameraPriority(CurPriority);
    //            break;
    //        }
    //    }

    //    // ���� ī�޶�� ���ο� �켱������ ����
    //    pCam->SetCameraPriority(iPriority);
    //}

    // Scale(Orthographic)
    ImGui::Text("Projection Scale");
    ImGui::SameLine(120);
    float Scale = pCam->GetScale();

    ImGui::BeginDisabled(IsPerspective);

    if (ImGui::DragFloat("##CameraScale", &Scale, 0.01f))
    {
        if (Scale < 0.1f)
            Scale = 0.1f;
        pCam->SetScale(Scale);
    }

    ImGui::EndDisabled();

    return TRUE;
}
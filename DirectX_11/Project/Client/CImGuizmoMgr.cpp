#include "pch.h"
#include "CImGuizmoMgr.h"

#include "ImGui/imgui.h"
#include "ImGui//ImGuizmo.h"

#include "ImGuiMgr.h"
#include "InspectorUI.h"

#include <Engine/func.h>
#include <Engine/struct.h>
#include <Engine/define.h>
#include <Engine/CRenderMgr.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>

#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>

CImGuizmoMgr::CImGuizmoMgr()    :
    m_arrView{},
    m_arrProj{},
    m_arrIdentity{},
    m_GizmoOp(GIZMO_OP::TRANSLATE),
    m_GizmoMode(GIZMO_MODE::WORLD),
    m_bIsManipulating(false)
{
}

CImGuizmoMgr::~CImGuizmoMgr()
{
}

void CImGuizmoMgr::tick()
{
    // 프레임 초기화
	ImGuizmo::BeginFrame();
	float windowWidth = (float)ImGui::GetIO().DisplaySize.x;
	float windowHeight = (float)ImGui::GetIO().DisplaySize.y;
	ImGuizmo::SetRect(0, 0, windowWidth, windowHeight);

    RenderGizmo();
}

void CImGuizmoMgr::CalcMatrix()
{
    ConvertMatrixToFloatArray(g_transform.matView, m_arrView);
    ConvertMatrixToFloatArray(g_transform.matProj, m_arrProj);
    ConvertMatrixToFloatArray(XMMatrixIdentity(), m_arrIdentity);
}

void CImGuizmoMgr::RenderGizmo()
{
    InspectorUI* pInsp = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");

    if (nullptr == pInsp)
        return;

    CGameObject* pTarget = pInsp->GetTargetObject();

    if (!IsValid(pTarget))
        return;

    CTransform* pTrans = pTarget->Transform();

    if (nullptr == pTrans)
        return;

    CalcMatrix();

    float arrTrans[16] = {};
    ConvertMatrixToFloatArray(pTrans->GetWorldMat(), arrTrans);

    ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
    //switch (m_GizmoOp)
    //{
    //case GIZMO_OP::TRANSLATE:
    //    op = ImGuizmo::TRANSLATE;
    //    break;
    //case GIZMO_OP::SCALE:
    //    op = ImGuizmo::SCALE;
    //    break;
    //case GIZMO_OP::ROTATE:
    //    op = ImGuizmo::ROTATE;
    //    break;
    //}

    ImGuizmo::MODE mode = ImGuizmo::LOCAL;
    //switch (m_GizmoMode)
    //{
    //case GIZMO_MODE::LOCAL:
    //    mode = ImGuizmo::LOCAL;
    //    break;
    //case GIZMO_MODE::WORLD:
    //    mode = ImGuizmo::WORLD;
    //    break;
    //}

    if (ImGuizmo::Manipulate(m_arrView, m_arrProj, op, mode, arrTrans, nullptr))
    {
        m_bIsManipulating = true;
        float vTrans[3] = {};  float vScale[3] = {};  float vRot[3] = {};

        ImGuizmo::DecomposeMatrixToComponents(arrTrans, vTrans, vRot, vScale);
        switch (m_GizmoOp)
        {
        case GIZMO_OP::TRANSLATE:
            pTrans->SetRelativePos(vTrans[0], vTrans[1], vTrans[2]);
            break;
        case GIZMO_OP::SCALE:
            pTrans->SetRelativeScale(vScale[0], vScale[1], vScale[2]);
            break;
        case GIZMO_OP::ROTATE:
            pTrans->SetRelativeRot(XMConvertToRadians(vRot[0]), XMConvertToRadians(vRot[1]), XMConvertToRadians(vRot[2]));
            break;
        }
    }
}

#include "pch.h"
#include "Animator2DUI.h"

#include "AnimEditorUI.h"
#include <Engine/CAnimator2D.h>
#include <Engine/CAnim2D.h>
#include <Engine/CTimeMgr.h>

Animator2DUI::Animator2DUI() :
    ComponentUI("##Animator2D", COMPONENT_TYPE::ANIMATOR2D),
    m_pAnimSelected(nullptr),
    m_iPreviewIdx(0)
{
    SetName("Animator2D");
}

Animator2DUI::~Animator2DUI()
{
}

int Animator2DUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    // 보유 애니메이션 출력
    ImGui::Text("Animation List");
    CAnimator2D* pAnimator = GetTarget()->Animator2D();
    const map<wstring, CAnim2D*>& mapAnim2D = pAnimator->GetAnims();

    if (ImGui::BeginListBox("##Animlist", ImVec2(200.f, 100.f)))
    {
        string strAnimKey;

        for (const auto& pair : mapAnim2D)
        {
            strAnimKey = string(pair.first.begin(), pair.first.end());
            const bool is_selected = (m_pAnimSelected == pair.second);

            if (ImGui::Selectable(strAnimKey.c_str(), is_selected))
            {
                m_pAnimSelected = pair.second;
                m_iPreviewIdx = 0;
            }
        }

        ImGui::EndListBox();
    }

    ImGui::SameLine();

    ImGui::BeginDisabled(nullptr == m_pAnimSelected);
    if (ImGui::Button("Edit"))
    {
        AnimEditorUI* pUI = dynamic_cast<AnimEditorUI*>(ImGuiMgr::GetInst()->FindUI("##AnimEditor"));
        assert(pUI);
        pUI->SetActive(true);
        pUI->SetAnim2D(m_pAnimSelected);
    }
    ImGui::EndDisabled();

    // 애니메이션 추가
    if (ImGui::Button("Add"))
    {
        wstring strPath;
        if (S_OK == GetLoadFilePath(L"\\animation", strPath))
        {
            pAnimator->LoadAnim(strPath);
        }
    }

    // 선택 애니메이션 삭제
    ImGui::SameLine();
    ImGui::BeginDisabled(nullptr == m_pAnimSelected);
    if (ImGui::Button("Delete"))
    {
        pAnimator->DeleteAnim(m_pAnimSelected->GetName());
        m_pAnimSelected = nullptr;
    }
    ImGui::EndDisabled();

    // 선택 애니메이션 실행
    ImGui::SameLine();
    ImGui::BeginDisabled(nullptr == m_pAnimSelected);
    if (ImGui::Button("Play"))
    {
        pAnimator->Play(m_pAnimSelected->GetName(), true);
    }
    ImGui::EndDisabled();

    if (ImGui::Button("Create Animation"))
    {
        AnimEditorUI* pUI = dynamic_cast<AnimEditorUI*>(ImGuiMgr::GetInst()->FindUI("##AnimEditor"));
        assert(pUI);
        pUI->Reset();
        pUI->SetActive(true);
    }

    ImGui::SameLine(); HelpMarker("Open AnimEditorUI");

    AnimPreview();

    return TRUE;
}

void Animator2DUI::AnimPreview()
{
    if (nullptr == m_pAnimSelected)
    {
        SetSize(0.f, 180.f);
        return;
    }

    SetSize(0.f, 500.f);
    ImGui::Separator();
    ImGui::Text("Animation Review");
    
    // 애니메이션 정보 받아오기
    const vector<tAnim2DFrm>& vecFrm = m_pAnimSelected->GetFrames();
    Vec2 vBacksize = m_pAnimSelected->GetBackSize();
    Ptr<CTexture> pTex = m_pAnimSelected->GetAtlasTex();
    Vec2 vTexSize = Vec2((float)pTex->Width(), (float)pTex->Height());

    static float m_fAccTime = 0.f;
    m_fAccTime += DT;

    if (vecFrm.size() <= m_iPreviewIdx)
        m_iPreviewIdx = 0;

    if (m_fAccTime > vecFrm[m_iPreviewIdx].fDuration)
    {
        m_fAccTime = m_fAccTime - vecFrm[m_iPreviewIdx].fDuration;

        if (vecFrm.size() <= ++m_iPreviewIdx)
        {
            m_iPreviewIdx = 0;
        }
    }

    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    float fBackAspect = (vBacksize.x * vTexSize.x) / (vBacksize.y * vTexSize.y);
    ImVec2 canvas_sz = ImVec2(300.f, 300.f / fBackAspect);
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
    Vec2 vScale = Vec2(canvas_sz.x / (vBacksize.x * vTexSize.x), canvas_sz.y / (vBacksize.y * vTexSize.y));

    // Draw border and background color
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    draw_list->PushClipRect(canvas_p0, canvas_p1, true);

    Vec2 vValue = vBacksize - vecFrm[m_iPreviewIdx].SliceUV;
    vValue /= 2.f;
    ImVec2 vDist = ImVec2((vValue.x + vecFrm[m_iPreviewIdx].Offset.x) * vTexSize.x * vScale.x,
        (vValue.y + vecFrm[m_iPreviewIdx].Offset.y) * vTexSize.y * vScale.y);

    draw_list->AddImage((void*)(pTex->GetSRV().Get()),
        ImVec2(canvas_p0.x + vDist.x, canvas_p0.y + vDist.y),
        ImVec2(canvas_p0.x + vDist.x + vecFrm[m_iPreviewIdx].SliceUV.x * vScale.x * vTexSize.x,
            canvas_p0.y + vDist.y + vecFrm[m_iPreviewIdx].SliceUV.y * vScale.y * vTexSize.y),
        ImVec2(vecFrm[m_iPreviewIdx].LeftTopUV.x, vecFrm[m_iPreviewIdx].LeftTopUV.y),
        ImVec2(vecFrm[m_iPreviewIdx].LeftTopUV.x + vecFrm[m_iPreviewIdx].SliceUV.x,
            vecFrm[m_iPreviewIdx].LeftTopUV.y + vecFrm[m_iPreviewIdx].SliceUV.y));
}

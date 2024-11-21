#include "pch.h"
#include "LevelEditorUI.h"

#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>

LevelEditorUI::LevelEditorUI() :
    UI("##LevelEditorUI"),
    m_iLeftSelected(0),
    m_iRightSelected(0)
{
    SetName("Level Editor");
    SetWindowFlags(ImGuiWindowFlags_AlwaysVerticalScrollbar);

    for (int i = 0; i < MAX_LAYER; i++)
    {
        m_arrLayer[i] = to_string(i);
    }
}

LevelEditorUI::~LevelEditorUI()
{
}

int LevelEditorUI::render_update()
{
    // 현재 레벨의 설정을 변경하는 UI
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

    if (!pCurLevel)
        return FALSE;

    // 레벨명 표시
    ImGui::Text("Current Level ");
    ImGui::SameLine();
    string strName = string(pCurLevel->GetName().begin(), pCurLevel->GetName().end());
    ImGui::InputText("##CurLevelName", (char*)strName.c_str(), strName.size(), ImGuiInputTextFlags_ReadOnly);
  
    ImGui::NewLine();
    ImGui::Text("Layer Name");
    // 레벨 레이어 설정
    for (int i = 0; i < MAX_LAYER; i++)
    {
        CLayer* pLayer = pCurLevel->GetLayer(i);
        assert(nullptr != pLayer);

        string strLayerIdx = "Layer " + to_string(i);
        ImGui::Text((char*)strLayerIdx.c_str());
        ImGui::SameLine(100.f);
        
        string strLayerName = string(pLayer->GetName().begin(), pLayer->GetName().end());
        char szBuff[50] = {};
        memcpy(szBuff, strLayerName.c_str(), strLayerName.size());

        string strTag = "##LayerName" + strLayerIdx;
        if (ImGui::InputText((char*)strTag.c_str(), szBuff, 50, 
            ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            strLayerName = szBuff;
            pLayer->SetName(wstring(strLayerName.begin(), strLayerName.end()));
        }
    }
    
    // 레벨 충돌 설정
    ImGui::NewLine();
    ImGui::Text("Collision Setting");
    const vector<pair<UINT, UINT>>& vecCollisionPair = pCurLevel->GetCollisionPair();

    // 새로운 충돌 세팅 추가
    // 왼쪽 레이어
    ImGui::SetNextItemWidth(50.f);
    if (ImGui::BeginCombo("##LeftCombo", m_arrLayer[m_iLeftSelected].c_str()))
    {
        for (int i = 0; i < MAX_LAYER; i++)
        {
            const bool is_selected = (m_iLeftSelected == i);
            if (ImGui::Selectable(m_arrLayer[i].c_str(), is_selected))
            {
                m_iLeftSelected = i;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    // 오른쪽 레이어
    ImGui::SetNextItemWidth(50.f);
    if (ImGui::BeginCombo("##RightCombo", m_arrLayer[m_iRightSelected].c_str()))
    {
        for (int i = 0; i < MAX_LAYER; i++)
        {
            const bool is_selected = (m_iRightSelected == i);
            if (ImGui::Selectable(m_arrLayer[i].c_str(), is_selected))
            {
                m_iRightSelected = i;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    if (ImGui::Button("Add##CollsisonPair"))
    {
        pCurLevel->AddCollisionPair((UINT)m_iLeftSelected, (UINT)m_iRightSelected);
    }
    
    int pairIdx = 0;
    for (const auto& pPair : vecCollisionPair)
    {
        string strPair = "Setting " + to_string(pairIdx);
        ImGui::PushID(0);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.49f, 0.04f, 0.9f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.49f, 0.04f, 0.9f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.49f, 0.04f, 0.9f, 1.f));
        ImGui::Button((char*)strPair.c_str());
        ImGui::PopStyleColor(3);
        ImGui::PopID();

        // 충돌 설정 삭제 버튼
        ImGui::SameLine();
        string strBtn = "Delete##" + strPair;
        if (ImGui::Button((char*)strBtn.c_str()))
        {
            pCurLevel->DeleteCollisionPair(pairIdx);
        }

        CLayer* pLeftLayer = pCurLevel->GetLayer(pPair.first);
        CLayer* pRightLayer = pCurLevel->GetLayer(pPair.second);

        string strLeft = to_string(pLeftLayer->GetLayerIdx()) + ":" + 
                        string(pLeftLayer->GetName().begin(), pLeftLayer->GetName().end());
        string strLabel = "##" + strLeft;
        ImGui::SetNextItemWidth(150.f);
        ImGui::InputText(strLabel.c_str(), (char*)strLeft.c_str(), strLeft.size(), ImGuiInputTextFlags_ReadOnly);
        
        ImGui::SameLine();

        string strRight = to_string(pRightLayer->GetLayerIdx()) + ":" +
                        string(pRightLayer->GetName().begin(), pRightLayer->GetName().end());
        strLabel = "##" + strRight;
        ImGui::SetNextItemWidth(150.f);
        ImGui::InputText(strLabel.c_str(), (char*)strRight.c_str(), strRight.size(), ImGuiInputTextFlags_ReadOnly);

        pairIdx++;
    }

    return TRUE;
}


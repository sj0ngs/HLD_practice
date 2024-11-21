#include "pch.h"
#include "MenuUI.h"

#include "ImGuiMgr.h"
#include "OutlinerUI.h"
#include "InspectorUI.h"

#include <Engine\CEventMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\components.h>
#include <Engine\CScript.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CMaterial.h>
#include <Engine\CTimeMgr.h>
#include <Engine\CRenderMgr.h>

#include <Script\CScriptMgr.h>

#include "CLevelSaveLoad.h"

MenuUI::MenuUI() :
    UI("##Menu")
{
    SetName("Menu");
}

MenuUI::~MenuUI()
{
}

void MenuUI::finaltick()
{
    if (!IsActive())
        return;

    render_update();

    if (KEY_PRESSED(KEY::LCTRL) && KEY_PRESSED(KEY::LSHIFT) && KEY_TAP(KEY::S))
    {
        if (S_OK == CLevelSaveLoad::SaveCurrLevel())
            MessageBox(nullptr, L"저장 성공!", L"저장 완료", MB_OK);
    }
}

int MenuUI::render_update()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Level"))
            {
                // 레벨 저장
                wstring strPath;
                if (S_OK == GetSaveFilePath(L"Level\\", strPath))
                {
                    CLevelSaveLoad::SaveLevel(strPath, CLevelMgr::GetInst()->GetCurLevel());
                }
            }

            if (ImGui::MenuItem("Load Level"))
            {
                // Level 불러오기
                wstring strPath;
                if (S_OK == GetLoadFilePath(L"Level\\", strPath))
                {
                    CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(strPath);

                    tEvent evn = {};
                    evn.Type = EVENT_TYPE::LEVEL_CHANGE;
                    evn.wParam = (DWORD_PTR)pLoadedLevel;

                    CEventMgr::GetInst()->AddEvent(evn);
                }
            }

            // 레벨 에디터 호출
            if (ImGui::MenuItem("Edit Level"))
            {
                UI* pLevelEditorUI = ImGuiMgr::GetInst()->FindUI("##LevelEditorUI");
                pLevelEditorUI->SetActive(true);
            }

            if (ImGui::MenuItem("Make New Material"))
            {
                UI* pMtrlMake = ImGuiMgr::GetInst()->FindUI("##MaterialMake");
                pMtrlMake->SetActive(true);
            }


            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Game Object"))
        {
            if (ImGui::MenuItem("Create Empty Object"))
            {
                CreateEmptyObject();
            }

            if (ImGui::BeginMenu("Add Component"))
            {
                for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
                {
                    if (ImGui::MenuItem(ToString((COMPONENT_TYPE)i)))
                    {
                        AddComponent((COMPONENT_TYPE)i);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();

            if (ImGui::BeginMenu("Add Script"))
            {
                vector<wstring> vecScripts;
                CScriptMgr::GetScriptInfo(vecScripts);

                for (size_t i = 0; i < vecScripts.size(); i++)
                {
                    string strScriptName = string(vecScripts[i].begin(), vecScripts[i].end());
                    if (ImGui::MenuItem(strScriptName.c_str()))
                    {
                        AddScript(vecScripts[i]);
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

        if (nullptr != pCurLevel)
        {
            switch (pCurLevel->GetState())
            {
            case LEVEL_STATE::STOP:
                if (ImGui::Button("Play"))
                {
                    CLevelSaveLoad::SaveLevel(L"level\\temp.level", pCurLevel);
                    pCurLevel->ChangeState(LEVEL_STATE::PLAY);
                    ShowCursor(false);
                }
                break;
            case LEVEL_STATE::PAUSE:
            case LEVEL_STATE::PLAY:
                if (ImGui::Button("Stop"))
                {
                    pCurLevel->ChangeState(LEVEL_STATE::STOP);
                    CLevel* pTempLevel = CLevelSaveLoad::LoadLevel(L"level\\temp.level");

                    tEvent evn = {};
                    evn.Type = EVENT_TYPE::LEVEL_CHANGE;
                    evn.wParam = (DWORD_PTR)pTempLevel;
                    CEventMgr::GetInst()->AddEvent(evn);
                    CTimeMgr::GetInst()->Start();
                    ShowCursor(true);
                }

                switch (pCurLevel->GetState())
                {
                case LEVEL_STATE::PAUSE:
                    if (ImGui::Button("Start"))
                    {
                        pCurLevel->ChangeState(LEVEL_STATE::PLAY);
                        CTimeMgr::GetInst()->Start();
                    }
                    break;
                case LEVEL_STATE::PLAY:
                    if (ImGui::Button("Pause"))
                    {
                        pCurLevel->ChangeState(LEVEL_STATE::PAUSE);
                        CTimeMgr::GetInst()->Stop();
                    }
                    break;
                }
                break;
            }
        }

        ImGui::EndMainMenuBar();
    }
    return 0;
}

void MenuUI::CreateEmptyObject()
{
    CGameObject* pNewObject = new CGameObject;
    pNewObject->SetName(L"New Object");
    pNewObject->AddComponent(new CTransform());
    SpawnGameObject(pNewObject, CRenderMgr::GetInst()->GetEditorCamPos(), L"Default");

    OutlinerUI* pOutliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");
    pOutliner->SetReserveData((DWORD_PTR)pNewObject);
}

void MenuUI::AddComponent(COMPONENT_TYPE _type)
{
    OutlinerUI* pOutliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");

    CGameObject* pSelectedObj = pOutliner->GetSelectedObject();

    if (nullptr == pSelectedObj)
        return;

    if (nullptr != pSelectedObj->GetComponent(_type))
    {
        MessageBox(nullptr, L"이미 보유중인 컴포넌트는 추가 할 수 없습니다", L"AddComponent 오류", MB_OK);
        return;
    }

    if (nullptr != pSelectedObj->GetRenderComponent() && COMPONENT_TYPE::MESHRENDER <= _type &&
        _type <= COMPONENT_TYPE::DECAL)
    {
        MessageBox(nullptr, L"Render 컴포넌트는 한 종류만 가질수 있습니다", L"AddComponent 오류", MB_OK);
        return;
    }

    switch (_type)
    {
    case COMPONENT_TYPE::TRANSFORM:
        pSelectedObj->AddComponent(new CTransform());
        break;
    case COMPONENT_TYPE::COLLIDER2D:
        pSelectedObj->AddComponent(new CCollider2D());
        break;
    case COMPONENT_TYPE::COLLIDER3D:
        break;
    case COMPONENT_TYPE::ANIMATOR2D:
        pSelectedObj->AddComponent(new CAnimator2D());
        break;
    case COMPONENT_TYPE::ANIMATOR3D:
        break;
    case COMPONENT_TYPE::LIGHT2D:
        pSelectedObj->AddComponent(new CLight2D());
        break;
    case COMPONENT_TYPE::LIGHT3D:
        break;
    case COMPONENT_TYPE::CAMERA:
        pSelectedObj->AddComponent(new CCamera());
        break;
    case COMPONENT_TYPE::MESHRENDER:
        pSelectedObj->AddComponent(new CMeshRender());
        break;
    case COMPONENT_TYPE::PARTICLESYSTEM:
        pSelectedObj->AddComponent(new CParticleSystem());
        break;
    case COMPONENT_TYPE::TILEMAP:
        pSelectedObj->AddComponent(new CTileMap());
        break;
    case COMPONENT_TYPE::LANDSCAPE:
        break;
    case COMPONENT_TYPE::DECAL:
        break;
    }

    InspectorUI* pInspector = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");
    pInspector->SetTargetObject(pSelectedObj);
}

void MenuUI::AddScript(const wstring& _strScriptName)
{
    OutlinerUI* pOutliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");
    CGameObject* pSelectedObj = pOutliner->GetSelectedObject();

    if (nullptr == pSelectedObj)
        return;

    CScript* pScript = CScriptMgr::GetScript(_strScriptName);
    pSelectedObj->AddComponent(pScript);
    InitScript(pScript);

    InspectorUI* pInspector = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");
    pInspector->SetTargetObject(pSelectedObj);
}

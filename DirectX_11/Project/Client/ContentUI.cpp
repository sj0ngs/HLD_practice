#include "pch.h"
#include "ContentUI.h"

#include <Engine\CResMgr.h>
#include <Engine\CPathMgr.h>
#include <Engine\CEventMgr.h>
#include <Engine\CRes.h>

#include "InspectorUI.h"
#include "ImGuiMgr.h"
#include "TreeUI.h"

ContentUI::ContentUI()  :
    UI("##ContentUI")
{
    SetName("Content");

    m_Tree = new TreeUI("Content");
    m_Tree->SetActive(true);
    m_Tree->ShowRoot(false);
    m_Tree->UseDragDrop("Resource");

    m_Tree->AddDynamic_Select(this, (UI_DELEGATE_1)&ContentUI::SetTargetToInspector);
    AddChildUI(m_Tree);
}

ContentUI::~ContentUI()
{
}

void ContentUI::init()
{
    Reload();
}

void ContentUI::tick()
{
    UI::tick();

    if (CResMgr::GetInst()->IsResourceChanged())
    {
        ResetContent();
    }
}

int ContentUI::render_update()
{
    return 0;
}

void ContentUI::ResetContent()
{
    m_Tree->Clear();
    m_Tree->AddItem("Root", 0);

    for (UINT i = 0; i < (UINT)RES_TYPE::END; i++)
    {
        const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResources((RES_TYPE)i);
        
        TreeNode* pCategory = m_Tree->AddItem(ToString((RES_TYPE)i), 0);
        pCategory->SetCategoryNode(true);
        
        for (const auto& pair : mapRes)
        {
             m_Tree->AddItem(string(pair.first.begin(), pair.first.end()), (DWORD_PTR)pair.second.Get(), pCategory);
        }
    }
}

void ContentUI::SetTargetToInspector(DWORD_PTR _SelectedNode)
{
    TreeNode* pSelectedNode = (TreeNode*)_SelectedNode;
    CRes* pSelectedRes = (CRes*)pSelectedNode->GetData();

    if (nullptr != pSelectedRes)
    {
        InspectorUI* pInspector = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");
        pInspector->SetTargetRes(pSelectedRes);
    }
}

void ContentUI::Reload()
{
    m_vecResPath.clear();
    wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
    FindFileName(strContentPath);

    for (size_t i = 0; i < m_vecResPath.size(); i++)
    {
        RES_TYPE type = GetResTypeByExt(m_vecResPath[i]);
        
        // 파일명만 리소스 이름으로 쓰게 변경
        wstring strName = GetFileName(m_vecResPath[i]);

        if (RES_TYPE::END == type)
            continue;

        switch (type)
        {
        case RES_TYPE::MESHDATA:
            break;
        case RES_TYPE::MATERIAL:
            CResMgr::GetInst()->LoadRes<CMaterial>(strName, m_vecResPath[i]);
            break;
        case RES_TYPE::PREFAB:
            CResMgr::GetInst()->LoadRes<CPrefab>(strName, m_vecResPath[i]);
            break;
        case RES_TYPE::MESH:
            break;
        case RES_TYPE::TEXTURE:
            CResMgr::GetInst()->LoadRes<CTexture>(strName, m_vecResPath[i]);
            break;
        case RES_TYPE::SOUND:
            CResMgr::GetInst()->LoadRes<CSound>(strName, m_vecResPath[i]);
            break;
        }
    }

    // 리소스의 원본파일 체크
    for (UINT i = 0; i < (UINT)RES_TYPE::END; i++)
    {
        const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResources((RES_TYPE)i);

        for (const auto& pair : mapRes)
        {
            if (pair.second->IsEngineRes())
                continue;

            wstring strFilePath = strContentPath + pair.second->GetRelativePath();
            if (!filesystem::exists(strFilePath))
            {
                tEvent evn = {};
                evn.Type = EVENT_TYPE::DELETE_RESOURCE;
                evn.wParam = (DWORD_PTR)i;
                evn.lParam = (DWORD_PTR)pair.second.Get();

                CEventMgr::GetInst()->AddEvent(evn);
            }
        }
    }

    ResetContent();
}

void ContentUI::FindFileName(const wstring& _FolderPath)
{
    WIN32_FIND_DATA FindData = {};

    wstring strFolderPath = _FolderPath + L"*.*";

    HANDLE hFindHandle = FindFirstFile(strFolderPath.c_str(), &FindData);

    while (FindNextFile(hFindHandle, &FindData))
    {
        if (FILE_ATTRIBUTE_DIRECTORY & FindData.dwFileAttributes)
        {
            if (!wcscmp(FindData.cFileName, L".."))
            {
                continue;
            }

            FindFileName(_FolderPath + FindData.cFileName + L"\\");
            continue;
        }

        wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
        wstring strRelativePath = _FolderPath + FindData.cFileName;

        strRelativePath = strRelativePath.substr(strContentPath.length(), strRelativePath.length() - strContentPath.length());

        m_vecResPath.push_back(strRelativePath);
    }

    FindClose(hFindHandle);
}

RES_TYPE ContentUI::GetResTypeByExt(const wstring& _strRelativePath)
{
    wchar_t szExt[50] = {};
    _wsplitpath_s(_strRelativePath.c_str(), 0, 0, 0, 0, 0, 0, szExt, 50);
    wstring strExt = szExt;

    if (L".mdat" == strExt)
        return RES_TYPE::MESHDATA;
    else if (L".mtrl" == strExt)
        return RES_TYPE::MATERIAL;
    else if (L".pref" == strExt)
        return RES_TYPE::PREFAB;
    else if (L".mesh" == strExt)
        return RES_TYPE::MESH;
    else if (L".png" == strExt || L".PNG" == strExt || L".jpg" == strExt || L"JPG" == strExt ||
        L".jpeg" == strExt || L".JPEG" == strExt || L".bmp" == strExt || L".BMP" == strExt ||
        L".tga" == strExt || L".TGA" == strExt || L".dds" == strExt || L".DDS" == strExt)
        return RES_TYPE::TEXTURE;
    else if (L".wav" == strExt || L".WAV" == strExt || L".mp3" == strExt || L".MP3" == strExt ||
        L".ogg" == strExt || L".OGG" == strExt)
        return RES_TYPE::SOUND;
    else
        return RES_TYPE::END;
}


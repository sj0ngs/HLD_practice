#include "pch.h"
#include "TileMapUI.h"

#include <Engine/CTileMap.h>
#include <Engine/CPathMgr.h>

#include "TileMapEditorUI.h"

TileMapUI::TileMapUI() :
    ComponentUI("##TileMap", COMPONENT_TYPE::TILEMAP)
{
    SetName("TileMap");
}

TileMapUI::~TileMapUI()
{
}

int TileMapUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    CTileMap* pTileMap = (CTileMap*)GetTarget()->GetComponent(COMPONENT_TYPE::TILEMAP);
    ImGui::Text("Row    : %d", (int)pTileMap->GetTileCount().x);
    ImGui::Text("Column : %d", (int)pTileMap->GetTileCount().y);

    ImVec2 vContentSize = ImGui::GetContentRegionAvail();
    if (ImGui::Button("Load From File", ImVec2(vContentSize.x * 0.3f, 20.f)))
    {
        LoadTile();
    }
    
    ImGui::SameLine();

    if (ImGui::Button("Edit TileMap", ImVec2(vContentSize.x * 0.3f, 20.f)))
    {
        TileMapEditorUI* pUI = dynamic_cast<TileMapEditorUI*>(ImGuiMgr::GetInst()->FindUI("##TileMapEditor"));
        assert(pUI);
        pUI->Reset(pTileMap, pTileMap->GetMaterial()->GetTexParam(TEX_0),
            (UINT)pTileMap->GetTileCount().x, (UINT)pTileMap->GetTileCount().y,
            pTileMap->GetSliceSize(), pTileMap->GetTiles());
    }

    ImGui::SameLine();

    if (ImGui::Button("Create TileMap", ImVec2(vContentSize.x * 0.3f, 20.f)))
    {
        TileMapEditorUI* pUI = dynamic_cast<TileMapEditorUI*>(ImGuiMgr::GetInst()->FindUI("##TileMapEditor"));
        assert(pUI);
        pUI->SetActive(true);
        pUI->Clear();
    }

    return TRUE;
}


void TileMapUI::LoadTile()
{
    // open a file name
    OPENFILENAME ofn = {};

    wstring strTileFolderPath = CPathMgr::GetInst()->GetContentPath();
    strTileFolderPath += L"tilemap\\";

    wchar_t szFilePath[256] = {};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFilePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 256;
    ofn.lpstrFilter = L"tile\0*.tile\0ALL\0*.*";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = strTileFolderPath.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (false == GetOpenFileName(&ofn))
        return;

    wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
    size_t start = strFilePath.length();
    wstring Buff = szFilePath;
    size_t end = Buff.length();
    wstring FilePath = Buff.substr(start, end);

    ((CTileMap*)GetTarget()->GetComponent(COMPONENT_TYPE::TILEMAP))->Load(FilePath);
}
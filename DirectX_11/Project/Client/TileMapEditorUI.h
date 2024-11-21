#pragma once
#include "UI.h"

class CTileMap;

class TileMapEditorUI :
    public UI
{
private:
    CTileMap*       m_pTileMap;
    Ptr<CTexture>   m_pTileTex;

    UINT            m_iTileCountX;
    UINT            m_iTileCountY;

    Vec2            m_vSliceSize;

    UINT            m_iSelectTile;

    UINT            m_iSliceCountX;
    UINT            m_iSliceCountY;

    vector<tTile>   m_vecTile;      // 타일 이미지 정보
    vector<tTile>   m_vecMap;       // 맵의 타일 정보

public:
    void SetAtlasTex(Ptr<CTexture> _pTex) { m_pTileTex = _pTex; }
    void SetSliceCount(UINT _iSliceX, UINT _iSliceY);

public:
    virtual void finaltick() override;
    virtual int render_update() override;

public:
    TileMapEditorUI();
    ~TileMapEditorUI();

public:
    void SaveTileMap();
    void LoadTileMap();
    void Clear();
    void Reset(CTileMap* _pTileMap, Ptr<CTexture> _pTex, UINT _iTileCountX, UINT _iTileCountY, 
                Vec2 _m_vSliceSize, const vector<tTile> _vecTile);

private:
    void EditTile();
    void ShowTileList();
    void ShowTileMap();

    int Save(const wstring& _strRelativePath);
    int Load(const wstring& _strRelativePath);
};


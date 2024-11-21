#pragma once
#include "CRenderComponent.h"

class CStructuredBuffer;

class CTileMap :
    public CRenderComponent
{
private:
    UINT                m_iTileCountX;
    UINT                m_iTileCountY;

    Vec2                m_vSliceSize;

    vector<tTile>       m_vecTile;
    CStructuredBuffer*  m_SB;
    
public:
    void SetSliceSize(Vec2 _vSliceSize) { m_vSliceSize = _vSliceSize; };
    void SetSliceSize(UINT _iX, UINT _iY) { m_vSliceSize = Vec2(1.f / (float)_iX, 1.f / (float)_iY); }
    Vec2 GetSliceSize() const { return m_vSliceSize; }

    void SetTileCount(UINT _iXCount, UINT _iYCount);
    Vec2 GetTileCount() const { return Vec2((float)m_iTileCountX, (float)m_iTileCountY); }

    void SetTiles(const vector<tTile> _vecTile) { m_vecTile = _vecTile; }
    const vector<tTile>& GetTiles() const { return m_vecTile; }

public:
    virtual void finaltick() override;
    virtual void render() override;

    int Save(const wstring& _strRelativePath);
    int Load(const wstring& _strRelativePath);

    void UpdateData();

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CTileMap);
public:
    CTileMap();
    CTileMap(const CTileMap& _Origin);
    ~CTileMap();
};


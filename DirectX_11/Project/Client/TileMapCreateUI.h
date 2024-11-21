#pragma once
#include "UI.h"

class TileMapCreateUI :
    public UI
{
private:
    Ptr<CTexture>   m_pTileTex;

    UINT            m_iSliceCountX;
    UINT            m_iSliceCountY;

public:
    virtual void finaltick() override;
    virtual int render_update() override;

public:
    TileMapCreateUI();
    ~TileMapCreateUI();

public:
    void Reset();
    void Clear() { m_pTileTex = nullptr, m_iSliceCountX = 1, m_iSliceCountY = 1; }
};


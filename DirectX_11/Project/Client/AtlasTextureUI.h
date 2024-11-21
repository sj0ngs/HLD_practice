#pragma once
#include "UI.h"

struct tPixel
{
    BYTE b;
    BYTE g;
    BYTE r;
    BYTE a;

    tPixel(BYTE _r, BYTE _g, BYTE _b)
        : r(_r), g(_g), b(_b), a(0)
    {
    }
};

class AnimEditorUI;

class AtlasTextureUI :
    public UI
{
private:
    AnimEditorUI*       m_pAnimEditor;
	Ptr<CTexture>       m_pTex;
    vector<tAnim2DFrm>  m_vecFrm;
    UINT                m_iTexWidth;
    UINT                m_iTexHeight;

    Vec2                m_vScrolling;
    ImVec2              m_vFirst;
    ImVec2              m_vSecond;

    bool                m_bGrid;
    bool                m_bAddingRect;

    float               m_fScale;

    bool                m_bUpdate;  // 프레임 추가 요청이 생겼는가

public:
    void SetAtlasTex(Ptr<CTexture> _pTex);
    void SetVecFrm(vector<tAnim2DFrm>& _vecFrm) { m_vecFrm = _vecFrm; }

public:
	virtual void finaltick();

	virtual int render_update();

public:
	AtlasTextureUI();
	~AtlasTextureUI();

public:
    void Clear();

private:
	void DrawImage();
	// Vec4 = Min x, Min y, Max x, Max y
	Vec4 FindImageRect(Vec2 _vStart);
};


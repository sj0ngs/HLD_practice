#pragma once
#include "UI.h"

class CAnim2D;
class CTexture;

enum Mode
{
	Mode_Move,
	Mode_Swap
};

class AnimEditorUI :
    public UI
{
private:
	CAnim2D*			m_pAnim2D;
	vector<tAnim2DFrm>	m_vecFrm;
	Ptr<CTexture>		m_pTex;
	Vec2				m_vBackSize;
	wstring				m_strName;
		
	float				m_fFrmScrollX;
	int					m_iSelectFrm;

	ImVec2				m_vTexSize;

	bool				m_bBlockEdit;

	int					m_iMode;

	UINT				m_iOverlayIdx;
	bool				m_bOverlay;
	bool				m_bGrid;

	bool				m_bDeleteFrm;
	int					m_iDeleteIdx;

public:
	void SetAnim2D(CAnim2D* _pAnim);
	void SetVecFrm(vector<tAnim2DFrm>& _vecFrm) { m_vecFrm = _vecFrm; }
	const vector<tAnim2DFrm>& GetVecFrm() const { return m_vecFrm; }

	virtual void finaltick();

	virtual int render_update();

public:
	AnimEditorUI();
	~AnimEditorUI();

public:
	void SaveAnim();
	void LoadAnim();
	void LoadTex();

	void Reset();

private:
	void FramePreview();
	void FrameData();
	void PlayAnim();
	void EditAnim();
};


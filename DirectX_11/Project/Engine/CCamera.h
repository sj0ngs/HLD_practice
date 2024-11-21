#pragma once
#include "CComponent.h"

class CGameObject;

class CCamera :
    public CComponent
{
private:
    float       m_fAspectRatio;
    float       m_fScale;

    PROJ_TYPE   m_ProjType;

    UINT        m_iLayerMask;

    int         m_iCamIdx;  // 카메라 우선순위

    Matrix      m_matView;
    Matrix      m_matProj;

    Vec3        m_vOffset; // 카메라 흔들림 등에 쓰일 오프셋
    Vec2        m_vProjSize; // 투영 영역의 크기

    vector<CGameObject*> m_vecOpaque;
    vector<CGameObject*> m_vecMask;
    vector<CGameObject*> m_vecTransparent;
    vector<CGameObject*> m_vecPost;
    vector<CGameObject*> m_vecUI;

    Vec2 m_vCamRightTopPos;
    Vec2 m_vCamLeftBottomPos;

public:
    void SetProjType(PROJ_TYPE _Type) { m_ProjType = _Type; }
    PROJ_TYPE GetProjType() const { return m_ProjType; }

    void SetScale(float _fScale) { m_fScale = _fScale; }
    float GetScale() const { return m_fScale; }

    void SetLayerMask(int _iLayer, bool _Visible);
    void SetLayerMaskAll(bool _Visible);

    void SetCameraIndex(int _idx);
    int GetCameraIndex() { return m_iCamIdx; }

    const Matrix& GetViewMat() const { return m_matView; }
    const Matrix& GetProjMat() const { return m_matProj; }

    Vec3 GetCamOffset() { return m_vOffset; }
    void SetCamOffset(Vec3 _vOffset) { m_vOffset = _vOffset; }

    Vec2 GetProjSize() { return m_vProjSize; }

public:
    void SortObject();
    void render();

    void PlayProj() { g_transform.matView = m_matView; g_transform.matProj = m_matProj; }

public:
    virtual void begin() override;
    virtual void finaltick() override;

    CLONE(CCamera);
public:
    virtual void SaveToLevelFile(FILE* _pFile);
    virtual void LoadFromLevelFile(FILE* _pFile);

private:
    void clear();
    void render_opaque();
    void render_mask();
    void render_transparent();
    void render_postprocess();
    void render_ui();

    void CalcViewMat();
    void CalcProjMat();

    bool FrustrumCheck(CRenderComponent* _pCom);

public:
    CCamera();
    CCamera(const CCamera& _Other);
    ~CCamera();
};


#pragma once
#include "CSingleton.h"

#include "ptr.h"
#include "CTexture.h"

class CCamera;
class CLight2D;
class CStructuredBuffer;

class CRenderMgr :
    public CSingleton<CRenderMgr>
{
    SINGLE(CRenderMgr);
private:
    vector<CCamera*>        m_vecCam;
    CCamera*                m_pEditorCam;

    vector<tDebugShapeInfo> m_vecShapeInfo;

    vector<tLightInfo>      m_vecLight2D;
    CStructuredBuffer*      m_Light2DBuffer;

    void(CRenderMgr::* RENDER_FUNC)(void);

    Ptr<CTexture>           m_RTCopyTex;

    bool                    m_bIsPlayingLevelChange;

public:
    CCamera* GetMainCamera() const 
    { 
        if (m_vecCam.empty())
        {
            return nullptr; 
        }
        return m_vecCam[0]; 
    }

    vector<tDebugShapeInfo>& GetDebugShapeInfo() { return m_vecShapeInfo; }

    Vec3 GetEditorCamPos();

    void LevelChangePlaying() { m_bIsPlayingLevelChange = true; }

public:
    void init();
    void render();

public:
    int RegisterCamera(CCamera* _Cam, int _Idx);
    void RegisterEditorCamera(CCamera* _Cam) { m_pEditorCam = _Cam; }
    void SetRenderFunc(bool _bIsPlay);
    void RegisterLight2D(const tLightInfo& _Light2D) { m_vecLight2D.push_back(_Light2D); }
    void ClearCamera() { m_vecCam.clear(); }

    void AddDebugShapeInfo(const tDebugShapeInfo& _info) { m_vecShapeInfo.push_back(_info); }

    void CopyRenderTarget();

    void ClearDebugShapeInfo() { m_vecShapeInfo.clear(); }

private:
    void UpdateData();
    void render_play();
    void render_editor();
    void Clear();
};


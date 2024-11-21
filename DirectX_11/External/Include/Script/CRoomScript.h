#pragma once

#include <Engine/CScript.h>
#include <Engine/CRoomCollisionShader.h>
#include <Engine/CRoomShadowShader.h>
#include <Engine/CObstacleMapShader.h>

class CPlayerScript;
class CStructuredBuffer;
class CRoomScript :
    public CScript
{
private:
    CGameObject* m_pDodge;
    CGameObject* m_pOverlay;
    CGameObject* m_pLayer0;
    CGameObject* m_pLayer1;
    CGameObject* m_pLayer2;

    Ptr<CTexture> m_pDodgeTex;           // 백그라운드 추가 색상
    Ptr<CTexture> m_pOverlayTex;         // 빛 등을 표현하기 위한 추가 색상

    Ptr<CTexture> m_pLayer0Tex;        // 정적 그림자용 이미지
    Ptr<CTexture> m_pLayer1Tex;        // 실제 플로어 그림에 해당하는 이미지
    Ptr<CTexture> m_pLayer1_invTex;    // 실제 그려지는 용도가 아닌 낙하 체크 등을 하기 위한 이미지
    Ptr<CTexture> m_pLayer2Tex;        // 벽 등을 표현하기 위해 플로어 이미지 위에 덮어씌워 그려주는 이미지

    // 깇찾기 및 룸 사이즈 관리
    Vec2 m_vRoomSize;
    Ptr<CTexture> m_pObstacleTex;
    Ptr<CObstacleMapShader> m_pObstacleMapCS;

    // 충돌
    Ptr<CRoomCollisionShader> m_pCollisionCS;
    CStructuredBuffer* m_pCollisionResultBuffer;

    CPlayerScript* m_pPlayer;

    // 그림자
    Ptr<CTexture> m_pLayerShadowTex;      // 동적 그림자들을 그려둘 텍스쳐
    Ptr<CRoomShadowShader>  m_pShadowCS;
    CGameObject* m_pDynamicShadow;

public:
    void SetBGDodgeTex(Ptr<CTexture> m_pTex) { m_pDodgeTex = m_pTex; }
    Ptr<CTexture> GetBGDodgeTex() { return m_pDodgeTex; }

    void SetBGOverlayTex(Ptr<CTexture> m_pTex) { m_pOverlayTex = m_pTex; }
    Ptr<CTexture> GetBGOverlayTex() { return m_pOverlayTex; }

    void SetLayer0Tex(Ptr<CTexture> m_pTex) { m_pLayer0Tex = m_pTex; }
    Ptr<CTexture> GetLayer0Tex() { return m_pLayer0Tex; }

    void SetLayer1Tex(Ptr<CTexture> m_pTex) { m_pLayer1Tex = m_pTex; }
    Ptr<CTexture> GetLayer1Tex() { return m_pLayer1Tex; }

    void SetLayer1InverseTex(Ptr<CTexture> m_pTex) { m_pLayer1_invTex = m_pTex; }
    Ptr<CTexture> GetLayer1InverseTex() { return m_pLayer1_invTex; }

    void SetLayer2Tex(Ptr<CTexture> m_pTex) { m_pLayer2Tex = m_pTex; }
    Ptr<CTexture> GetLayer2Tex() { return m_pLayer2Tex; }

    Vec2 GetRoomSize() { return m_vRoomSize; }

public:
    virtual void init() override;
    virtual void begin() override;
    virtual void tick() override;
    virtual void SetScriptTex(const string& _strDesc, const Ptr<CTexture>& _pTex) override;
    virtual void LoadFromLevelFile(FILE* _pFile) override;

    CLONE(CRoomScript);
public:
    CRoomScript();
    CRoomScript(const CRoomScript& _pOrigin);
    ~CRoomScript();

private:
    tRoomCollisionResult& RoomCollisionCheck(Vec2& _vPos, float& _fSize);
    void DrawShadow();

public:
    void RoomCollisionCheckAndMove(Vec2& _vPos, float& _fSize, CTransform* _pTargetTransform, int _iType = 0);
    int RoomCollisionCheckFloorType(Vec2& _vPos, float& _fSize);
};


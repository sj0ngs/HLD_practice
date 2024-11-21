#pragma once
#include <Engine\CScript.h>

class CGameObject;
class CRoomScript;
class CCameraMoveScript :
    public CScript
{
private:
    float m_fCamSpeed;
    float m_fCamRotSpeed;

    // 게임중에 자동 카메라 움직임 구현용
    CGameObject*    m_pPlayer;
    CGameObject*    m_pAim;
    CGameObject*    m_pTarget;

    // 게임 룸 밖으로 카메라가 이동하지 않게 막기
    CRoomScript*    m_pRoom;

    bool    m_bCamShake;
    Vec3    m_vShakeDir;
    float   m_fAccShakeTime;
    float   m_fShakeSpeed;
    float   m_fShakeTime;

    // 카메라 효과 용
    CGameObject*    m_pFade;
    float           m_fAccFadeTime;
    float           m_fFadeTime;
    Vec4            m_vFadeColor;
    bool            m_bFadeIn;

    CGameObject*    m_pLetterBox;
    float           m_fAccLBTime;
    float           m_fLBTime;
    Vec4            m_vLetterBoxColor;
    float           m_fMaxValue;
    bool            m_bLetterBoxIn;

    bool            m_bFollowPlayer;

public:
    void SetRoom(CRoomScript* _pRoom) { m_pRoom = _pRoom; }
    void SetFollowPlayer(bool _bSet) { m_bFollowPlayer = _bSet; }

    void SetTarget(CGameObject* _pObj) { m_pTarget = _pObj; }

public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CCameraMoveScript);

public:
    CCameraMoveScript();
    ~CCameraMoveScript();

private:
    void Camera2DMove();
    void Camera3DMove();
    void Game();
    void CameraShake();
    void MoveLimit();

    void Editor();

public:
    void BeginShake(Vec3 _vDir, float _fTime, float _fSpeed) { m_vShakeDir = _vDir; m_fShakeTime = _fTime; m_fShakeSpeed = _fSpeed; m_bCamShake = true; }
    void BeginFade(float _fTime, Vec4 _vColor, bool _bIn) { m_fFadeTime = _fTime; m_vFadeColor = _vColor; m_bFadeIn = _bIn; }
    void BeginLetterBox(float _fTime, Vec4 _vColor, bool _bIn, float _fMaxValue) { m_fLBTime = _fTime; m_vLetterBoxColor = _vColor; m_bLetterBoxIn = _bIn; m_fMaxValue = _fMaxValue; }

private:
    void FadeEffect();
    void LetterBox();
};


#pragma once
#include <Engine\CState.h>

#include "CPlayerIdle.h"

class CAnim2D;
class CPlayerDash :
    public CState
{
private:
    float       m_fDashSpeed;
    Vec3        m_vecEnterDir;
    
    const float m_fWaitTime;
    float       m_fAccWaitTime;

    EFACE_DIR   m_eFaceDir;

    bool        m_bDashEnd;
    CAnim2D*    m_pCurAnim;

    bool        m_bPhantomSlash;
    Vec3        m_vDashStartPos;

    CGameObject* m_pTrail;

    float       m_fTime;
    float       m_fAccTime;

    bool        m_bBackFace;
    
public:
    void UsePhantomSlash() { m_bPhantomSlash = true; }

public:
    CPlayerDash();
    ~CPlayerDash();

public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void SetDashDir();
    void PhantomSlash();
    void PhantomSlashAttack(); // 돌진 공격 종료시 판정용 충돌체 생성
};


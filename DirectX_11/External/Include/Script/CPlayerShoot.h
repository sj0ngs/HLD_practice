#pragma once
#include <Engine\CState.h>
#include "CPlayerIdle.h"

class CPlayerShoot :
    public CState
{
private:
    EFACE_DIR   m_eFaceDir;
    CAnim2D*    m_pCurAnim;

    bool        m_bAble;
    bool        m_bShot;

public:
    CPlayerShoot();
    ~CPlayerShoot();

public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void SetShootDir();
    void ShootPistol();
    void ShootRailGun();
    void ShootShotGun();
    void ShootHandCannon();

    float CalculateAngle();
    Vec3 CalculateSpawnPos(float _fOffset);
    Vec3 GetPlayerDir();
    void Recoil(float _fTime, float _fSpeed);

    Vec3 RayCast(Vec3 _vDir, const vector<CGameObject*>& _pVecObj);
};


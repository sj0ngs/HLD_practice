#pragma once
#include <Engine\CScript.h>

class CPlayerScript;
class CHUDScript :
    public CScript
{
private:
    CPlayerScript*  m_pPlayer;

    CGameObject*    m_arrHP[5];
    CGameObject*    m_arr3Shot[3];
    CGameObject*    m_arr6Shot[6];
    CGameObject*    m_arr4Shot[4];

    CGameObject*    m_pStamia;
    Vec3            m_vecStamina;

    CGameObject*    m_arrWeapon[4];
    CGameObject*    m_arrWeaponBG[4];

    CGameObject*    m_arrHK[3];

    float           m_fDefaultAlphaValue;
    float           m_fEmtpyAlphaValue;

public:
    void SetPlayer(CPlayerScript* _pPlayer) { m_pPlayer = _pPlayer; }

public:
    virtual void init() override;
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CHUDScript);
public:
    CHUDScript();
    ~CHUDScript();

public:
    void HPChange(float _fHP);
    void AmmoChange(float _fAmmo);
    void StaminaChange(float _fStamina);
    void WeaponChange();
    void HealthKitChange(UINT _iHealthKit);

private:
    void Init();
    void SetBattery();
};


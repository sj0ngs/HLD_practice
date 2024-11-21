#pragma once
#include <Engine\CAI.h>

enum class EGUN_TYPE
{
    PISTOL,
    RAILGUN,
    SHOTGUN,
    HANDCANNON
};

#define MAX_HP 5.f

class CHUDScript;
class CRoomScript;
class CPlayerScript :
    public AI
{
private:
    float       m_fMoveSpeed;
    Vec3        m_vFaceDir;

    Vec3        m_vCurPos;
    Vec3        m_vPrevPos;

    // 플레이어 스테이터스
    float       m_fHP;
    float       m_fStamina;
    const float m_fStaminaUse;
    float       m_fAmmo;
    UINT        m_iHealthKit;
    EGUN_TYPE   m_eGunType;
    UINT        m_iGranade;
    
    bool        m_bInvin;

    bool        m_bAmmoRegain;
    float       m_fAccRegainTime;
    float       m_fRegainTime;

    bool        m_bHit;
    float       m_fAccHitTime;
    float       m_fHitTime;

    bool        m_bHeal;
    float       m_fAccHealTime;
    float       m_fHealTime;

    CHUDScript* m_pHUD;
    CRoomScript* m_pRoom;

    Vec3        m_vSafePos;

public:
    void SetMoveSpeed(float _fSpeed) { m_fMoveSpeed = _fSpeed; }
    float GetMoveSpeed() const { return m_fMoveSpeed; }

    void SetFaceDir(Vec3 _vFaceDir) { m_vFaceDir = _vFaceDir; }
    const Vec3& GetFaceDir() const { return m_vFaceDir; }

    void SetGunType(EGUN_TYPE _eType) { m_eGunType = _eType; }
    EGUN_TYPE GetGunType() const { return m_eGunType; }

    float GetHP() { return m_fHP; }
    void SetHUD(CHUDScript* _pHUD) { m_pHUD = _pHUD; }

    void SetRoom(CRoomScript* _pRoom) { m_pRoom = _pRoom; }
    CRoomScript* GetRoom() { return m_pRoom; }

    float GetAmmo() { return m_fAmmo; }

    UINT GetHealthKit() { return m_iHealthKit; }

    void AmmoRegain(DWORD_PTR _pRegain);

    void SetInvin(bool _bInvin) { m_bInvin = _bInvin; }
    bool IsInvin() { return m_bInvin; }

    void SetSafePos(Vec3 _vPos) { m_vSafePos = _vPos; }
    Vec3 GetSafePos() { return m_vSafePos; }

public:
    virtual void begin() override;
    virtual void end() override;
    virtual void tick() override;

    virtual void TakeDamage(CCollider2D* _pOther) override;
    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    virtual void LoadFromLevelFile(FILE* _pFile) override;


    CLONE(CPlayerScript);
public:
    CPlayerScript();
    ~CPlayerScript();

public:
    bool Shoot();
    bool Dash();
    void Heal();
    void RoomCollisionCheck(int _iType = 0);
    int RoomCollisionCheckType();
    void TakeDamage(float _fDmg);
    void Victory();

private:
    void Dissolve();
    void LoadFromFile();
    void SaveToFile();
};


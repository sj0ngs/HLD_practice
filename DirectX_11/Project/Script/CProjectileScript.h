#pragma once
#include <Engine\CScript.h>

class CProjectileScript :
    public CScript
{
private:
    Vec3    m_vDir;     // �̵� ����
    float   m_fSpeed;   // �̵� �ӵ�

    Vec3    m_vStartPos;
    float   m_fDist;
    float   m_faccDist;

    bool    m_bHomming;
    float   m_fHommingRatio;
    CGameObject* m_pTarget;

    Ptr<CSound> m_pEndSound;

    // ���� �ִϸ��̼�
    Vec3    m_vDeadSize;
    wstring m_strDeadAnimName;
    bool    m_bIsFade;

    // ���� ����
    float    m_fDmg;

public:
    void SetDir(Vec3 _vDir) { m_vDir = _vDir; }
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }

    void SetStartPos(Vec3 _vStartPos) { m_vStartPos = _vStartPos; };
    void SetDist(float _fDist) { m_fDist = _fDist; }

    void SetDeadSize(Vec3 _vDeadSize) { m_vDeadSize = _vDeadSize; }
    void SetDeadAnim(const wstring& _strDeadAnim) { m_strDeadAnimName = _strDeadAnim; }

    float GetDamage() { return m_fDmg; }
    void SetDamage(float _fDmg) { m_fDmg = _fDmg; }

    void SetHomming(bool _bHomming) { m_bHomming = _bHomming; }
    void SetHommingRatio(float _fRatio) { m_fHommingRatio = _fRatio; }
    void SetTarget(CGameObject* _pTarget) { m_pTarget = _pTarget; }

    void SetEndSound(const Ptr<CSound>& _pSound) { m_pEndSound = _pSound; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CProjectileScript);
public:
    CProjectileScript();
    ~CProjectileScript();
};


#pragma once
//#include <Engine\CAI.h>
#include "CMonsterScript.h"

class CGhostBeamBirdScript :
    public CMonsterScript
{
private:
    // ÄðÅ¸ÀÓ
    bool        m_bSound;

    bool        m_bHit;
    float       m_fAccHitTime;
    float       m_fHitTime;

public:
    void SetFaceDir(Vec3 _vFaceDir) { m_vFaceDir = _vFaceDir; }
    const Vec3& GetFaceDir() const { return m_vFaceDir; }
    CGameObject* GetTarget() { return m_pTarget; }

    float GetHP() { return m_fHP; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    CLONE(CGhostBeamBirdScript);
public:
    CGhostBeamBirdScript();
    ~CGhostBeamBirdScript();
};


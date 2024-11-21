#pragma once
#include <Engine\CState.h>

class CAnim2D;
class CJerkPopeSpawn :
    public CState
{
private:
    CAnim2D*    m_pCurAnim;
    bool        m_bSpawned;

    float       m_fTime;
    float       m_fAccTime;

    CGameObject* m_arrBirdMan[4];

public:
    CJerkPopeSpawn();
    ~CJerkPopeSpawn();

    CLONE(CJerkPopeSpawn);
public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void Spawn();
    void Clear();
};


#pragma once
#include <Engine\CState.h>

enum class EFACE_DIR
{
    EAST,
    WEST,
    SOUTH,
    NORTH
};
class CPlayerIdle :
    public CState
{
private:
    EFACE_DIR m_eFD; 

public:
    void SetFaceDir(EFACE_DIR _eDir) { m_eFD = _eDir; }

public:
    CPlayerIdle();
    ~CPlayerIdle();

public:
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void RotateFace();
};


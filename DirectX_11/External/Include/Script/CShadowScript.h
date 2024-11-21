#pragma once
#include <Engine/CScript.h>

enum EShadowType
{
    SMALL,
    LARGE
};

class CShadowScript :
    public CScript
{
private:
    EShadowType m_eType;
    Vec2 m_vOffset;
    Vec2 m_vShadowPos;
    bool m_bOnShadow;

public:
    void SetOffset(Vec2 _vOffset) { m_vOffset = _vOffset; }
    Vec2 GetOffset() { return m_vOffset; }
    Vec2 GetShadowFinalPos() { return m_vShadowPos; }

    EShadowType GetShadowType() { return m_eType; }

    void SetOnShadow(bool _bSet) { m_bOnShadow = _bSet; }
    bool IsOnShadow() { return m_bOnShadow; }

public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void LoadFromLevelFile(FILE* _pFile) override;

    CLONE(CShadowScript);
public:
    CShadowScript();
    ~CShadowScript();
};


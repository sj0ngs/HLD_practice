#pragma once
#include <Engine\CScript.h>

class CMenuItemScript :
    public CScript
{
private:
    CGameObject* m_pMenu;
    Vec4    m_vChangedColor;
    wstring m_strText;
    
    bool    m_bPrevEnter;

    bool    m_bLevelChange;
    float   m_fAccTime;
    float   m_fTime;

public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CMenuItemScript);
public:
    CMenuItemScript();
    ~CMenuItemScript();

public:
    void ButtonVisible();
};


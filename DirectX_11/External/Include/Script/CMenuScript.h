#pragma once
#include <Engine\CScript.h>

class CMenuScript :
    public CScript
{
private:
    bool    m_bMouseClicked;
    bool    m_bBooked;

    CScript*        m_pStartButtonInst;
    SCRIPT_DELEGATE m_pButtonVisibleFunc;

public:
    bool GetMouseClicked() { return m_bMouseClicked; }
    void AddDynamicStartButton(CScript* _pInst, SCRIPT_DELEGATE _pFunc) { m_pStartButtonInst = _pInst; m_pButtonVisibleFunc = _pFunc; }

public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CMenuScript);
public:
    CMenuScript();
    ~CMenuScript();
};


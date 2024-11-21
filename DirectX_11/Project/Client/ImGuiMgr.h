#pragma once
#include <Engine\CSingleton.h>

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"

class UI;

// ========
// Delegate
// ========
typedef void (UI::*UI_DELEGATE)(void);
typedef void (UI::*UI_DELEGATE_1)(DWORD_PTR);
typedef void (UI::*UI_DELEGATE_2)(DWORD_PTR, DWORD_PTR);
typedef void (UI::*UI_DELEGATE_3)(DWORD_PTR, DWORD_PTR, DWORD_PTR);

class ImGuiMgr :
    public CSingleton<ImGuiMgr>
{
    SINGLE(ImGuiMgr);
private:
    HWND                    m_hMainWnd;

    map<string, UI*>        m_mapUI;

    HANDLE                  m_hObserver;

public:
    void init(HWND _hWnd);
    void progress();

public:
    UI* FindUI(const string& _strName) const;
    HWND GetMainHwnd() const { return m_hMainWnd; }

private:
    void Begin();
    void tick();
    void finaltick();
    void render();
    
    void CreateUI();
    void InitUI();

    void ObserveContent();
};
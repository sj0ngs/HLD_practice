#include "pch.h"
#include "ImGuiMgr.h"

#include <Engine\CDevice.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CPathMgr.h>
#include <Engine\CKeyMgr.h>
#include <Engine\CGameObject.h>

#include "CImGuizmoMgr.h"

#include "UI.h"
#include "InspectorUI.h"
#include "ContentUI.h"
#include "ListUI.h"
#include "AnimEditorUI.h"
#include "AtlasTextureUI.h"
#include "TileMapEditorUI.h"
#include "TileMapCreateUI.h"
#include "OutlinerUI.h"
#include "MenuUI.h"

#include "ParamUI.h"
#include "MaterialMakeUI.h"
#include "LevelEditorUI.h"

ImGuiMgr::ImGuiMgr()	:
	m_hMainWnd(nullptr),
	m_hObserver(nullptr)
{
}

ImGuiMgr::~ImGuiMgr()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Del_Map(m_mapUI);
}

void ImGuiMgr::init(HWND _hWnd)
{
	m_hMainWnd = _hWnd;

	// ImGui 초기화
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hMainWnd);
	ImGui_ImplDX11_Init(DEVICE, CONTEXT);

	CreateUI();

	InitUI();

	// Content Observer 생성
	wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
	m_hObserver = FindFirstChangeNotification(strContentPath.c_str(), true,
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_ACTION_REMOVED | FILE_ACTION_ADDED);
}

void ImGuiMgr::progress()
{
	Begin();

	tick();

	finaltick();

	render();

	ObserveContent();
}

void ImGuiMgr::Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	CImGuizmoMgr::GetInst()->tick();

	ParamUI::Clear();
}

void ImGuiMgr::tick()
{
	for (const auto& pair : m_mapUI)
	{
		pair.second->tick();
	}
}

void ImGuiMgr::finaltick()
{
	//ImGui::ShowDemoWindow();

	for (const auto& pair : m_mapUI)
	{
		if(pair.second->IsActive())
			pair.second->finaltick();
	}

	if (KEY_TAP(KEY::ENTER))
	{
		ImGui::SetWindowFocus(nullptr);
	}
}

void ImGuiMgr::render()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows(메인 윈도우 외부 윈도우)
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void ImGuiMgr::CreateUI()
{
	UI* pUI = nullptr;

	pUI = new InspectorUI;
	pUI->SetActive(true);
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));

	pUI = new ListUI;
	pUI->SetModal(true);
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));

	pUI = new AnimEditorUI;
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));

	pUI = new AtlasTextureUI;
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));

	pUI = new TileMapEditorUI;
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));

	pUI = new TileMapCreateUI;
	pUI->SetModal(true);
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));

	pUI = new ContentUI;
	pUI->SetActive(true);
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));

	pUI = new OutlinerUI;
	pUI->SetActive(true);
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));

	pUI = new MenuUI;
	pUI->SetActive(true);
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));

	pUI = new MaterialMakeUI;
	pUI->SetActive(false);
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));

	pUI = new LevelEditorUI;
	pUI->SetActive(false);
	m_mapUI.insert(make_pair(pUI->GetID(), pUI));
}

void ImGuiMgr::InitUI()
{
	for (const auto& pair : m_mapUI)
	{
		pair.second->init();
	}
}

void ImGuiMgr::ObserveContent()
{
	DWORD dwWaitStatus = WaitForSingleObject(m_hObserver, 0);

	if (WAIT_OBJECT_0 == dwWaitStatus)
	{
		ContentUI* pContentUI = (ContentUI*)FindUI("##ContentUI");
		pContentUI->Reload();

		FindNextChangeNotification(m_hObserver);
	}
}

UI* ImGuiMgr::FindUI(const string& _strName) const
{
	map<string, UI*>::const_iterator iter = m_mapUI.find(_strName);

	if (iter == m_mapUI.end())
	{
		return nullptr;
	}
	else
	{
		return iter->second;
	}
}

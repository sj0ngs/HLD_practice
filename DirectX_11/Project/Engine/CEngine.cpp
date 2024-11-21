#include "pch.h"
#include "CEngine.h"
#include "CDevice.h"

#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CPathMgr.h"
#include "CResMgr.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CEventMgr.h"
#include "CCollisionMgr.h"
#include "CFontMgr.h"

CEngine::CEngine()	:
	m_hWnd(nullptr),
	m_vResolution{}
{
}

CEngine::~CEngine()
{
}

int CEngine::init(HWND _hWnd, UINT _iWidth, UINT _iHeight)
{
	// 메인 윈도우 핸들
	m_hWnd = _hWnd;
	m_vResolution = Vec2((float)_iWidth, (float)_iHeight);

	// 해상도에 맞는 작업영역 크기 조정
	RECT rt = { 0, 0, (int)_iWidth, (int)_iHeight };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(m_hWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);
	ShowWindow(m_hWnd, true);
	SetWindowText(m_hWnd, wstring(L"Hyper Light Drifter").c_str());

	// Device 초기화
	if (FAILED(CDevice::GetInst()->init(m_hWnd, (UINT)m_vResolution.x, (UINT)m_vResolution.y)))
	{
		MessageBox(nullptr, L"Device 초기화 실패", L"Device 초기화 오류", MB_OK);
		return E_FAIL;
	}

	// Manager 초기화
	CPathMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();
	CKeyMgr::GetInst()->init();
	CResMgr::GetInst()->init();
	CRenderMgr::GetInst()->init();
	CFontMgr::GetInst()->init();
	CLevelMgr::GetInst()->init();

	return S_OK;
}

void CEngine::progress()
{
	tick();

	render();

	CEventMgr::GetInst()->tick();
}

void CEngine::tick()
{
	// Manager Tick
	CResMgr::GetInst()->tick();
	CTimeMgr::GetInst()->tick();
	CKeyMgr::GetInst()->tick();	
	CLevelMgr::GetInst()->tick();
	CCollisionMgr::GetInst()->tick();
}

void CEngine::render()
{
	CTimeMgr::GetInst()->render();

	CRenderMgr::GetInst()->render();
}

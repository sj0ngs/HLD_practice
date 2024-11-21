#include "pch.h"
#include "CTimeMgr.h"

#include "CEngine.h"
#include "CFontMgr.h"


CTimeMgr::CTimeMgr() :
	m_llPrevCount{},
	m_llCurCount{},
	m_llFrequency{},
	m_iCallCount(0),
	m_fDeltaTime(0.f),
	m_fTime(0.f),
	m_bStop(false),
	m_llStopCount{},
	m_fPausedTime(0.f),
	m_fFrameRate(60.f),
	m_bRender(true),
	m_fAccRendTime(0.f)
{

}

CTimeMgr::~CTimeMgr()
{
}


void CTimeMgr::init()
{
	// 1초당 카운팅 증가량
	QueryPerformanceFrequency(&m_llFrequency);
	QueryPerformanceCounter(&m_llCurCount);
	QueryPerformanceCounter(&m_llPrevCount);
}

void CTimeMgr::tick()
{
	m_bRender = false;

	m_fAccRendTime += m_fDeltaTime;

	if (m_fAccRendTime >= 1.f / m_fFrameRate)
	{
		m_bRender = true;
		m_fAccRendTime -= 1.f / m_fFrameRate;
	}

	if (m_bStop)
	{
		m_fDeltaTime = 0.f;
		return;
	}

	QueryPerformanceCounter(&m_llCurCount);

	// tick 간격 시간
	m_fDeltaTime = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequency.QuadPart;

	if (0.f > m_fDeltaTime)
	{
		m_fDeltaTime = 0.f;
	}

	// 누적 시간
	m_fTime += m_fDeltaTime;

	// 함수 호출 횟수
	++m_iCallCount;

	// 이전 카운트 값을 현재 카운트로 갱신
	m_llPrevCount = m_llCurCount;		

	GlobalData.tDT = m_fDeltaTime;
	GlobalData.tAccTime = m_fTime;
}

void CTimeMgr::render()
{
	// 1초에 한번
	wchar_t szBuff[256] = {};
	if (1.f <= m_fTime)
	{
		swprintf_s(szBuff, L"FPS : %d, DT : %f", m_iCallCount, m_fDeltaTime);
		SetWindowText(CEngine::GetInst()->GetMainWnd(), szBuff);

		m_fTime -= 1.f;
		m_iCallCount = 0;
	}

	// 폰트 출력
	//CFontMgr::GetInst()->DrawFont(szBuff, 10.f, 30.f, 16, FONT_RGBA(255, 30, 30, 255));
}

void CTimeMgr::Reset()
{
	QueryPerformanceCounter(&m_llCurCount);
	m_llPrevCount = m_llCurCount;

	m_llStopCount.QuadPart = 0;
	m_bStop = false;
}

void CTimeMgr::Stop()
{
	if (m_bStop)
		return;
	
	QueryPerformanceCounter(&m_llCurCount);

	m_llStopCount = m_llCurCount;
	m_bStop = true;
}

void CTimeMgr::Start()
{
	LARGE_INTEGER llStartCount;
	llStartCount.QuadPart = 0;

	QueryPerformanceCounter(&llStartCount);

	if (m_bStop)
	{
		m_fPausedTime = (float)((llStartCount.QuadPart - m_llStopCount.QuadPart) / m_llFrequency.QuadPart);

		m_llPrevCount = llStartCount;
		m_llStopCount.QuadPart = 0;
		m_bStop = false;
	}
}
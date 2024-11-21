#pragma once


class CTimeMgr
	: public CSingleton<CTimeMgr>
{	
	SINGLE(CTimeMgr);
private:
	LARGE_INTEGER	m_llPrevCount;
	LARGE_INTEGER	m_llCurCount;
	LARGE_INTEGER	m_llFrequency;

	UINT			m_iCallCount;
	float			m_fDeltaTime;
	float			m_fTime;

	bool			m_bStop;
	LARGE_INTEGER	m_llStopCount;

	float			m_fPausedTime;

	const float		m_fFrameRate;
	bool			m_bRender;
	float			m_fAccRendTime;

public:
	float GetDeltaTime() const { return m_fDeltaTime; }
	bool IsStop() const { return m_bStop; }

	bool CanRender() { return m_bRender; }

public:
	void init();
	void tick();
	void render();

public:
	void Reset();
	void Stop();
	void Start();
};


#include "pch.h"
#include "CSpectatorScript.h"

CSpectatorScript::CSpectatorScript()	:
	CScript(SCRIPT_TYPE::SPECTATORSCRIPT),
	m_bRun(false),
	m_bTurned(false),
	m_fAccTime(0.f),
	m_fTime(5.f)
{
}

CSpectatorScript::~CSpectatorScript()
{
}

void CSpectatorScript::begin()
{
	// 필드 상에서 왼쪽에 있음
	if (0.f <= Transform()->GetRelativePos().x)
	{
		Transform()->SetRelativeRot(0.f, 180.f, 0.f);
		m_bTurned = true;
	}
}

void CSpectatorScript::tick()
{
	if (!m_bRun)
		return;

	Vec3 vPos = Transform()->GetRelativePos();
	float m_fMoveSpeed = 100.f;
	if (!m_bTurned)
		m_fMoveSpeed *= -1.f;

	vPos.x += m_fMoveSpeed * DT;
	Transform()->SetRelativePos(vPos);

	if (m_fAccTime >= m_fTime)
		Destory();

	m_fAccTime += DT;
}

void CSpectatorScript::Runaway()
{
	m_bRun = true;

	if (Animator2D())
		Animator2D()->Play(L"spr_CultBirdMove", true);

	Transform()->SetRelativeScale(96.f, 96.f, 1.f);
	if (m_bTurned)
		Transform()->SetRelativeRot(0.f, 0.f, 0.f);
	else
		Transform()->SetRelativeRot(0.f, 180.f, 0.f);
}

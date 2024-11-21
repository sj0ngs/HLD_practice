#include "pch.h"
#include "CAttackScript.h"

#include <Engine\CAnim2D.h>
CAttackScript::CAttackScript()	:
	CScript(SCRIPT_TYPE::ATTACKSCRIPT),
	m_fDmg(0.f),
	m_fGain(0.f),
	m_pRegainInst(nullptr),
	m_pRegainFunc(nullptr),
	m_pTarget(nullptr)
{
}

CAttackScript::~CAttackScript()
{
}

void CAttackScript::begin()
{
}

void CAttackScript::tick()
{
	if (Animator2D())
	{
		if (Animator2D()->GetCurAnim()->GetCurFrameNotify() == L"OnCollision")
		{
			if (Collider2D())
				Collider2D()->OnCollision();
		}
		else if(Animator2D()->GetCurAnim()->GetCurFrameNotify() == L"OffCollision")
		{
			if (Collider2D())
				Collider2D()->OffCollision();
		}

		if (Animator2D()->GetCurAnim()->IsFinish())
		{
			Destory();
			return;
		}
	}

	if (IsValid(m_pTarget))
	{
		Vec3 vPos = m_pTarget->Transform()->GetRelativePos();
		vPos += m_vOffset;
		Transform()->SetRelativePos(vPos);
	}
}

void CAttackScript::BeginOverlap(CCollider2D* _Other)
{
	if (m_pRegainInst)
	{
		((m_pRegainInst)->*(m_pRegainFunc))((DWORD_PTR)m_fGain);
	}
}

void CAttackScript::OnOverlap(CCollider2D* _Other)
{
}

void CAttackScript::EndOverlap(CCollider2D* _Other)
{
}

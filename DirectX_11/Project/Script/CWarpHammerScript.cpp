#include "pch.h"
#include "CWarpHammerScript.h"

#include <Engine\CAnim2D.h>
#include "CAttackScript.h"

CWarpHammerScript::CWarpHammerScript() :
	CScript(SCRIPT_TYPE::WARPHAMMERSCRIPT)
{
}

CWarpHammerScript::~CWarpHammerScript()
{
}

void CWarpHammerScript::begin()
{
	Collider2D()->OffCollision();
	GetOwner()->GetScript<CAttackScript>()->SetDamage(2.f);
}

void CWarpHammerScript::tick()
{
	if (Animator2D()->GetCurAnim()->GetCurFrameNotify() == L"On")
	{
		Collider2D()->OnCollision();
	}
	else if (Animator2D()->GetCurAnim()->GetCurFrameNotify() == L"Off")
	{
		Collider2D()->OffCollision();
	}
}

void CWarpHammerScript::BeginOverlap(CCollider2D* _Other)
{
}

void CWarpHammerScript::OnOverlap(CCollider2D* _Other)
{
}

void CWarpHammerScript::EndOverlap(CCollider2D* _Other)
{
}

#include "pch.h"
#include "CRisingBlockScript.h"

#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

CRisingBlockScript::CRisingBlockScript()	:
	CScript(SCRIPT_TYPE::RISINGBLOCKSCRIPT)
{
}

CRisingBlockScript::~CRisingBlockScript()
{
}

void CRisingBlockScript::begin()
{
	if (Collider2D())
		Collider2D()->OffCollision();

	if (Animator2D())
		Animator2D()->Stop();
}

void CRisingBlockScript::tick()
{
}

void CRisingBlockScript::RiseBlock()
{
	if (Animator2D())
		Animator2D()->Play(L"spr_RisingPlatform32_Animated", false);
	
	if (Collider2D())
		Collider2D()->OnCollision();
}

void CRisingBlockScript::DropBlock()
{
	if (Animator2D())
		Animator2D()->Play(L"spr_DropPlatform32", false);

	if (Collider2D())
		Collider2D()->OffCollision();
}

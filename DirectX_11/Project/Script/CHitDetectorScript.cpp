#include "pch.h"
#include "CHitDetectorScript.h"

#include <Engine/CAI.h>

CHitDetectorScript::CHitDetectorScript()	:
	CScript(SCRIPT_TYPE::HITDETECTORSCRIPT),
	m_pOwnerScript(nullptr)
{
}

CHitDetectorScript::CHitDetectorScript(const CHitDetectorScript*& _pOrigin)	:
	CScript(SCRIPT_TYPE::HITDETECTORSCRIPT),
	m_pOwnerScript(nullptr)
{
}

CHitDetectorScript::~CHitDetectorScript()
{
}

void CHitDetectorScript::begin()
{
}

void CHitDetectorScript::tick()
{
	if (m_pOwnerScript)
	{
		CGameObject* pObj = m_pOwnerScript->GetOwner();
		if (IsValid(pObj))
			Transform()->SetRelativePos(pObj->Transform()->GetRelativePos());
		else
			Destory();

		if (m_pOwnerScript->IsDead())
			Destory();
	}
}

void CHitDetectorScript::BeginOverlap(CCollider2D* _Other)
{
	if (m_pOwnerScript)
		m_pOwnerScript->TakeDamage(_Other);
}

void CHitDetectorScript::OnOverlap(CCollider2D* _Other)
{
}

void CHitDetectorScript::EndOverlap(CCollider2D* _Other)
{
}
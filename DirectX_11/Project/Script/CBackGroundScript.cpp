#include "pch.h"
#include "CBackGroundScript.h"

#include <Engine/CRenderMgr.h>

CBackGroundScript::CBackGroundScript()	:
	CScript(SCRIPT_TYPE::BACKGROUNDSCRIPT),
	m_pCamObj(nullptr)
{
}

CBackGroundScript::CBackGroundScript(const CBackGroundScript& _pOrigin)	:
	CScript(_pOrigin),
	m_pCamObj(nullptr)
{
}

CBackGroundScript::~CBackGroundScript()
{
}

void CBackGroundScript::begin()
{
	m_pCamObj = CRenderMgr::GetInst()->GetMainCamera()->GetOwner();
}

void CBackGroundScript::tick()
{
	if (IsValid(m_pCamObj))
	{
		Vec3 vPos = m_pCamObj->Transform()->GetRelativePos();
		Vec3 vMove = (vPos - m_vCamPrevPos) / -20.f;
		vMove += Transform()->GetRelativePos();
		vMove.z = 1000.f;
		Transform()->SetRelativePos(vMove);
		m_vCamPrevPos = vPos;
	}
}


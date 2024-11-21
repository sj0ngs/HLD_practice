#include "pch.h"
#include "CZOrderScript.h"

CZOrderScript::CZOrderScript()	:
	CScript(SCRIPT_TYPE::ZORDERSCRIPT),
	m_bOrderingOn(true)
{
}

CZOrderScript::~CZOrderScript()
{
}

void CZOrderScript::tick()
{
	if (false == m_bOrderingOn)
		return;

	Vec3 vPos = Transform()->GetRelativePos();
	
	vPos.z = 200.f;
	vPos.z += (vPos.y - Transform()->GetRelativeScale().y / 2.f) / 10.f;
	Transform()->SetRelativePos(vPos);
}
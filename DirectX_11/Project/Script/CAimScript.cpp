#include "pch.h"
#include "CAimScript.h"

#include "CMissileScript.h"
#include "CPlayerScript.h"
#include <Engine\CLevelMgr.h>

CAimScript::CAimScript()	:
	CScript((UINT)SCRIPT_TYPE::AIMSCRIPT),
	m_pPlayer(nullptr),
	m_fDist(40.f)
{
}

CAimScript::~CAimScript()
{
}

void CAimScript::begin()
{
	m_pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
}

void CAimScript::tick()
{	
	// 마우스 위치로 다이아몬드 형태 이동
	Vec3 vMousePos = CKeyMgr::GetInst()->GetMouseWorldPos();
	Vec3 vMousePos3D = Vec3(vMousePos.x, vMousePos.y, 0.f);

	if (IsValid(m_pPlayer)) 
	{
		Vec3 vPlayerPos = m_pPlayer->Transform()->GetWorldPos();
		vMousePos3D = Vec3(vMousePos.x, vMousePos.y, vPlayerPos.z);

		Vec3 vDir = vMousePos3D - vPlayerPos;
		vDir.Normalize();

		CPlayerScript* pPlayerScript = m_pPlayer->GetScript<CPlayerScript>();

		if (nullptr != pPlayerScript)
		{
			pPlayerScript->SetFaceDir(vDir);
		}
	}

	Transform()->SetRelativePos(vMousePos3D);
}
#include "pch.h"
#include "CPlayerFall.h"

#include <Engine\CRenderMgr.h>
#include <Engine\CAnim2D.h>
#include "CPlayerScript.h"
#include "CZOrderScript.h"
#include "CCameraMoveScript.h"
#include "CShadowScript.h"

CPlayerFall::CPlayerFall()	:
	m_pCurAnim(nullptr),
	m_fFallTime(0.5f),
	m_fAccFallTime(0.f),
	m_fFallSpeed(500.f)
{
}

CPlayerFall::~CPlayerFall()
{
}

void CPlayerFall::tick()
{
	if (m_fAccFallTime >= m_fFallTime)
	{
		ChangeState(L"Idle");
	}
	else
	{
		m_fAccFallTime += DT;
		CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
		Vec3 vPos = pPlayer->Transform()->GetRelativePos();
		vPos.y -= m_fFallSpeed * DT;
		vPos.z = 800.f;
		pPlayer->Transform()->SetRelativePos(vPos);
	}
}

void CPlayerFall::Enter()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());

	GetOwnerAI()->Transform()->SetRelativeScale(Vec3(26.f, 40.f, 1.f) * 2.f);
	m_pCurAnim = pPlayer->Animator2D()->FindAnim(L"spr_CharDroppingDown");
	m_pCurAnim->Reset();
	pPlayer->Animator2D()->Play(L"spr_CharDroppingDown", true);

	// z order, 카메라 추적을 잠깐 중단시켜야함
	CZOrderScript* pZOrder = dynamic_cast<CZOrderScript*>(pPlayer->GetOwner()->GetScript(SCRIPT_TYPE::ZORDERSCRIPT));
	if (pZOrder)
		pZOrder->SetOrdering(false);

	CCameraMoveScript* pCamMove = dynamic_cast<CCameraMoveScript*>(CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript(SCRIPT_TYPE::CAMERAMOVESCRIPT));
	if (pCamMove)
		pCamMove->SetFollowPlayer(false);

	CShadowScript* pShadow = dynamic_cast<CShadowScript*>(pPlayer->GetOwner()->GetScript(SCRIPT_TYPE::SHADOWSCRIPT));
	if (pShadow)
		pShadow->SetOnShadow(false);
}

void CPlayerFall::Exit()
{
	m_fAccFallTime = 0.f;

	// 안전한 위치로 이동
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	pPlayer->Transform()->SetRelativePos(pPlayer->GetSafePos());

	// z order, 카메라 추적을 잠깐 중단시켜야함
	CZOrderScript* pZOrder = dynamic_cast<CZOrderScript*>(pPlayer->GetOwner()->GetScript(SCRIPT_TYPE::ZORDERSCRIPT));
	if (pZOrder)
		pZOrder->SetOrdering(true);

	CCameraMoveScript* pCamMove = dynamic_cast<CCameraMoveScript*>(CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript(SCRIPT_TYPE::CAMERAMOVESCRIPT));
	if (pCamMove)
		pCamMove->SetFollowPlayer(true);

	pPlayer->TakeDamage(1.f);

	CShadowScript* pShadow = dynamic_cast<CShadowScript*>(pPlayer->GetOwner()->GetScript(SCRIPT_TYPE::SHADOWSCRIPT));
	if (pShadow)
		pShadow->SetOnShadow(true);
}

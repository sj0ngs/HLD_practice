#include "pch.h"
#include "CCultBirdAttack.h"
#include "CCultBirdScript.h"

#include <Engine\CAnim2D.h>
#include "CAttackScript.h"

CCultBirdAttack::CCultBirdAttack() :
	m_pCurAnim(nullptr),
	m_bEnd(false)
{
}

CCultBirdAttack::~CCultBirdAttack()
{
}

void CCultBirdAttack::tick()
{
	if (!m_bEnd && m_pCurAnim->GetCurFrameNotify() == L"Attack")
	{
		// 충돌체 생성
		Attack();
		m_bEnd = true;
	}

	if (m_pCurAnim->IsFinish())
	{
		ChangeState(L"Move");
		return;
	}
}

void CCultBirdAttack::Enter()
{
	CCultBirdScript* pMon = ((CCultBirdScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	m_pCurAnim = pMon->Animator2D()->FindAnim(L"spr_CultBirdAttack");
	pMon->Animator2D()->Play(L"spr_CultBirdAttack", false);
	// 오른쪽 보기
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	}
	// 왼쪽 보기
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
	}

	pMon->SetAttackCool(3.f);
}

void CCultBirdAttack::Exit()
{
	m_pCurAnim = nullptr;
	m_bEnd = false;
}

void CCultBirdAttack::Attack()
{
	CGameObject* pAttack = CResMgr::GetInst()->FindRes<CPrefab>(L"CultBirdSpear")->Instantiate();

	CCultBirdScript* pMon = ((CCultBirdScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();
	float fDir = 1.f;
	if (vDir.y < 0.f)
	{
		fDir = -1.f;
	}
	float fAngle = vDir.Dot(Vec3(1.0, 0.f, 0.f));
	fAngle = acosf(fAngle) * fDir;

	Vec3 vPos = pMon->Transform()->GetRelativePos();
	vPos += vDir * 80.f;
	vPos.y += 20.f;
	vPos.z -= 10.f;

	pAttack->Transform()->SetRelativeRot(Vec3(0.f, 0.f, fAngle));
	pAttack->GetScript<CAttackScript>()->SetDamage(2.f);
	pAttack->Collider2D()->OffCollision();
	SpawnGameObject(pAttack, vPos, L"MonsterAttack");

	CResMgr::GetInst()->FindRes<CSound>(L"snd_CultBirdSpear")->Play(1, 10.f, true);
}

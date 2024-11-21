#include "pch.h"
#include "CCultBirdIdle.h"
#include "CCultBirdScript.h"

CCultBirdIdle::CCultBirdIdle()	:
	m_fDetectRadius(150.f)
{
}

CCultBirdIdle::~CCultBirdIdle()
{
}

void CCultBirdIdle::tick()
{
	CCultBirdScript* pMon = ((CCultBirdScript*)GetOwnerAI());

	// �÷��̾ ���� �����ȿ� ������ ������Ʈ ����
	Vec3 vMonPos = pMon->Transform()->GetRelativePos();
	vMonPos.z = 0.f;
	Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
	vPlayerPos.z = 0.f;

	float fDist = Vec3::Distance(vMonPos, vPlayerPos);

	if (fDist <= m_fDetectRadius)
	{
		ChangeState(L"Move");
	}
}

void CCultBirdIdle::Enter()
{
	CCultBirdScript* pMon = ((CCultBirdScript*)GetOwnerAI());
	Vec3 vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	pMon->Animator2D()->Play(L"spr_CultBirdIdle", true);
	// ������ ����
	if (90.f >= fAngle)
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	}
	// ���� ����
	else
	{
		pMon->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
	}
}

void CCultBirdIdle::Exit()
{
}

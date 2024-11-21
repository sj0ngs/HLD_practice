#include "pch.h"
#include "CCultBirdMove.h"

#include "CCultBirdScript.h"

CCultBirdMove::CCultBirdMove()	:
	m_fMoveSpeed(150.f),
	m_fAttackRadius(150.f),
	m_fDodgeRadius(50.f)
{
}

CCultBirdMove::~CCultBirdMove()
{
}

void CCultBirdMove::tick()
{
	CCultBirdScript* pMon = ((CCultBirdScript*)GetOwnerAI());
	WatchPlayer();

	Vec3 vPos = pMon->Transform()->GetRelativePos();
	vPos.z = 0.f;
	Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
	vPlayerPos.z = 0.f;
	float fDist = Vec3::Distance(vPos, vPlayerPos);

	// �����Ÿ� ���θ� ȸ��
	if (fDist <= m_fDodgeRadius && pMon->IsDodgeAvail())
	{
		ChangeState(L"Dodge");
		return;
	}
	// ���ݰŸ� ���θ� ����
	else if (fDist <= m_fAttackRadius && pMon->IsAttackAvail())
	{
		ChangeState(L"Attack");
		return;
	}

	// �ϴ� ������ ������� ���� ���� A* ��� ���ɼ� ����
	// �÷��̹������� �̵�
	vPos = pMon->Transform()->GetRelativePos();
	vPos += pMon->GetFaceDir() * DT * m_fMoveSpeed;
	pMon->Transform()->SetRelativePos(vPos);
}

void CCultBirdMove::Enter()
{
	WatchPlayer();
}

void CCultBirdMove::Exit()
{
}

void CCultBirdMove::WatchPlayer()
{
	// �׻� �÷��̾� ������ �ٶ󺸰��ϱ�
	CCultBirdScript* pMon = ((CCultBirdScript*)GetOwnerAI());
	Vec3 vMonPos = pMon->Transform()->GetRelativePos();
	vMonPos.z = 0.f;
	Vec3 vPlayerPos = pMon->GetTarget()->Transform()->GetRelativePos();
	vPlayerPos.z = 0.f;

	Vec3 vDir = vPlayerPos - vMonPos;
	vDir.Normalize();

	pMon->SetFaceDir(vDir);

	vDir = pMon->GetFaceDir();
	vDir.Normalize();

	float fCos = vDir.Dot(Vec3(1.f, 0.f, 0.f));
	float fAngle = acosf(fCos);
	fAngle *= 180 / XM_PI;

	pMon->Animator2D()->Play(L"spr_CultBirdMove", true);
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

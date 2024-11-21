#include "pch.h"
#include "CPlayerMove.h"

#include "CPlayerScript.h"
#include "CPlayerIdle.h"

CPlayerMove::CPlayerMove()	:
	m_eEntered(KEY::END)
{
}

CPlayerMove::~CPlayerMove()
{
}

void CPlayerMove::tick()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	Vec3 vCurPos = pPlayer->Transform()->GetRelativePos();
	// =================
	// 캐릭터 이동
	// W	: Move Up
	// A	: Move Left
	// S	: Move Down
	// D	: Move Right
	// =================
	
	float fDiaSpeed = pPlayer->GetMoveSpeed() * cosf(XMConvertToRadians(45.f));
	// 뒤로 이동
	if (KEY_PRESSED(KEY::W) && (KEY::W == m_eEntered))
	{
		if (KEY_PRESSED(KEY::A))
		{
			vCurPos.y += DT * fDiaSpeed;
			vCurPos.x -= DT * fDiaSpeed;
		}
		else if (KEY_PRESSED(KEY::D))
		{
			vCurPos.y += DT * fDiaSpeed;
			vCurPos.x += DT * fDiaSpeed;
		}
		else
		{
			vCurPos.y += DT * pPlayer->GetMoveSpeed();
		}
	}
	// 정면으로 이동
	else if (KEY_PRESSED(KEY::S) && (KEY::S == m_eEntered))
	{
		if (KEY_PRESSED(KEY::A))
		{
			vCurPos.y -= DT * fDiaSpeed;
			vCurPos.x -= DT * fDiaSpeed;
		}
		else if (KEY_PRESSED(KEY::D))
		{
			vCurPos.y -= DT * fDiaSpeed;
			vCurPos.x += DT * fDiaSpeed;
		}
		else
		{
			vCurPos.y -= DT * pPlayer->GetMoveSpeed();
		}
	}
	// 왼쪽으로 이동
	else if (KEY_PRESSED(KEY::A) && (KEY::A == m_eEntered))
	{
		if (KEY_PRESSED(KEY::W))
		{
			vCurPos.x -= DT * fDiaSpeed;
			vCurPos.y += DT * fDiaSpeed;
		}
		else if (KEY_PRESSED(KEY::S))
		{
			vCurPos.x -= DT * fDiaSpeed;
			vCurPos.y -= DT * fDiaSpeed;
		}
		else
		{
			vCurPos.x -= DT * pPlayer->GetMoveSpeed();
		}
	}
	// 오른쪽으로 이동
	else if (KEY_PRESSED(KEY::D) && (KEY::D == m_eEntered))
	{
		if (KEY_PRESSED(KEY::W))
		{
			vCurPos.x += DT * fDiaSpeed;
			vCurPos.y += DT * fDiaSpeed;
		}
		else if (KEY_PRESSED(KEY::S))
		{
			vCurPos.x += DT * fDiaSpeed;
			vCurPos.y -= DT * fDiaSpeed;
		}
		else
		{
			vCurPos.x += DT * pPlayer->GetMoveSpeed();
		}
	}
	else
	{
		ChangeState(L"Idle");
	}

	if (KEY_TAP(KEY::SPACE))
	{
		ChangeState(L"Dash");
	}
	else if (KEY_TAP(KEY::LBTN))
	{
		ChangeState(L"Attack");
	}
	else if (KEY_TAP(KEY::RBTN))
	{
		ChangeState(L"Shoot");
	}
	else if (KEY_TAP(KEY::Q))
	{
		CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
		if (pPlayer->GetHP() < MAX_HP && pPlayer->GetHealthKit() > 0)
		{
			ChangeState(L"Heal");
		}
	}

	pPlayer->Transform()->SetRelativePos(vCurPos);
	pPlayer->RoomCollisionCheck();
}

void CPlayerMove::Enter()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));

	if (KEY_PRESSED(KEY::W))
	{
		pPlayer->Animator2D()->Play(L"spr_CharWalkBack", true);
		m_eEntered = KEY::W;
		pPlayer->Transform()->SetRelativeScale(Vec3(48.f, 70.f, 1.f));
	}
	else if (KEY_PRESSED(KEY::S))
	{
		pPlayer->Animator2D()->Play(L"spr_CharWalkFront", true);
		m_eEntered = KEY::S;
		pPlayer->Transform()->SetRelativeScale(Vec3(48.f, 70.f, 1.f));
	}
	else if (KEY_PRESSED(KEY::A))
	{
		pPlayer->Transform()->SetRelativeRot(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
		pPlayer->Animator2D()->Play(L"spr_CharWalkSide", true);
		m_eEntered = KEY::A;
		pPlayer->Transform()->SetRelativeScale(Vec3(64.f, 64.f, 1.f));
	}
	else if (KEY_PRESSED(KEY::D))
	{
		pPlayer->Animator2D()->Play(L"spr_CharWalkSide", true);
		m_eEntered = KEY::D;
		pPlayer->Transform()->SetRelativeScale(Vec3(64.f, 64.f, 1.f));
	}
}

void CPlayerMove::Exit()
{
	CPlayerScript* pPlayer = ((CPlayerScript*)GetOwnerAI());
	pPlayer->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));

	CPlayerIdle* pIdle = (CPlayerIdle*)pPlayer->FindState(L"Idle");

	switch (m_eEntered)
	{
	case KEY::W:
		pIdle->SetFaceDir(EFACE_DIR::NORTH);
		break;
	case KEY::S:
		pIdle->SetFaceDir(EFACE_DIR::SOUTH);
		break;
	case KEY::A:
		pIdle->SetFaceDir(EFACE_DIR::WEST);
		break;
	case KEY::D:
		pIdle->SetFaceDir(EFACE_DIR::EAST);
		break;
	}
}

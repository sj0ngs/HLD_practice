#include "pch.h"
#include "CCameraMoveScript.h"

#include <Engine\CLevelMgr.h>
#include <Engine\CGameObject.h>

#include "CRoomScript.h"

CCameraMoveScript::CCameraMoveScript() :
	CScript((UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT),
	m_fCamSpeed(500.f),
	m_fCamRotSpeed(5.f),
	m_pPlayer(nullptr),
	m_pAim(nullptr),
	m_pTarget(nullptr),
	m_pRoom(nullptr),
	m_bCamShake(false),
	m_fAccShakeTime(0.f),
	m_fShakeSpeed(0.f),
	m_fShakeTime(0.f),
	m_pFade(nullptr),
	m_fAccFadeTime(0.f),
	m_fFadeTime(0.f),
	m_bFadeIn(true),
	m_bFollowPlayer(true),
	m_pLetterBox(nullptr),
	m_fAccLBTime(0.f),
	m_fLBTime(0.f),
	m_fMaxValue(0.f),
	m_bLetterBoxIn(true)
{
}

CCameraMoveScript::~CCameraMoveScript()
{
}

void CCameraMoveScript::begin()
{
	m_pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	m_pAim = CLevelMgr::GetInst()->FindObjectByName(L"Aim");
	if (m_pPlayer)
	{
		Vec3 vPlayerPos = m_pPlayer->Transform()->GetRelativePos();
		Transform()->SetRelativePos(Vec3(vPlayerPos.x, vPlayerPos.y, Transform()->GetRelativePos().z));
	}

	m_pFade = CResMgr::GetInst()->FindRes<CPrefab>(L"FadeEffect")->Instantiate();
	m_pFade->MeshRender()->GetMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"RTCopyTex"));
	SpawnGameObject(m_pFade, Transform()->GetRelativePos(), 0);

	m_pLetterBox = CResMgr::GetInst()->FindRes<CPrefab>(L"LetterBox")->Instantiate();
	m_pLetterBox->MeshRender()->GetMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"RTCopyTex"));
	SpawnGameObject(m_pLetterBox, Transform()->GetRelativePos(), 0);
}

void CCameraMoveScript::tick()
{
	if (LEVEL_STATE::PLAY == CLevelMgr::GetInst()->GetCurLevel()->GetState())
	{
		Game();
	}
	else
	{
		Editor();
	}
}

void CCameraMoveScript::Game()
{
	FadeEffect();
	LetterBox();

	if (!m_pPlayer || !m_pAim || !m_bFollowPlayer)
		return;

	Vec3 vTargetPos;
	if (nullptr != m_pTarget)
		vTargetPos = m_pTarget->Transform()->GetRelativePos();
	else
	{
		Vec3 vPlayerPos = m_pPlayer->Transform()->GetRelativePos();
		Vec3 vAimPos = m_pAim->Transform()->GetRelativePos();

		vTargetPos = vPlayerPos * 0.8f + vAimPos * 0.2f;
	}

	Vec3 vCurPos = Transform()->GetRelativePos();
	vTargetPos.z = vCurPos.z;
	float vDist = Vector3::Distance(vCurPos, vTargetPos);
	Vec3 vDir = vTargetPos - vCurPos;
	vDir.Normalize();
	vTargetPos = vCurPos + vDir * vDist * 5.f * DT;
	Transform()->SetRelativePos(vTargetPos);

	MoveLimit();

	// 카메라흔들림 효과
	if (m_bCamShake)
		CameraShake();

	if (IsValid(m_pFade))
		m_pFade->Transform()->SetRelativePos(Transform()->GetRelativePos());

	if (IsValid(m_pLetterBox))
		m_pLetterBox->Transform()->SetRelativePos(Transform()->GetRelativePos());
}

void CCameraMoveScript::CameraShake()
{
	Vec3 vOffset = Camera()->GetCamOffset();
	m_fAccShakeTime += DT;
	if (m_fAccShakeTime <= m_fShakeTime / 2.f)
	{
		vOffset += m_vShakeDir * DT * m_fShakeSpeed;
	}
	else if (m_fAccShakeTime > m_fShakeTime / 2.f && m_fAccShakeTime < m_fShakeTime)
	{
		vOffset -= m_vShakeDir * DT * m_fShakeSpeed;
	}
	else if (m_fAccShakeTime >= m_fShakeTime)
	{
		Camera()->SetCamOffset(Vec3(0.f, 0.f, 0.f));
		m_bCamShake = false;
		m_fAccShakeTime = 0.f;
		return;
	}
	Camera()->SetCamOffset(vOffset);
}

void CCameraMoveScript::MoveLimit()
{
	if (nullptr == m_pRoom)
		return;

	Vec2 vProjSize = Camera()->GetProjSize();
	Vec2 vRoomSize = m_pRoom->GetRoomSize();
	Vec3 vCamPos = Transform()->GetRelativePos();

	Vec2 vProjRightTop = vCamPos.XY(); 
	vProjRightTop += vProjSize / 2.f;
	Vec2 vProjLeftBottom = vCamPos.XY(); 
	vProjLeftBottom -= vProjSize / 2.f;

	Vec3 vMove = {};
	if (vProjRightTop.x > vRoomSize.x / 2.f)
		vMove.x = vRoomSize.x / 2.f - vProjRightTop.x;
	else if (vProjLeftBottom.x < -vRoomSize.x / 2.f)
		vMove.x = -vRoomSize.x / 2.f - vProjLeftBottom.x;
	
	if (vProjRightTop.y > vRoomSize.y / 2.f)
		vMove.y = vRoomSize.y / 2.f - vProjRightTop.y;
	else if (vProjLeftBottom.y < -vRoomSize.y / 2.f)
		vMove.y = -vRoomSize.y / 2.f - vProjLeftBottom.y;

	Transform()->SetRelativePos(vCamPos + vMove);
}

void CCameraMoveScript::Camera2DMove()
{
	Vec3 vCurPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vCurPos.y += DT * m_fCamSpeed;
	}
	else if (KEY_PRESSED(KEY::S))
	{
		vCurPos.y -= DT * m_fCamSpeed;
	}
	else if (KEY_PRESSED(KEY::A))
	{
		vCurPos.x -= DT * m_fCamSpeed;
	}
	else if (KEY_PRESSED(KEY::D))
	{
		vCurPos.x += DT * m_fCamSpeed;
	}

	Transform()->SetRelativePos(vCurPos);

	// 카메라 회전
	Vec3 vCurRot = Transform()->GetRelativeRot();
	if (KEY_PRESSED(KEY::Q))
	{
		vCurRot.x += DT * XM_PI;
	}

	if (KEY_PRESSED(KEY::E))
	{
		vCurRot.y += DT * XM_PI;
	}

	if (KEY_PRESSED(KEY::R))
	{
		vCurRot.z += DT * XM_PI;
	}

	if (KEY_PRESSED(KEY::LALT) && 0.f > CKeyMgr::GetInst()->GetmouseWheelMovement())
	{
		float fScale = Camera()->GetScale();
		fScale += DT * CKeyMgr::GetInst()->GetmouseWheelMovement() * 0.5f;
		Camera()->SetScale(fScale);
	}
	
	if (KEY_PRESSED(KEY::LALT) && 0.f < CKeyMgr::GetInst()->GetmouseWheelMovement())
	{
		float fScale = Camera()->GetScale();
		fScale += DT * CKeyMgr::GetInst()->GetmouseWheelMovement() * 0.5f;
		Camera()->SetScale(fScale);
	}

	//Transform()->SetRelativeRot(vCurRot);
}

void CCameraMoveScript::Camera3DMove()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRight = Transform()->GetRelativeDir(DIR_TYPE::RIGHT);
	Vec3 vUP = Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vFront = Transform()->GetRelativeDir(DIR_TYPE::FRONT);

	if (KEY_PRESSED(KEY::W))
	{
		vPos += DT * vFront * m_fCamSpeed;
	}
	
	if (KEY_PRESSED(KEY::S))
	{
		vPos -= DT * vFront * m_fCamSpeed;
	}
	
	if (KEY_PRESSED(KEY::A))
	{
		vPos -= DT * vRight * m_fCamSpeed;
	}
	
	if (KEY_PRESSED(KEY::D))
	{
		vPos += DT * vRight * m_fCamSpeed;
	}

	Transform()->SetRelativePos(vPos);

	// 카메라 회전
	Vec3 vRot = Transform()->GetRelativeRot();

	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vMousePos = CKeyMgr::GetInst()->GetMouseDir();
		vRot.x -= DT * m_fCamRotSpeed * vMousePos.y;
		vRot.y += DT * m_fCamRotSpeed * vMousePos.x;

		if (KEY_PRESSED(KEY::Q))
		{
			vRot.z += DT * m_fCamRotSpeed;
		}
		else if (KEY_PRESSED(KEY::E))
		{
			vRot.z -= DT * m_fCamRotSpeed;
		}
	}
	Transform()->SetRelativeRot(vRot);
}

void CCameraMoveScript::Editor()
{
	// 투영방식에 따른 카메라 움직임
	if (PROJ_TYPE::ORTHOGRAPHIC == Camera()->GetProjType())
	{
		Camera2DMove();
	}
	else
	{
		Camera3DMove();
	}

	// 투영법 변경
	//if (KEY_TAP(KEY::TAB))
	//{
	//	if (PROJ_TYPE::ORTHOGRAPHIC == Camera()->GetProjType())
	//	{
	//		Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	//	}
	//	else
	//	{
	//		Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	//	}
	//}
}

void CCameraMoveScript::FadeEffect()
{
	// 페이드 효과
	if (!IsValid(m_pFade))
		return;

	if (0.f != m_fFadeTime)
	{
		m_fAccFadeTime += DT;

		float fValue = m_fAccFadeTime / m_fFadeTime;
		// 배율이 점점 작아져야한다
		if (m_bFadeIn)
		{
			fValue = 1 - fValue;
		}
		m_pFade->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &fValue);
		m_pFade->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &m_vFadeColor);

		if (m_fAccFadeTime >= m_fFadeTime)
		{
			m_fFadeTime = 0.f;
			m_fAccFadeTime = 0.f;
		}
	}
	//else
	//{
	//	float fValue = 0.f;
	//	m_pFade->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &fValue);
	//	m_pFade->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &m_vFadeColor);
	//}
}

void CCameraMoveScript::LetterBox()
{
	if (!IsValid(m_pLetterBox))
		return;

	if (0.f != m_fLBTime)
	{
		m_fAccLBTime += DT;

		float fValue = 0.f;
		if (m_bLetterBoxIn)
			fValue = m_fAccLBTime / m_fLBTime * m_fMaxValue;
		else
			fValue = (1.f - m_fAccLBTime / m_fLBTime) * m_fMaxValue;
		
		if (fValue > m_fMaxValue)
			fValue = m_fMaxValue;
		else if (0.f > m_fMaxValue)
			fValue = 0.f;
			
		m_pLetterBox->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &fValue);
		m_pLetterBox->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &m_vLetterBoxColor);

		if (m_fAccLBTime >= m_fLBTime)
		{
			m_fLBTime = 0.f;
			m_fAccLBTime = 0.f;
		}
	}
}

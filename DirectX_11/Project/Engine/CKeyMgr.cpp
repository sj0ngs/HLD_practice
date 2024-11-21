#include "pch.h"
#include "CKeyMgr.h"

#include "CEngine.h"

#include "CRenderMgr.h"

#include "CCamera.h"

int g_arrVK[(UINT)KEY::END]
=
{
	 VK_UP, 
	 VK_DOWN,
	 VK_LEFT,
	 VK_RIGHT,

	 VK_SPACE,	 
	 VK_RETURN,
	 VK_ESCAPE,
	 VK_LMENU,
	 VK_LCONTROL,
	 VK_LSHIFT,
	 VK_TAB,	 
	 
	 VK_LBUTTON,
	 VK_RBUTTON,

	 'Q',
	 'W',
	 'E',
	 'R',

	 'T',
	 'Y',
	 'U',
	 'I',
	 'O',
	 'P',

	 'A',
	 'S',
	 'D',
	 'F',

	 'Z',
	 'X',
	 'C',
	 'V',
	 'B',

	 '0',
	 '1',
	 '2',
	 '3',
	 '4',
	 '5',
	 '6',
	 '7',
	 '8',
	 '9',

	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
};

CKeyMgr::CKeyMgr()	:
	m_fMouseWheelMovememt(0.f)
{

}

CKeyMgr::~CKeyMgr()
{

}


void CKeyMgr::init()
{
	for (int i = 0; i < (int)KEY::END; ++i)
	{
		m_vecKey.push_back(tKeyInfo{ (KEY)i  , KEY_STATE::NONE });
	}
}

void CKeyMgr::tick()
{
	if (GetFocus())
	{
		for (size_t i = 0; i < m_vecKey.size(); ++i)
		{
			if (GetAsyncKeyState(g_arrVK[(UINT)m_vecKey[i].key]) & 0x8000)
			{
				// 이전에는 눌리지 않았다.
				if (false == m_vecKey[i].bPrev)
				{
					m_vecKey[i].state = KEY_STATE::TAP;
					m_vecKey[i].bPrev = true;
				}
				else
				{
					// 지금도 눌려있고, 이전 프레임에서도 눌려있었다.
					m_vecKey[i].state = KEY_STATE::PRESSED;
				}
			}
			else
			{
				// 눌려있지 않다.
				if (false == m_vecKey[i].bPrev)
				{
					m_vecKey[i].state = KEY_STATE::NONE;
				}
				else
				{
					m_vecKey[i].state = KEY_STATE::RELEASED;
					m_vecKey[i].bPrev = false;
				}
			}
		}

		// Mouse 위치 갱신
		CalcMousePos();
	}

	// Window 가 focus 상태가 아니다
	else
	{
		for (size_t i = 0; i < m_vecKey.size(); ++i)
		{
			if (KEY_STATE::TAP == m_vecKey[i].state || KEY_STATE::PRESSED == m_vecKey[i].state)
			{
				m_vecKey[i].state = KEY_STATE::RELEASED;
			}

			else if (KEY_STATE::RELEASED == m_vecKey[i].state)
			{
				m_vecKey[i].state = KEY_STATE::NONE;
			}			 
		}
	}	
}

void CKeyMgr::CalcMousePos()
{
	m_vPreMousePos = m_vMousePos;

	POINT ptMousePos = {};
	GetCursorPos(&ptMousePos);
	ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
	m_vMousePos = Vec2((float)ptMousePos.x, (float)ptMousePos.y);

	m_vMouseDir = m_vMousePos - m_vPreMousePos;
	m_vMouseDir.y *= -1.f;

	Vec2 vResoltion = CEngine::GetInst()->GetWindowResolution();
	Vec4 vPos = Vec4(m_vMousePos.x / vResoltion.x, m_vMousePos.y / vResoltion.y, 0.f, 1);
	vPos.x -= 0.5f;
	vPos.y -= 0.5f;

	// 2, 2 로 Scale 변환
	vPos.x *= 2.f;
	vPos.y *= -2.f;

	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCamera();

	if (!pMainCam)
		return;

	// NDC -> View
	Matrix matProjInv = XMMatrixInverse(nullptr, pMainCam->GetProjMat());

	vPos.x = vPos.x * matProjInv._11 + vPos.y * matProjInv._21 + vPos.z * matProjInv._31 + vPos.w * matProjInv._41;
	vPos.y = vPos.x * matProjInv._12 + vPos.y * matProjInv._22 + vPos.z * matProjInv._32 + vPos.w * matProjInv._42;
	vPos.z = vPos.x * matProjInv._13 + vPos.y * matProjInv._23 + vPos.z * matProjInv._33 + vPos.w * matProjInv._43;
	vPos.w = vPos.x * matProjInv._14 + vPos.y * matProjInv._24 + vPos.z * matProjInv._34 + vPos.w * matProjInv._44;

	// View -> World
	Matrix matViewInv = XMMatrixInverse(nullptr, pMainCam->GetViewMat());

	vPos.x = vPos.x * matViewInv._11 + vPos.y * matViewInv._21 + vPos.z * matViewInv._31 + vPos.w * matViewInv._41;
	vPos.y = vPos.x * matViewInv._12 + vPos.y * matViewInv._22 + vPos.z * matViewInv._32 + vPos.w * matViewInv._42;
	vPos.z = vPos.x * matViewInv._13 + vPos.y * matViewInv._23 + vPos.z * matViewInv._33 + vPos.w * matViewInv._43;
	vPos.w = vPos.x * matViewInv._14 + vPos.y * matViewInv._24 + vPos.z * matViewInv._34 + vPos.w * matViewInv._44;

	m_vMouseWorldPos = Vec3(vPos.x, vPos.y, vPos.z);

	Vec4 vMouseNormal = Vec4(0.f, 0.f, 1.f, 0.f);
	vMouseNormal.x = vMouseNormal.x * matProjInv._11 + vMouseNormal.y * matProjInv._21 +
		vMouseNormal.z * matProjInv._31 + vMouseNormal.w * matProjInv._41;
	vMouseNormal.y = vMouseNormal.x * matProjInv._12 + vMouseNormal.y * matProjInv._22 +
		vMouseNormal.z * matProjInv._32 + vMouseNormal.w * matProjInv._42;
	vMouseNormal.z = vMouseNormal.x * matProjInv._13 + vMouseNormal.y * matProjInv._23 +
		vMouseNormal.z * matProjInv._33 + vMouseNormal.w * matProjInv._43;
	vMouseNormal.w = vMouseNormal.x * matProjInv._14 + vMouseNormal.y * matProjInv._24 +
		vMouseNormal.z * matProjInv._34 + vMouseNormal.w * matProjInv._44;

	vMouseNormal.x = vMouseNormal.x * matViewInv._11 + vMouseNormal.y * matViewInv._21 +
		vMouseNormal.z * matViewInv._31 + vMouseNormal.w * matViewInv._41;
	vMouseNormal.y = vMouseNormal.x * matViewInv._12 + vMouseNormal.y * matViewInv._22 +
		vMouseNormal.z * matViewInv._32 + vMouseNormal.w * matViewInv._42;
	vMouseNormal.z = vMouseNormal.x * matViewInv._13 + vMouseNormal.y * matViewInv._23 +
		vMouseNormal.z * matViewInv._33 + vMouseNormal.w * matViewInv._43;
	vMouseNormal.w = vMouseNormal.x * matViewInv._14 + vMouseNormal.y * matViewInv._24 +
		vMouseNormal.z * matViewInv._34 + vMouseNormal.w * matViewInv._44;

	m_vMouseNormal = Vec3(vMouseNormal.x, vMouseNormal.y, vMouseNormal.z);
}

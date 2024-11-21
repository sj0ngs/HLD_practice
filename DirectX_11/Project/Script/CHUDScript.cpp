#include "pch.h"
#include "CHUDScript.h"
#include <Engine\CResMgr.h>
#include "CPlayerScript.h"

CHUDScript::CHUDScript()	:
	CScript(SCRIPT_TYPE::HUDSCRIPT),
	m_pPlayer(nullptr),
	m_arrHP{},
	m_arr3Shot{},
	m_arr6Shot{},
	m_arr4Shot{},
	m_pStamia(nullptr),
	m_arrWeapon{},
	m_arrWeaponBG{},
	m_arrHK{},
	m_fDefaultAlphaValue(0.f),
	m_fEmtpyAlphaValue(0.2f)
{
}

CHUDScript::~CHUDScript()
{
}

void CHUDScript::init()
{
	Init();
}

void CHUDScript::begin()
{
	m_pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScript<CPlayerScript>();
	m_pPlayer->SetHUD(this);

	//init();
	HPChange(m_pPlayer->GetHP());
	WeaponChange();
	HealthKitChange(m_pPlayer->GetHealthKit());
}

void CHUDScript::tick()
{
}

void CHUDScript::Init()
{
	// Back Ground
	Vec3 vHPPos = Vec3(-500.f, 280.f, 10.f);

	CGameObject* pBG = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
	pBG->SetName(L"HUD_BG");
	pBG->Transform()->SetRelativeScale(Vec3(91.f, 35.f, 1.f));
	pBG->Transform()->SetRelativePos(vHPPos + Vec3(45.f, -10.f, 0.f));
	int a = 1;
	Vec4 vDefault = Vec4(1.f, 0.f, 1.f, 1.f);
	Vec4 vChanged = Vec4(0.f, 0.f, 0.f, 0.f);
	float falpha = 0.7f;
	pBG->MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &a);
	pBG->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_0, &vDefault);
	pBG->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vChanged);
	pBG->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &falpha);
	pBG->SetScriptObj(true);
	AddChildObject(GetOwner(), pBG);

	// HP 관련
	vHPPos.z = 5.f;
	for (int i = 0; i < 5; i++)
	{
		m_arrHP[i] = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
		m_arrHP[i]->SetName(L"HP_" + to_wstring(i));
		m_arrHP[i]->Transform()->SetRelativeScale(Vec3(8.f, 8.f, 1.f) * 2.f);
		m_arrHP[i]->Transform()->SetRelativePos(vHPPos + Vec3(9.f, 0.f, 0.f) * 2.f * (float)i + Vec3(8.f, 0.f, 0.f));
		m_arrHP[i]->SetScriptObj(true);
		AddChildObject(GetOwner(), m_arrHP[i]);
	}

	//HPChange(m_pPlayer->GetHP());
	
	vHPPos.x += 2.f;
	vHPPos.y -= 15.f;
	// 3발
	for (int i = 0; i < 3; i++)
	{
		m_arr3Shot[i] = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
		m_arr3Shot[i]->SetName(L"3shot_" + to_wstring(i));
		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_HudBatteryPip_3shot_0");
		m_arr3Shot[i]->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
		m_arr3Shot[i]->Transform()->SetRelativeScale(Vec3(12.f, 4.f, 1.f) * 2.f);
		m_arr3Shot[i]->Transform()->SetRelativePos(vHPPos + Vec3(13.f, 0.f, 0.f) * 2.f * (float)i + Vec3(12.f, 0.f, 0.f));
		m_arr3Shot[i]->SetScriptObj(true);
		AddChildObject(GetOwner(), m_arr3Shot[i]);
	}

	// 4발
	for (int i = 0; i < 4; i++)
	{
		m_arr4Shot[i] = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
		m_arr4Shot[i]->SetName(L"4shot_" + to_wstring(i));
		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_HudBatteryPip_4shot_0");
		m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
		m_arr4Shot[i]->Transform()->SetRelativeScale(Vec3(8.f, 4.f, 1.f) * 2.f);
		m_arr4Shot[i]->Transform()->SetRelativePos(vHPPos + Vec3(9.f, 0.f, 0.f) * 2.f * (float)i + Vec3(8.f, 0.f, 0.f));
		m_arr4Shot[i]->SetScriptObj(true);
		AddChildObject(GetOwner(), m_arr4Shot[i]);
	}
	
	// 6발
	for (int i = 0; i < 6; i++)
	{
		m_arr6Shot[i] = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
		m_arr6Shot[i]->SetName(L"6shot_" + to_wstring(i));
		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_HudBatteryPip_6shot_0");
		m_arr6Shot[i]->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
		m_arr6Shot[i]->Transform()->SetRelativeScale(Vec3(5.f, 4.f, 1.f) * 2.f);
		m_arr6Shot[i]->Transform()->SetRelativePos(vHPPos + Vec3(6.f, 0.f, 0.f) * 2.f * (float)i + Vec3(5.f, 0.f, 0.f));
		m_arr6Shot[i]->SetScriptObj(true);
		AddChildObject(GetOwner(), m_arr6Shot[i]);
	}

	// 스태미너 관련
	m_pStamia = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
	m_pStamia->SetName(L"Stammina");
	m_pStamia->Transform()->SetRelativeScale(Vec3(86.f, 2.f, 1.f));
	m_pStamia->Transform()->SetRelativePos(vHPPos + Vec3(43.f, -7.f, 1.f));
	Vec3 vPos = m_pStamia->Transform()->GetRelativePos();
	Vec3 vScale = m_pStamia->Transform()->GetRelativeScale() / 2.f;;
	m_vecStamina = vPos - vScale;
	vDefault = Vec4(1.f, 0.f, 1.f, 1.f);
	vChanged = Vec4(1.f, 1.f, 1.f, 0.f);
	m_pStamia->MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &a);
	m_pStamia->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_0, &vDefault);
	m_pStamia->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vChanged);
	m_pStamia->SetScriptObj(true);
	AddChildObject(GetOwner(), m_pStamia);
	
	// 총기 관련
	vHPPos = Vec3(-350.f, 265.f, 5.f);
	for (int i = 0; i < 4; i++)
	{
		m_arrWeaponBG[i] = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
		m_arrWeaponBG[i]->SetName(L"WeaponBG_" + to_wstring(i));
		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_HudWeaponBG_1");
		m_arrWeaponBG[i]->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
		m_arrWeaponBG[i]->Transform()->SetRelativeScale(Vec3(28.f, 13.f, 1.f) * 2.f);
		m_arrWeaponBG[i]->Transform()->SetRelativePos(vHPPos + Vec3(-4.f, 4.f, 1.f) * (float)i);
		m_arrWeaponBG[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &falpha);
		m_arrWeaponBG[i]->SetScriptObj(true);
	}

	for (int i = 3; i >= 0; i--)
	{
		AddChildObject(GetOwner(), m_arrWeaponBG[i]);
	}

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_HudWeaponBG_0");
	m_arrWeaponBG[0]->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);

	for (int i = 0; i < 4; i++)
	{
		m_arrWeapon[i] = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
		m_arrWeapon[i]->SetName(L"Weapon_" + to_wstring(i));
		pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_itemsGUI_" + to_wstring(i));
		m_arrWeapon[i]->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
		m_arrWeapon[i]->Transform()->SetRelativeScale(Vec3(22.f, 10.f, 1.f) * 2.f);
		vPos = m_arrWeaponBG[i]->Transform()->GetRelativePos();
		vPos.z -= 1.f;
		m_arrWeapon[i]->Transform()->SetRelativePos(vPos);
		m_arrWeapon[i]->SetScriptObj(true);
		AddChildObject(GetOwner(), m_arrWeapon[i]);
	}

	//WeaponChange();

	// 헬스킷
	vHPPos = Vec3(-550.f, 270.f, 10.f);
	CGameObject* pHKBG = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
	pHKBG->SetName(L"Health_BG");
	pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_HudMedBG_0");
	pHKBG->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
	pHKBG->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &falpha);
	pHKBG->Transform()->SetRelativeScale(Vec3(14.f, 18.f, 1.f) * 2.f);
	pHKBG->Transform()->SetRelativePos(vHPPos);
	pHKBG->SetScriptObj(true);
	AddChildObject(GetOwner(), pHKBG);

	CGameObject* pHKIcon = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
	pHKIcon->SetName(L"Health_Img");
	pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_HudMedIcon_0");
	pHKIcon->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
	pHKIcon->Transform()->SetRelativeScale(Vec3(7.f, 11.f, 1.f) * 2.f);
	pHKIcon->Transform()->SetRelativePos(vHPPos + Vec3(0.f, 5.f, -1.f));
	pHKIcon->SetScriptObj(true);
	AddChildObject(GetOwner(), pHKIcon);

	vHPPos.y -= 12.f;
	vHPPos.x -= 8.f;
	vHPPos.z -= 5.f;
	for (int i = 0; i < 3; i++)
	{
		m_arrHK[i] = CResMgr::GetInst()->FindRes<CPrefab>(L"HUD")->Instantiate();
		m_arrHK[i]->SetName(L"HealthKit_" + to_wstring(i));
		m_arrHK[i]->Transform()->SetRelativeScale(Vec3(5.f, 5.f, 1.f));
		m_arrHK[i]->Transform()->SetRelativePos(vHPPos + Vec3(8.f,0.f,0.f) * (float)i);
		m_arrHK[i]->SetScriptObj(true);
		AddChildObject(GetOwner(), m_arrHK[i]);
	}

	//HealthKitChange(m_pPlayer->GetHealthKit());
}

void CHUDScript::SetBattery()
{
	Vec3 v3Scale = Vec3(12.f, 4.f, 1.f) * 2.f;
	Vec3 v4Scale = Vec3(8.f, 4.f, 1.f) * 2.f;
	Vec3 v6Scale = Vec3(5.f, 4.f, 1.f) * 2.f;

	switch (m_pPlayer->GetGunType())
	{
	case EGUN_TYPE::PISTOL:
		v3Scale = Vec3();
		v4Scale = Vec3();
		break;
	case EGUN_TYPE::RAILGUN:
		v3Scale = Vec3();
		v6Scale = Vec3();
		break;
	case EGUN_TYPE::SHOTGUN:
		v4Scale = Vec3();
		v6Scale = Vec3();
		break;
	case EGUN_TYPE::HANDCANNON:
		v3Scale = Vec3();
		v6Scale = Vec3();
		break;
	}

	for (int i = 0; i < 3; i++)
	{
		m_arr3Shot[i]->Transform()->SetRelativeScale(v3Scale);
	}

	for (int i = 0; i < 4; i++)
	{
		m_arr4Shot[i]->Transform()->SetRelativeScale(v4Scale);
	}

	for (int i = 0; i < 6; i++)
	{
		m_arr6Shot[i]->Transform()->SetRelativeScale(v6Scale);
	}

	AmmoChange(m_pPlayer->GetAmmo());
}

void CHUDScript::HPChange(float _fHP)
{
	for (int i = 0; i < 5; i++)
	{
		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_HudHealthPip_Empty_" + to_wstring(i));
		m_arrHP[i]->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
		m_arrHP[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &m_fEmtpyAlphaValue);
	}

	for (int i = 0; i < (int)_fHP; i++)
	{
		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_HudHealthPip_" + to_wstring(i));
		m_arrHP[i]->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
		m_arrHP[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &m_fDefaultAlphaValue);
	}
}

void CHUDScript::AmmoChange(float _fAmmo)
{
	int a = 0;
	Vec4 vDefault = Vec4(1.f, 1.f, 1.f, 0.5f);
	Vec4 vChanged = Vec4(0.96f, 0.086f, 0.71f, 1.f);

	for (int i = 0; i < 3; i++)
	{
		m_arr3Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &a);
		m_arr3Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &m_fEmtpyAlphaValue);
	}

	for (int i = 0; i < 4; i++)
	{
		m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &a);
		m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &m_fEmtpyAlphaValue);
	}

	for (int i = 0; i < 6; i++)
	{
		m_arr6Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &a);
		m_arr6Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &m_fEmtpyAlphaValue);
	}

	a = 1;
	switch (m_pPlayer->GetGunType())
	{
	case EGUN_TYPE::PISTOL:
	{
		int iAmmo = (int)(_fAmmo / (100.f / 6.f));

		for (int i = 0; i < iAmmo; i++)
		{
			m_arr6Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &a);
			m_arr6Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_0, &vDefault);
			m_arr6Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vChanged);
			m_arr6Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &m_fDefaultAlphaValue);
		}
	}
		break;
	case EGUN_TYPE::RAILGUN:
	{
		int iAmmo = (int)(_fAmmo / (100.f / 4.f));

		for (int i = 0; i < iAmmo; i++)
		{
			m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &a);
			m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_0, &vDefault);
			m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vChanged);
			m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &m_fDefaultAlphaValue);
		}
	}
		break;
	case EGUN_TYPE::SHOTGUN:
	{
		int iAmmo = (int)(_fAmmo / (100.f / 3.f));

		for (int i = 0; i < iAmmo; i++)
		{
			m_arr3Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &a);
			m_arr3Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_0, &vDefault);
			m_arr3Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vChanged);
			m_arr3Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &m_fDefaultAlphaValue);
		}
	}
		break;
	case EGUN_TYPE::HANDCANNON:
	{
		int iAmmo = (int)(_fAmmo / (100.f / 4.f));

		for (int i = 0; i < iAmmo; i++)
		{
			m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &a);
			m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_0, &vDefault);
			m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_1, &vChanged);
			m_arr4Shot[i]->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &m_fDefaultAlphaValue);
		}
	}
		break;
	}
}

void CHUDScript::StaminaChange(float _fStamina)
{
	Vec3 vDefault = Vec3(86.f, 2.f, 1.f);
	
	vDefault.x *= _fStamina / 100.f;
	m_pStamia->Transform()->SetRelativeScale(vDefault);
	Vec3 vPos = m_vecStamina;
	vPos.x += vDefault.x / 2.f;
	m_pStamia->Transform()->SetRelativePos(vPos);
}

void CHUDScript::WeaponChange()
{
	SetBattery();
	int idx = (int)m_pPlayer->GetGunType();

	for (int i = 0; i < 4; i++)
	{
		m_arrWeapon[i]->Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));
	}

	Vec3 vPos = m_arrWeaponBG[0]->Transform()->GetRelativePos();
	vPos.z -= 0.5f;
	m_arrWeapon[idx]->Transform()->SetRelativePos(vPos);
	m_arrWeapon[idx]->Transform()->SetRelativeScale(Vec3(22.f, 10.f, 1.f) * 2.f);
}

void CHUDScript::HealthKitChange(UINT _iHealthKit)
{
	Ptr<CTexture> pTex;
	for (int i = 0; i < 3; i++)
	{
		if((int)_iHealthKit - 1 >= i)
			pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_InvHealthPack_Pip_0");
		else
			pTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_InvHealthPack_Pip_1");

		m_arrHK[i]->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
	}
}

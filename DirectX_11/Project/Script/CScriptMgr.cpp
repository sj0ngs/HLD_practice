#include "pch.h"
#include "CScriptMgr.h"

#include "CAimScript.h"
#include "CAttackScript.h"
#include "CBackGroundScript.h"
#include "CBirdManScript.h"
#include "CBlockScript.h"
#include "CBossTriggerScript.h"
#include "CCameraMoveScript.h"
#include "CCultBirdScript.h"
#include "CEffectAnimScript.h"
#include "CGhostBeamBirdScript.h"
#include "CHitDetectorScript.h"
#include "CHUDScript.h"
#include "CJerkPopeScript.h"
#include "CMenuItemScript.h"
#include "CMenuScript.h"
#include "CMissileScript.h"
#include "CMonsterScript.h"
#include "CPlayerScript.h"
#include "CPlayerTrailScript.h"
#include "CProjectileScript.h"
#include "CRisingBlockScript.h"
#include "CRoomScript.h"
#include "CShadowScript.h"
#include "CSpectatorScript.h"
#include "CTriggerScript.h"
#include "CWarpHammerScript.h"
#include "CZOrderScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CAimScript");
	_vec.push_back(L"CAttackScript");
	_vec.push_back(L"CBackGroundScript");
	_vec.push_back(L"CBirdManScript");
	_vec.push_back(L"CBlockScript");
	_vec.push_back(L"CBossTriggerScript");
	_vec.push_back(L"CCameraMoveScript");
	_vec.push_back(L"CCultBirdScript");
	_vec.push_back(L"CEffectAnimScript");
	_vec.push_back(L"CGhostBeamBirdScript");
	_vec.push_back(L"CHitDetectorScript");
	_vec.push_back(L"CHUDScript");
	_vec.push_back(L"CJerkPopeScript");
	_vec.push_back(L"CLevel1Script");
	_vec.push_back(L"CMenuItemScript");
	_vec.push_back(L"CMenuScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CPlayerTrailScript");
	_vec.push_back(L"CProjectileScript");
	_vec.push_back(L"CRisingBlockScript");
	_vec.push_back(L"CRoomScript");
	_vec.push_back(L"CShadowScript");
	_vec.push_back(L"CSpectatorScript");
	_vec.push_back(L"CTriggerScript");
	_vec.push_back(L"CWarpHammerScript");
	_vec.push_back(L"CZOrderScript");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CAimScript" == _strScriptName)
		return new CAimScript;
	if (L"CAttackScript" == _strScriptName)
		return new CAttackScript;
	if (L"CBackGroundScript" == _strScriptName)
		return new CBackGroundScript;
	if (L"CBirdManScript" == _strScriptName)
		return new CBirdManScript;
	if (L"CBlockScript" == _strScriptName)
		return new CBlockScript;
	if (L"CBossTriggerScript" == _strScriptName)
		return new CBossTriggerScript;
	if (L"CCameraMoveScript" == _strScriptName)
		return new CCameraMoveScript;
	if (L"CCultBirdScript" == _strScriptName)
		return new CCultBirdScript;
	if (L"CEffectAnimScript" == _strScriptName)
		return new CEffectAnimScript;
	if (L"CGhostBeamBirdScript" == _strScriptName)
		return new CGhostBeamBirdScript;
	if (L"CHitDetectorScript" == _strScriptName)
		return new CHitDetectorScript;
	if (L"CHUDScript" == _strScriptName)
		return new CHUDScript;
	if (L"CJerkPopeScript" == _strScriptName)
		return new CJerkPopeScript;
	if (L"CMenuItemScript" == _strScriptName)
		return new CMenuItemScript;
	if (L"CMenuScript" == _strScriptName)
		return new CMenuScript;
	if (L"CMissileScript" == _strScriptName)
		return new CMissileScript;
	if (L"CMonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CPlayerTrailScript" == _strScriptName)
		return new CPlayerTrailScript;
	if (L"CProjectileScript" == _strScriptName)
		return new CProjectileScript;
	if (L"CRisingBlockScript" == _strScriptName)
		return new CRisingBlockScript;
	if (L"CRoomScript" == _strScriptName)
		return new CRoomScript;
	if (L"CShadowScript" == _strScriptName)
		return new CShadowScript;
	if (L"CSpectatorScript" == _strScriptName)
		return new CSpectatorScript;
	if (L"CTriggerScript" == _strScriptName)
		return new CTriggerScript;
	if (L"CWarpHammerScript" == _strScriptName)
		return new CWarpHammerScript;
	if (L"CZOrderScript" == _strScriptName)
		return new CZOrderScript;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::AIMSCRIPT:
		return new CAimScript;
		break;
	case (UINT)SCRIPT_TYPE::ATTACKSCRIPT:
		return new CAttackScript;
		break;
	case (UINT)SCRIPT_TYPE::BACKGROUNDSCRIPT:
		return new CBackGroundScript;
		break;
	case (UINT)SCRIPT_TYPE::BIRDMANSCRIPT:
		return new CBirdManScript;
		break;
	case (UINT)SCRIPT_TYPE::BLOCKSCRIPT:
		return new CBlockScript;
		break;
	case (UINT)SCRIPT_TYPE::BOSSTRIGGERSCRIPT:
		return new CBossTriggerScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return new CCameraMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::CULTBIRDSCRIPT:
		return new CCultBirdScript;
		break;
	case (UINT)SCRIPT_TYPE::EFFECTANIMSCRIPT:
		return new CEffectAnimScript;
		break;
	case (UINT)SCRIPT_TYPE::GHOSTBEAMBIRDSCRIPT:
		return new CGhostBeamBirdScript;
		break;
	case (UINT)SCRIPT_TYPE::HITDETECTORSCRIPT:
		return new CHitDetectorScript;
		break;
	case (UINT)SCRIPT_TYPE::HUDSCRIPT:
		return new CHUDScript;
		break;
	case (UINT)SCRIPT_TYPE::JERKPOPESCRIPT:
		return new CJerkPopeScript;
		break;
	case (UINT)SCRIPT_TYPE::MENUITEMSCRIPT:
		return new CMenuItemScript;
		break;
	case (UINT)SCRIPT_TYPE::MENUSCRIPT:
		return new CMenuScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERSCRIPT:
		return new CMonsterScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERTRAILSCRIPT:
		return new CPlayerTrailScript;
		break;
	case (UINT)SCRIPT_TYPE::PROJECTILESCRIPT:
		return new CProjectileScript;
		break;
	case (UINT)SCRIPT_TYPE::RISINGBLOCKSCRIPT:
		return new CRisingBlockScript;
		break;
	case (UINT)SCRIPT_TYPE::ROOMSCRIPT:
		return new CRoomScript;
		break;
	case (UINT)SCRIPT_TYPE::SHADOWSCRIPT:
		return new CShadowScript;
		break;
	case (UINT)SCRIPT_TYPE::SPECTATORSCRIPT:
		return new CSpectatorScript;
		break;
	case (UINT)SCRIPT_TYPE::TRIGGERSCRIPT:
		return new CTriggerScript;
		break;
	case (UINT)SCRIPT_TYPE::WARPHAMMERSCRIPT:
		return new CWarpHammerScript;
		break;
	case (UINT)SCRIPT_TYPE::ZORDERSCRIPT:
		return new CZOrderScript;
		break;
	}
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::AIMSCRIPT:
		return L"CAimScript";
		break;

	case SCRIPT_TYPE::ATTACKSCRIPT:
		return L"CAttackScript";
		break;

	case SCRIPT_TYPE::BACKGROUNDSCRIPT:
		return L"CBackGroundScript";
		break;

	case SCRIPT_TYPE::BIRDMANSCRIPT:
		return L"CBirdManScript";
		break;

	case SCRIPT_TYPE::BLOCKSCRIPT:
		return L"CBlockScript";
		break;

	case SCRIPT_TYPE::BOSSTRIGGERSCRIPT:
		return L"CBossTriggerScript";
		break;

	case SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return L"CCameraMoveScript";
		break;

	case SCRIPT_TYPE::CULTBIRDSCRIPT:
		return L"CCultBirdScript";
		break;

	case SCRIPT_TYPE::EFFECTANIMSCRIPT:
		return L"CEffectAnimScript";
		break;

	case SCRIPT_TYPE::GHOSTBEAMBIRDSCRIPT:
		return L"CGhostBeamBirdScript";
		break;

	case SCRIPT_TYPE::HITDETECTORSCRIPT:
		return L"CHitDetectorScript";
		break;

	case SCRIPT_TYPE::HUDSCRIPT:
		return L"CHUDScript";
		break;

	case SCRIPT_TYPE::JERKPOPESCRIPT:
		return L"CJerkPopeScript";
		break;

	case SCRIPT_TYPE::LEVEL1SCRIPT:
		return L"CLevel1Script";
		break;

	case SCRIPT_TYPE::MENUITEMSCRIPT:
		return L"CMenuItemScript";
		break;

	case SCRIPT_TYPE::MENUSCRIPT:
		return L"CMenuScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::MONSTERSCRIPT:
		return L"CMonsterScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::PLAYERTRAILSCRIPT:
		return L"CPlayerTrailScript";
		break;

	case SCRIPT_TYPE::PROJECTILESCRIPT:
		return L"CProjectileScript";
		break;

	case SCRIPT_TYPE::RISINGBLOCKSCRIPT:
		return L"CRisingBlockScript";
		break;

	case SCRIPT_TYPE::ROOMSCRIPT:
		return L"CRoomScript";
		break;

	case SCRIPT_TYPE::SHADOWSCRIPT:
		return L"CShadowScript";
		break;

	case SCRIPT_TYPE::SPECTATORSCRIPT:
		return L"CSpectatorScript";
		break;

	case SCRIPT_TYPE::TRIGGERSCRIPT:
		return L"CTriggerScript";
		break;

	case SCRIPT_TYPE::WARPHAMMERSCRIPT:
		return L"CWarpHammerScript";
		break;

	case SCRIPT_TYPE::ZORDERSCRIPT:
		return L"CZOrderScript";
		break;

	}
	return nullptr;
}
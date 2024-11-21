#include "pch.h"
#include "CLevelSaveLoad.h"

#include <Engine\CPathMgr.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\components.h>
#include <Engine\CScript.h>

#include <Script\CScriptMgr.h>

CLevel* CLevelSaveLoad::m_pCurrLevel = nullptr;
wstring CLevelSaveLoad::m_strCurrLevelPath = wstring();

int CLevelSaveLoad::SaveLevel(const wstring& _strLevelPath, CLevel* _pLevel)
{
	if (LEVEL_STATE::STOP != _pLevel->GetState())
		return E_FAIL;

	wstring strPath = CPathMgr::GetInst()->GetContentPath();
	strPath += _strLevelPath;

	FILE* pFile = nullptr;

	errno_t iErrNo = _wfopen_s(&pFile, strPath.c_str(), L"wb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Level Save 실패, Error Number = %d", iErrNo);
		MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
		return E_FAIL;
	}

	// Save Level Name
	SaveWString(_pLevel->GetName(), pFile);

	// Save Level Collision Setting
	const vector<pair<UINT, UINT>>& vecCollisionPair = _pLevel->GetCollisionPair();
	SaveCollisionSetting(vecCollisionPair, pFile);

	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		CLayer* pLayer = _pLevel->GetLayer(i);
		
		// Save Layer Name
		SaveWString(pLayer->GetName(), pFile);

		// Save GameObjects
		const vector<CGameObject*>& vecParent =  pLayer->GetParentObject();

		size_t ObjCount = vecParent.size();
		fwrite(&ObjCount, sizeof(ObjCount), 1, pFile);

		for (size_t j = 0; j < ObjCount; j++)
		{
			SaveGameObject(vecParent[j], pFile);
		}
	}

	fclose(pFile);

	return S_OK;
}

int CLevelSaveLoad::SaveGameObject(CGameObject* _pObject, FILE* _pFile)
{
	bool bIsScript = _pObject->IsScriptObj();
	fwrite(&bIsScript, sizeof(bIsScript), 1, _pFile);

	if (true == bIsScript)
		return S_OK;

	// Save Name
	SaveWString(_pObject->GetName(), _pFile);

	// Save Component
	for (UINT i = 0; i <= (UINT)COMPONENT_TYPE::END; i++)
	{
		if ((UINT)COMPONENT_TYPE::END == i)
		{
			fwrite(&i, sizeof(UINT), 1, _pFile);
			break;
		}

		CComponent* pCom = _pObject->GetComponent((COMPONENT_TYPE)i);

		if (nullptr == pCom)
			continue;

		fwrite(&i, sizeof(UINT), 1, _pFile);

		pCom->SaveToLevelFile(_pFile);
	}

	// Save Script
	const vector<CScript*>& vecScript = _pObject->GetScripts();
	size_t ScriptCount = vecScript.size();
	fwrite(&ScriptCount, sizeof(size_t), 1, _pFile);

	for (size_t i = 0; i < ScriptCount; i++)
	{
		wstring ScriptName = CScriptMgr::GetScriptName(vecScript[i]);
		SaveWString(ScriptName, _pFile);
		vecScript[i]->SaveToLevelFile(_pFile);
	}
	
	// Save Child
	const vector<CGameObject*>& vecChild = _pObject->GetChild();
	size_t ChildCount = vecChild.size();
	fwrite(&ChildCount, sizeof(size_t), 1, _pFile);

	for (size_t i = 0; i < ChildCount; i++)
	{
		SaveGameObject(vecChild[i], _pFile);
	}

	return S_OK;
}

CLevel* CLevelSaveLoad::LoadLevel(const wstring& _strLevelPath)
{
	wstring strPath = CPathMgr::GetInst()->GetContentPath();
	strPath += _strLevelPath;

	FILE* pFile = nullptr;

	errno_t iErrNo = _wfopen_s(&pFile, strPath.c_str(), L"rb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Level Load 실패, Error Number = %d", iErrNo);
		MessageBox(nullptr, szStr, L"파일 로드 실패", MB_OK);
		return nullptr;
	}

	CLevel* pNewLevel = new CLevel;

	// Load Level Name
	wstring strLevelName;
	LoadWString(strLevelName, pFile);
	pNewLevel->SetName(strLevelName);

	// Save Level Collision 
	LoadCollisionSetting(pNewLevel, pFile);

	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		CLayer* pLayer = pNewLevel->GetLayer(i);

		// Load Layer Name
		wstring strLayerName;
		LoadWString(strLayerName, pFile);
		pLayer->SetName(strLayerName);

		// Load GameObjects 
		const vector<CGameObject*>& vecParent = pLayer->GetParentObject();

		size_t ObjCount = 0;
		fread(&ObjCount, sizeof(ObjCount), 1, pFile);

		for (size_t j = 0; j < ObjCount; j++)
		{
			CGameObject* pNewObj = LoadGameObject(pFile);

			if(nullptr != pNewObj)
				pNewLevel->AddGameObject(pNewObj, i, false);
		}
	}
	
	fclose(pFile);

	pNewLevel->ChangeState(LEVEL_STATE::STOP);

	m_strCurrLevelPath = _strLevelPath;
	m_pCurrLevel = pNewLevel;

	return pNewLevel;
}

CGameObject* CLevelSaveLoad::LoadGameObject(FILE* _pFile)
{
	bool bIsScript = false;
	fread(&bIsScript, sizeof(bIsScript), 1, _pFile);

	if (true == bIsScript)
		return nullptr;

	// Load Name
	CGameObject* pNewObj = new CGameObject;
	wstring strName;
	LoadWString(strName, _pFile);
	pNewObj->SetName(strName);

	// Load Component
	while (true)
	{
		UINT ComponentType = 0;
		fread(&ComponentType, sizeof(UINT), 1, _pFile);

		if ((UINT)COMPONENT_TYPE::END == ComponentType)
			break;

		CComponent* pCom = nullptr;

		switch ((COMPONENT_TYPE)ComponentType)
		{
		case COMPONENT_TYPE::TRANSFORM:
			pCom = new CTransform();
			break;
		case COMPONENT_TYPE::COLLIDER2D:
			pCom = new CCollider2D();
			break;
		case COMPONENT_TYPE::COLLIDER3D:
			break;
		case COMPONENT_TYPE::ANIMATOR2D:
			pCom = new CAnimator2D();
			break;
		case COMPONENT_TYPE::ANIMATOR3D:
			break;
		case COMPONENT_TYPE::LIGHT2D:
			pCom = new CLight2D();
			break;
		case COMPONENT_TYPE::LIGHT3D:
			break;
		case COMPONENT_TYPE::CAMERA:
			pCom = new CCamera();
			break;
		case COMPONENT_TYPE::MESHRENDER:
			pCom = new CMeshRender();
			break;
		case COMPONENT_TYPE::PARTICLESYSTEM:
			pCom = new CParticleSystem();
			break;
		case COMPONENT_TYPE::TILEMAP:
			pCom = new CTileMap();
			break;
		case COMPONENT_TYPE::LANDSCAPE:
			break;
		case COMPONENT_TYPE::DECAL:
			break;
		}

		pCom->LoadFromLevelFile(_pFile);
		pNewObj->AddComponent(pCom);
	}

	// Load Script
	size_t ScriptCount = 0;
	fread(&ScriptCount, sizeof(size_t), 1, _pFile);

	for (size_t i = 0; i < ScriptCount; i++)
	{
		wstring strScriptName;
		LoadWString(strScriptName, _pFile);

		CScript* pScript = CScriptMgr::GetScript(strScriptName);
		pNewObj->AddComponent(pScript);
		pScript->LoadFromLevelFile(_pFile);

		InitScript(pScript);
	}

	// Load Child
	size_t ChildCount = 0;
	fread(&ChildCount, sizeof(size_t), 1, _pFile);

	for (size_t i = 0; i < ChildCount; i++)
	{
		CGameObject* pChild = LoadGameObject(_pFile);
		if(nullptr != pChild)
			pNewObj->AddChild(pChild);
	}

	return pNewObj;
}

int CLevelSaveLoad::SaveCollisionSetting(const vector<pair<UINT, UINT>>& _vecCollision, FILE* _pFile)
{
	// 충돌 설정 수 저장
	size_t count = _vecCollision.size();
	fwrite(&count, sizeof(count), 1, _pFile);

	for (size_t i = 0; i < count; i++)
	{
		fwrite(&_vecCollision[i].first, sizeof(_vecCollision[i].first), 1, _pFile);
		fwrite(&_vecCollision[i].second, sizeof(_vecCollision[i].second), 1, _pFile);
	}

	return S_OK;
}

int CLevelSaveLoad::LoadCollisionSetting(CLevel* _pLevel, FILE* _pFile)
{
	// 충돌 설정 수 로드
	size_t count;
	fread(&count, sizeof(count), 1, _pFile);

	// 콜리전 설정 레벨에 저장
	for (size_t i = 0; i < count; i++)
	{
		UINT iLeft = 0, iRight = 0;

		fread(&iLeft, sizeof(iLeft), 1, _pFile);
		fread(&iRight, sizeof(iRight), 1, _pFile);

		_pLevel->AddCollisionPair(iLeft, iRight);
	}

	return S_OK;
}

int CLevelSaveLoad::SaveCurrLevel()
{
	if (m_pCurrLevel)
		return SaveLevel(m_strCurrLevelPath, m_pCurrLevel);

	return E_FAIL;
}


#include "pch.h"
#include "CPrefab.h"

#include "CGameObject.h"
#include "CComponent.h"
#include "CResMgr.h"

#include "CScript.h"
#include <Script\CScriptMgr.h>

CPrefab::CPrefab(bool _bIsEngine)	:
	CRes(RES_TYPE::PREFAB, _bIsEngine),
	m_ProtoObj(nullptr)
{
}

CPrefab::~CPrefab()
{
	SAFE_DEL(m_ProtoObj);
}

void CPrefab::UpdateData()
{
}

void CPrefab::RegisterProtoObject(CGameObject* _Proto)
{
	// 원본은 레이어에 포함되면 안된다
	assert(-1 == _Proto->GetLayerIndex());

	m_ProtoObj = _Proto;
}

CGameObject* CPrefab::Instantiate()
{
	assert(m_ProtoObj);

	return m_ProtoObj->Clone();
}

int CPrefab::Load(const wstring& _strPath)
{
	wstring strPath = CPathMgr::GetInst()->GetContentPath();
	strPath += _strPath;

	FILE* pFile = LoadFile(_strPath, L"프리팹 로드 실패");

	m_ProtoObj = LoadGameObject(pFile);
	fclose(pFile);
	
	return S_OK;
}

int CPrefab::Save(const wstring& _strRelativePath)
{
	wstring strPath = (_strRelativePath + L"\\" + m_ProtoObj->GetName()) + L".pref";

	FILE* pFile = SaveFile(strPath, L"프리팹 저장 실패");

	int result = SaveGameObject(m_ProtoObj, pFile);
	fclose(pFile);

	return result;
}

int CPrefab::SaveGameObject(CGameObject* _pObj, FILE* _pFile)
{
	bool bIsScript = _pObj->IsScriptObj();
	fwrite(&bIsScript, sizeof(bIsScript), 1, _pFile);

	if (true == bIsScript)
		return S_OK;

	// Save Name
	SaveWString(_pObj->GetName(), _pFile);

	// Save Component
	for (UINT i = 0; i <= (UINT)COMPONENT_TYPE::END; i++)
	{
		if ((UINT)COMPONENT_TYPE::END == i)
		{
			fwrite(&i, sizeof(UINT), 1, _pFile);
			break;
		}

		CComponent* pCom = _pObj->GetComponent((COMPONENT_TYPE)i);

		if (nullptr == pCom)
			continue;

		fwrite(&i, sizeof(UINT), 1, _pFile);

		pCom->SaveToLevelFile(_pFile);
	}

	// Save Script
	const vector<CScript*>& vecScript = _pObj->GetScripts();
	size_t ScriptCount = vecScript.size();
	fwrite(&ScriptCount, sizeof(size_t), 1, _pFile);

	for (size_t i = 0; i < ScriptCount; i++)
	{
		wstring ScriptName = CScriptMgr::GetScriptName(vecScript[i]);
		SaveWString(ScriptName, _pFile);
		vecScript[i]->SaveToLevelFile(_pFile);
	}

	// Save Child
	const vector<CGameObject*>& vecChild = _pObj->GetChild();
	size_t ChildCount = vecChild.size();
	fwrite(&ChildCount, sizeof(size_t), 1, _pFile);

	for (size_t i = 0; i < ChildCount; i++)
	{
		SaveGameObject(vecChild[i], _pFile);
	}

	return S_OK;
}

CGameObject* CPrefab::LoadGameObject(FILE* _pFile)
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
		pScript->LoadFromLevelFile(_pFile);
		pNewObj->AddComponent(pScript);
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

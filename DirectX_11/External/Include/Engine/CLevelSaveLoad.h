#pragma once

class CLevel;
class CLayer;
class CGameObject;
class CLevelSaveLoadE
{
public:
	static int SaveLevel(const wstring& _strLevelPath, CLevel* _pLevel);
	static int SaveGameObject(CGameObject* _pObject, FILE* _pFile);

	static CLevel* LoadLevel(const wstring& _strLevelPath);
	static CGameObject* LoadGameObject(FILE* _pFile);

	static int SaveCollisionSetting(const vector<pair<UINT, UINT>>& _vecCollision, FILE* _pFile);
	static int LoadCollisionSetting(CLevel* _pLevel, FILE* _pFile);
};


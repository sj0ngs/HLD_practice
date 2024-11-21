#pragma once

class CGameObject;
bool IsValid(CGameObject*& _pObj);

// 오브젝트 생성
void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, int _LayerIdx);
void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, const wstring& _strLayer);
void AddChildObject(CGameObject* _pParent, CGameObject* _pChild);

// 오브젝트 삭제
void DestoryObject(CGameObject* _DeleteObj);

class CScript;
void InitScript(CScript* _pScript);

// DrawDebugShape
void DrawDebugRect(Vec3 _vWorldPos, Vec3 _vWorldScale, Vec3 _vRotation, Vec4 _vColor, float _fTime = 0.f);
void DrawDebugRect(const Matrix& _matWorld, Vec4 _vColor, float _fTime = 0.f);

void DrawDebugCircle(Vec3 _vWorldPos, float _fRadius, Vec3 _vRotation, Vec4 _vColor, float _fTime = 0.f);
void DrawDebugCircle(const Matrix& _matWorld, Vec4 _vColor, float _fTime = 0.f);

void DrawDebugTri(Vec3 _vWorldPos, Vec3 _vWorldScale, Vec3 _vRotation, Vec4 _vColor, float _fTime = 0.f);
void DrawDebugTri(const Matrix& _matWorld, Vec4 _vColor, float _fTime = 0.f);

void ConvertMatrixToFloatArray(const Matrix& _mat, float* _pArray);

void ConvertFloatArrayToMatrix(Matrix* _mat, const float* _pArray);

const char* ToString(RES_TYPE);
const wchar_t* ToWString(RES_TYPE);

const char* ToString(COMPONENT_TYPE);
const wchar_t* ToWString(COMPONENT_TYPE);

void SaveWString(const wstring& _str, FILE* _pFile);
void LoadWString(wstring& _str, FILE* _pFile);

void Savestring(const string& _str, FILE* _pFile);
void Loadstring(string& _str, FILE* _pFile);

FILE* SaveFile(const wstring& _strRelativePath, const wstring& _strError);
FILE* LoadFile(const wstring& _strRelativePath, const wstring& _strError);

wstring GetFileName(const wstring& _strPath); // 경로에서 폴더와 확장자 명을 제거한 파일명만 리턴

int GetSaveFilePath(const wstring& _strRelativePath, wstring& _strFileOut);
int GetLoadFilePath(const wstring& _strRelativePath, wstring& _strFileOut);

int GetRandInt(int _iMaxRange, int _iMinRange = 0);

class CRes;
template<typename T>
class Ptr;
void SaveResRef(Ptr<CRes> _Res, FILE* _pFile);

class CResMgr;
template<typename T>
void LoadResRef(Ptr<T>& _Res, FILE* _pFile)
{
	int i = 0;
	fread(&i, sizeof(i), 1, _pFile);

	if (i)
	{
		wstring strKey, strRelativePath;
		LoadWString(strKey, _pFile);
		LoadWString(strRelativePath, _pFile);

		_Res = CResMgr::GetInst()->FindRes<T>(strKey);
	}
}

template<typename T, UINT Size>
void Safe_Del_Array(T* (&arr)[Size])
{
	for (UINT i = 0; i < Size; i++)
	{
		if (nullptr != arr[i])
			delete arr[i];
	}
}

template<typename T>
void Safe_Del_Vec(vector<T*>& _vec)
{
	for (size_t i = 0; i < _vec.size(); i++)
	{
		if (nullptr != _vec[i])
			delete _vec[i];
	}
	_vec.clear();
}

template<typename T1, typename T2>
void Safe_Del_Map(map<T1, T2>& _map)
{
	for (const auto& pair : _map)
	{
		if (nullptr != pair.second)
			delete pair.second;
	}
	_map.clear();
}

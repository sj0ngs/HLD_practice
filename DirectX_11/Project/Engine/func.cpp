#include "pch.h"

#include "CEventMgr.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CPathMgr.h"
#include "CResMgr.h"
#include "CLevel.h"
#include "CLayer.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CScript.h"

bool IsValid(CGameObject*& _pObj)
{
	if (_pObj == nullptr)
	{
		return false;
	}
	else if(_pObj->IsDead())
	{
		_pObj = nullptr;
		return false;
	}
	return true;
}

void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, int _LayerIdx)
{
	_NewObject->Transform()->SetRelativePos(_vWorldPos);

	tEvent evn = {};

	evn.Type = EVENT_TYPE::CREATE_OBJECT;
	evn.wParam = (DWORD_PTR)_NewObject;
	evn.lParam = (DWORD_PTR)_LayerIdx;

	CEventMgr::GetInst()->AddEvent(evn);
}

void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, const wstring& _strLayer)
{
	_NewObject->Transform()->SetRelativePos(_vWorldPos);

	tEvent evn = {};

	evn.Type = EVENT_TYPE::CREATE_OBJECT;
	evn.wParam = (DWORD_PTR)_NewObject;
	evn.lParam = (DWORD_PTR)CLevelMgr::GetInst()->GetCurLevel()->FindLayerByName(_strLayer)->GetLayerIdx();

	CEventMgr::GetInst()->AddEvent(evn);
}

void AddChildObject(CGameObject* _pParent, CGameObject* _pChild)
{
	tEvent evn = {};
	evn.Type = EVENT_TYPE::ADD_CHILD;
	evn.wParam = (DWORD_PTR)_pParent;
	evn.lParam = (DWORD_PTR)_pChild;
	CEventMgr::GetInst()->AddEvent(evn);
}

void DestoryObject(CGameObject* _DeleteObj)
{
	if (_DeleteObj->IsDead())
		return;

	tEvent evn = {};

	evn.Type = EVENT_TYPE::DELETE_OBJECT;
	evn.wParam = (DWORD_PTR)_DeleteObj;

	CEventMgr::GetInst()->AddEvent(evn);
}

void InitScript(CScript* _pScript)
{
	if (nullptr == _pScript)
		return;

	tEvent evn = {};
	evn.Type = EVENT_TYPE::INIT_SCRIPT;
	evn.wParam = (DWORD_PTR)_pScript;

	CEventMgr::GetInst()->AddEvent(evn);
}

void DrawDebugRect(Vec3 _vWorldPos, Vec3 _vWorldScale, Vec3 _vRotation, Vec4 _vColor, float _fTime)
{
	tDebugShapeInfo info = {};

	info.matWorld = XMMatrixIdentity();
	info.eShape = SHAPE_TYPE::RECT;
	info.vWorldPos = _vWorldPos;
	info.vWorldScale = _vWorldScale;
	info.vWorldRotation = _vRotation;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugRect(const Matrix& _matWorld, Vec4 _vColor, float _fTime)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	info.eShape = SHAPE_TYPE::RECT;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(Vec3 _vWorldPos, float _fRadius, Vec3 _vRotation, Vec4 _vColor, float _fTime)
{
	tDebugShapeInfo info = {};

	info.matWorld = XMMatrixIdentity();
	info.eShape = SHAPE_TYPE::CIRCLE;
	info.vWorldPos = _vWorldPos;
	info.vWorldScale = Vec3(_fRadius * 2.f, _fRadius * 2.f, 1.f);
	info.vWorldRotation = _vRotation;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(const Matrix& _matWorld, Vec4 _vColor, float _fTime)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	info.eShape = SHAPE_TYPE::CIRCLE;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugTri(Vec3 _vWorldPos, Vec3 _vWorldScale, Vec3 _vRotation, Vec4 _vColor, float _fTime)
{
	tDebugShapeInfo info = {};

	info.matWorld = XMMatrixIdentity();
	info.eShape = SHAPE_TYPE::TRI;
	info.vWorldPos = _vWorldPos;
	info.vWorldScale = _vWorldScale;
	info.vWorldRotation = _vRotation;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugTri(const Matrix& _matWorld, Vec4 _vColor, float _fTime)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	info.eShape = SHAPE_TYPE::TRI;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void ConvertMatrixToFloatArray(const Matrix& _mat, float* _pArray)
{
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, _mat);
	memcpy(_pArray, &temp, sizeof(float) * 16);
}

void ConvertFloatArrayToMatrix(Matrix* _mat, const float* _pArray)
{
	_mat->_11 = _pArray[0];
	_mat->_12 = _pArray[1];
	_mat->_13 = _pArray[2];
	_mat->_14 = _pArray[3];

	_mat->_21 = _pArray[4];
	_mat->_22 = _pArray[5];
	_mat->_23 = _pArray[6];
	_mat->_24 = _pArray[7];

	_mat->_31 = _pArray[8];
	_mat->_32 = _pArray[9];
	_mat->_33 = _pArray[10];
	_mat->_34 = _pArray[11];

	_mat->_41 = _pArray[12];
	_mat->_42 = _pArray[13];
	_mat->_43 = _pArray[14];
	_mat->_44 = _pArray[15];
}

const char* ToString(RES_TYPE _type)
{
	return RES_TYPE_STR[(UINT)_type];
}

const wchar_t* ToWString(RES_TYPE _type)
{
	return RES_TYPE_WSTR[(UINT)_type];
}

const char* ToString(COMPONENT_TYPE _type)
{
	return COMPONENT_TYPE_STR[(UINT)_type];
}

const wchar_t* ToWString(COMPONENT_TYPE _type)
{
	return COMPONENT_TYPE_WSTR[(UINT)_type];
}

void SaveWString(const wstring& _str, FILE* _pFile)
{
	// 입력할 문자열 길이를 저장한다
	size_t iLen = _str.length();
	fwrite(&iLen, sizeof(size_t), 1, _pFile);
	// 문자열을 저장한다
	fwrite(_str.c_str(), sizeof(wchar_t), iLen, _pFile);
}

void LoadWString(wstring& _str, FILE* _pFile)
{
	// 저장한 문자열 길이를 불러온다
	size_t iLen = 0;
	fread(&iLen, sizeof(size_t), 1, _pFile);
	// 윈도우 파일 경로는 255 비트를 넘길수 없기에 버퍼를 만들어서 통으로 불러온다
	wchar_t szBuff[256] = {};
	fread(szBuff, sizeof(wchar_t), iLen, _pFile);
	// 불러온 버퍼를 저장할 문자열에 복사한다
	_str = szBuff;
}

void Savestring(const string& _str, FILE* _pFile)
{
	// 입력할 문자열 길이를 저장한다
	size_t iLen = _str.length();
	fwrite(&iLen, sizeof(size_t), 1, _pFile);
	// 문자열을 저장한다
	fwrite(_str.c_str(), sizeof(char), iLen, _pFile);
}

void Loadstring(string& _str, FILE* _pFile)
{
	// 저장한 문자열 길이를 불러온다
	size_t iLen = 0;
	fread(&iLen, sizeof(size_t), 1, _pFile);
	// 윈도우 파일 경로는 255 비트를 넘길수 없기에 버퍼를 만들어서 통으로 불러온다
	char szBuff[256] = {};
	fread(szBuff, sizeof(char), iLen, _pFile);
	// 불러온 버퍼를 저장할 문자열에 복사한다
	_str = szBuff;
}

FILE* SaveFile(const wstring& _strRelativePath, const wstring& _strError)
{
	wstring strPath = CPathMgr::GetInst()->GetContentPath();
	strPath += _strRelativePath;

	FILE* pFile = nullptr;
	errno_t iErrNo = _wfopen_s(&pFile, strPath.c_str(), L"wb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Error Number = %d", iErrNo);
		MessageBox(nullptr, szStr, _strError.c_str(), MB_OK);
		return nullptr;
	}

	return pFile;
}

FILE* LoadFile(const wstring& _strRelativePath, const wstring& _strError)
{
	FILE* pFile = nullptr;

	errno_t iErrNo = _wfopen_s(&pFile, _strRelativePath.c_str(), L"rb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Error Number = % d", iErrNo);
		MessageBox(nullptr, szStr, _strError.c_str(), MB_OK);
		return nullptr;
	}
	return pFile;
}

wstring GetFileName(const wstring& _strPath)
{
	if (wstring::npos == _strPath.find(L'.'))
		assert(nullptr);

	size_t t = _strPath.find(L'.');
	wchar_t czBuff[100] = {};

	wcscpy_s(czBuff, _strPath.c_str());
	czBuff[t] = L'\0';
	
	wstring strName;
	// \\ 을 만날때까지 확인하고 짜르기
	wchar_t* ptr = wcsrchr(czBuff, L'\\');

	if (nullptr == ptr)
		strName = czBuff;
	else
	{
		wcscpy_s(czBuff, ptr + 1);
		strName = czBuff;	
	}

	return strName;
}

int GetSaveFilePath(const wstring& _strRelativePath, wstring& _strFileOut)
{
	// open a file name
	OPENFILENAME ofn = {};

	wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
	strContentPath += _strRelativePath;

	wchar_t szFilePath[256] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFilePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = L"ALL\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = strContentPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (false == GetSaveFileName(&ofn))
		return E_FAIL;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	size_t start = strFilePath.length();
	wstring Buff = szFilePath;
	size_t end = Buff.length();
	wstring FilePath = Buff.substr(start, end);
	_strFileOut = FilePath;

	return S_OK;
}

int GetLoadFilePath(const wstring& _strRelativePath, wstring& _strFileOut)
{
	// open a file name
	OPENFILENAME ofn = {};

	wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
	strContentPath += _strRelativePath;

	wchar_t szFilePath[256] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFilePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = L"ALL\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = strContentPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (false == GetOpenFileName(&ofn))
		return E_FAIL;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	size_t start = strFilePath.length();
	wstring Buff = szFilePath;
	size_t end = Buff.length();
	wstring FilePath = Buff.substr(start, end);

	_strFileOut = FilePath;
	return S_OK;
}

int GetRandInt(int _iMaxRange, int _iMinRange)
{
	LARGE_INTEGER llCount;
	QueryPerformanceCounter(&llCount);
	srand((UINT)llCount.QuadPart);

	int iRand = rand();
	iRand = (rand() % (_iMaxRange - _iMinRange + 1)) + _iMinRange;
	return iRand;
}

void SaveResRef(Ptr<CRes> _Res, FILE* _pFile)
{
	int i = 0;
	if (nullptr == _Res)
	{
		fwrite(&i, sizeof(i), 1, _pFile);
	}
	else
	{
		i = 1;
		fwrite(&i, sizeof(i), 1, _pFile);
		SaveWString(_Res->GetKey(), _pFile);
		SaveWString(_Res->GetRelativePath(), _pFile);
	}
}

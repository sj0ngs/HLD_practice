#pragma once

#include <Engine\ptr.h>
#include <Engine\CTexture.h>

class ParamUI
{
private:
	static UINT		g_NextID;

public:
	static void Clear() { g_NextID = 0; }

public:
	static int Param_Int(const string& _strDesc, int* _pData, bool _bDrag = false);
	static int Param_Float(const string& _strDesc, float* _pData, bool _bDrag = false);
	static int Param_Vec2(const string& _strDesc, Vec2* _pData, bool _bDrag = false);
	static int Param_Vec4(const string& _strDesc, Vec4* _pData, bool _bDrag = false);
	static int Param_Mat(const string& _strDesc, Matrix* _pData, bool _bDrag = false);

	static int Param_Tex(const string& _strDesc, Ptr<CTexture>& _Tex, UI* _pUI, UI_DELEGATE_1 _Func);

private:
	static string GetNextName(const string& _strName);
};


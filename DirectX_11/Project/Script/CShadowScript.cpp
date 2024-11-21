#include "pch.h"
#include "CShadowScript.h"

CShadowScript::CShadowScript()	:
	CScript((UINT)SCRIPT_TYPE::SHADOWSCRIPT),
	m_eType(SMALL),
	m_bOnShadow(true)
{
	AddScriptParam(&m_eType, SCRIPT_PARAM::INT, "ShadowType");
	AddScriptParam(&m_vOffset, SCRIPT_PARAM::VEC2, "ShadowOffest");
}

CShadowScript::~CShadowScript()
{
}

void CShadowScript::begin()
{
}

void CShadowScript::tick()
{
	m_vShadowPos = Transform()->GetRelativePos().XY() + m_vOffset;
}

void CShadowScript::LoadFromLevelFile(FILE* _pFile)
{
	CScript::LoadFromLevelFile(_pFile);

	for (size_t i = 0; i < m_vecParam.size(); i++)
	{
		int iType = 0;
		fread(&iType, sizeof(int), 1, _pFile);
		m_vecParam[i].eParam = (SCRIPT_PARAM)iType;

		int iData = 0;
		float fData = 0;
		Vec2 v2Data;
		Vec3 v3Data;
		Vec4 v4Data;
		Ptr<CTexture> pTex = nullptr;
		switch (m_vecParam[i].eParam)
		{
		case SCRIPT_PARAM::INT:
			fread(&iData, sizeof(iData), 1, _pFile);
			break;
		case SCRIPT_PARAM::FLOAT:
			fread(&fData, sizeof(fData), 1, _pFile);
			break;
		case SCRIPT_PARAM::VEC2:
			fread(&v2Data, sizeof(v2Data), 1, _pFile);
			break;
		case SCRIPT_PARAM::VEC3:
			fread(&v3Data, sizeof(v3Data), 1, _pFile);
			break;
		case SCRIPT_PARAM::VEC4:
			fread(&v4Data, sizeof(v4Data), 1, _pFile);
			break;
		case SCRIPT_PARAM::TEX:
			LoadResRef(pTex, _pFile);
			break;
		}

		Loadstring(m_vecParam[i].strDesc, _pFile);

		if (m_vecParam[i].strDesc == "ShadowType")
			m_eType = (EShadowType)iData;
		else if (m_vecParam[i].strDesc == "ShadowOffest")
			m_vOffset = v2Data;
	}
}

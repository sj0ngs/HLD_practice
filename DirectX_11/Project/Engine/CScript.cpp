#include "pch.h"
#include "CScript.h"

CScript::CScript(UINT _iScriptType)	:
	CComponent(COMPONENT_TYPE::SCRIPT),
	m_iScriptType(_iScriptType)
{
}

CScript::~CScript()
{
}

void CScript::SaveToLevelFile(FILE* _pFile)
{
	size_t iSize = m_vecParam.size();
	fwrite(&iSize, sizeof(iSize), 1, _pFile);

	for (size_t i = 0; i < m_vecParam.size(); i++)
	{
		// 타입 저장
		int iType = (int)m_vecParam[i].eParam;
		fwrite(&iType, sizeof(iType), 1, _pFile);

		// 파라미터 기준으로 데이터 저장
		switch (m_vecParam[i].eParam)
		{
		case SCRIPT_PARAM::INT:
		{
			int iData = *(int*)m_vecParam[i].pData;
			fwrite(&iData, sizeof(iData), 1, _pFile);
		}
			break;
		case SCRIPT_PARAM::FLOAT:
		{
			float fData = *(float*)m_vecParam[i].pData;
			fwrite(&fData, sizeof(fData), 1, _pFile);
		}
			break;
		case SCRIPT_PARAM::VEC2:
		{
			Vec2 v2Data = *(Vec2*)m_vecParam[i].pData;
			fwrite(&v2Data, sizeof(v2Data), 1, _pFile);
		}
			break;
		case SCRIPT_PARAM::VEC3:
		{
			Vec3 v3Data = *(Vec3*)m_vecParam[i].pData;
			fwrite(&v3Data, sizeof(v3Data), 1, _pFile);
		}
			break;
		case SCRIPT_PARAM::VEC4:
		{
			Vec4 v4Data = *(Vec4*)m_vecParam[i].pData;
			fwrite(&v4Data, sizeof(v4Data), 1, _pFile);
		}
			break;
		case SCRIPT_PARAM::TEX:
			Ptr<CTexture> pTex = (CTexture*)m_vecParam[i].pData;
			SaveResRef(pTex.Get(), _pFile);
		}

		// 디스크립션 저장
		Savestring(m_vecParam[i].strDesc, _pFile);
	}
}

void CScript::LoadFromLevelFile(FILE* _pFile)
{
	size_t iSize = 0;
	fread(&iSize, sizeof(iSize), 1, _pFile);
	m_vecParam.resize(iSize);
}

void CScript::AddScriptParam(void* _pData, SCRIPT_PARAM eParam, const string& _strDesc)
{
	m_vecParam.push_back(tScriptParam{ _pData, eParam, _strDesc });
}



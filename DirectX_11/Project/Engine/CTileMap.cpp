#include "pch.h"
#include "CTileMap.h"

#include "CResMgr.h"
#include "CPathMgr.h"
#include "CTransform.h"

#include "CStructuredBuffer.h"

CTileMap::CTileMap()	:
	CRenderComponent(COMPONENT_TYPE::TILEMAP),
	m_iTileCountX(1),
	m_iTileCountY(1)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TileMapMtrl"));
	GetDynamicMaterial();

	m_SB = new CStructuredBuffer();
	m_SB->Create(sizeof(tTile), m_iTileCountX * m_iTileCountY, SB_TYPE::READ_ONLY, true);
}

CTileMap::CTileMap(const CTileMap& _Origin)	:
	CRenderComponent(_Origin),
	m_iTileCountX(_Origin.m_iTileCountX),
	m_iTileCountY(_Origin.m_iTileCountY),	
	m_vSliceSize(_Origin.m_vSliceSize),
	m_vecTile(_Origin.m_vecTile)
{
	m_SB = new CStructuredBuffer();
	m_SB->Create(sizeof(tTile), m_iTileCountX * m_iTileCountY, SB_TYPE::READ_ONLY, true);
}

CTileMap::~CTileMap()
{
	if (nullptr != m_SB)
	{
		delete m_SB;
	}
}

void CTileMap::finaltick()
{
}

void CTileMap::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial())
		return;

	// Transform 에  UpdateData 요청
	Transform()->UpdateData();

	GetMaterial()->SetScalarParam(INT_0, &m_iTileCountX);
	GetMaterial()->SetScalarParam(INT_1, &m_iTileCountY);
	GetMaterial()->UpdateData();

	// 타일 구조화 버퍼 업데이트
	UpdateData();

	GetMesh()->render();
}

void CTileMap::UpdateData()
{
	m_SB->SetData(m_vecTile.data(), (UINT)(sizeof(tTile) * m_vecTile.size()));
	m_SB->UpdateData(20, PIPELINE_STAGE::PS_PIXEL);
}

void CTileMap::SaveToLevelFile(FILE* _File)
{
	CRenderComponent::SaveToLevelFile(_File);

	fwrite(&m_iTileCountX, sizeof(UINT), 1, _File);
	fwrite(&m_iTileCountY, sizeof(UINT), 1, _File);
	fwrite(&m_vSliceSize, sizeof(Vec2), 1, _File);
	fwrite(m_vecTile.data(), sizeof(tTile), m_vecTile.size(), _File);
}

void CTileMap::LoadFromLevelFile(FILE* _File)
{
	CRenderComponent::LoadFromLevelFile(_File);

	fread(&m_iTileCountX, sizeof(UINT), 1, _File);
	fread(&m_iTileCountY, sizeof(UINT), 1, _File);
	fread(&m_vSliceSize, sizeof(Vec2), 1, _File);

	SetTileCount(m_iTileCountX, m_iTileCountY);

	fread(m_vecTile.data(), sizeof(tTile), m_iTileCountX * m_iTileCountY, _File);
}


void CTileMap::SetTileCount(UINT _iXCount, UINT _iYCount)
{
	m_iTileCountX = _iXCount;
	m_iTileCountY = _iYCount;

	m_vecTile.clear();
	m_vecTile.resize(m_iTileCountX * m_iTileCountY);

	if (m_SB->GetElementCount() < m_vecTile.size())
	{
		m_SB->Create(sizeof(tTile), (UINT)m_vecTile.size(), SB_TYPE::READ_ONLY, true);
	}

	for (size_t i = 0; i < m_iTileCountY; i++)
	{
		for (size_t j = 0; j < m_iTileCountX; j++)
		{
			m_vecTile[i * m_iTileCountX + j].vLeftTop.x = m_vSliceSize.x * j;
			m_vecTile[i * m_iTileCountX + j].vLeftTop.y = 0.f;
			m_vecTile[i * m_iTileCountX + j].vSlice = m_vSliceSize;
		}
	}
	
	m_vecTile[0].vLeftTop = Vec2(m_vSliceSize.x * 7.f, m_vSliceSize.y * 5.f);
}

int CTileMap::Save(const wstring& _strRelativePath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	// 파일 입출력
	FILE* pFile = nullptr;
	errno_t iErrNo = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"TileMap Save 실패, Error Number = %d", iErrNo);
		MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
		return FALSE;
	}

	fwprintf_s(pFile, L"[ATLAS_KEY]\n");
	fwprintf_s(pFile, GetMaterial()->GetTexParam(TEX_0)->GetKey().c_str());
	fwprintf_s(pFile, L"\n\n");

	fwprintf_s(pFile, L"[ATLAS_PATH]\n");
	fwprintf_s(pFile, GetMaterial()->GetTexParam(TEX_0)->GetRelativePath().c_str());
	fwprintf_s(pFile, L"\n\n");

	fwprintf_s(pFile, L"[TILE_SLICE]\n");
	fwprintf_s(pFile, L"%d %d\n", m_iTileCountX, m_iTileCountY);
	fwprintf_s(pFile, L"\n");

	fwprintf_s(pFile, L"[SLICE_SIZE]\n");
	fwprintf_s(pFile, L"%f %f\n", m_vSliceSize.x, m_vSliceSize.y);
	fwprintf_s(pFile, L"\n");

	fwprintf_s(pFile, L"[TILE_COUNT]\n");
	wchar_t szNum[50] = {};
	size_t iTileCount = m_vecTile.size();
	_itow_s((int)iTileCount, szNum, 50, 10);
	fwprintf_s(pFile, szNum);
	fwprintf_s(pFile, L"\n\n");

	for (size_t i = 0; i < iTileCount; i++)
	{
		fwprintf_s(pFile, L"[%zd_TILE]\n", i);

		// LEFT_TOP
		fwprintf_s(pFile, L"[LEFT_TOP]\n");
		fwprintf_s(pFile, L"%f %f\n", m_vecTile[i].vLeftTop.x, m_vecTile[i].vLeftTop.y);

		// SIZE
		fwprintf_s(pFile, L"[SIZE]\n");
		fwprintf_s(pFile, L"%f %f\n", m_vecTile[i].vSlice.x, m_vecTile[i].vSlice.y);

		fwprintf_s(pFile, L"\n");
	}

	fclose(pFile);

	return TRUE;
}

int CTileMap::Load(const wstring& _strRelativePath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	// 파일 입출력
	FILE* pFile = nullptr;
	errno_t iErrNo = _wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"TileMap Load 실패, Error Number = %d", iErrNo);
		MessageBox(nullptr, szStr, L"파일 로드 실패", MB_OK);
		return FALSE;
	}

	wstring strAtlasKey;
	wstring strAtlasPath;
	size_t iTileCount = 0;

	while (true)
	{
		wchar_t szBuffer[256] = {};
		fwscanf_s(pFile, L"%s", szBuffer, 256);

		if (!wcscmp(szBuffer, L"[ATLAS_KEY]"))
		{
			fwscanf_s(pFile, L"%s", szBuffer, 256);
			strAtlasKey = szBuffer;
		}
		else if (!wcscmp(szBuffer, L"[ATLAS_PATH]"))
		{
			fwscanf_s(pFile, L"%s", szBuffer, 256);
			strAtlasPath = szBuffer;
		}
		else if (!wcscmp(szBuffer, L"[TILE_SLICE]"))
		{
			int iTileX = 0, iTileY = 0;
			fwscanf_s(pFile, L"%d %d\n", &iTileX, &iTileY);
			SetTileCount(iTileX, iTileY);
		}
		else if (!wcscmp(szBuffer, L"[SLICE_SIZE]"))
		{
			fwscanf_s(pFile, L"%f %f\n", &m_vSliceSize.x, &m_vSliceSize.y);
		}
		else if (!wcscmp(szBuffer, L"[TILE_COUNT]"))
		{
			fwscanf_s(pFile, L"%zd", &iTileCount);

			for (size_t i = 0; i < iTileCount; i++)
			{
				while (true)
				{
					wchar_t szBuffer[256] = {};
					fwscanf_s(pFile, L"%s", szBuffer, 256);

					if (!wcscmp(szBuffer, L"[LEFT_TOP]"))
					{
						fwscanf_s(pFile, L"%f %f", &m_vecTile[i].vLeftTop.x, &m_vecTile[i].vLeftTop.y);
					}
					else if (!wcscmp(szBuffer, L"[SIZE]"))
					{
						fwscanf_s(pFile, L"%f %f", &m_vecTile[i].vSlice.x, &m_vecTile[i].vSlice.y);
						break;
					}
				}
			}
			break;
		}
	}

	// 불러온 정보로 아틀라스 텍스쳐 생성해주기
	Ptr<CTexture> pAtlasTex = CResMgr::GetInst()->LoadRes<CTexture>(strAtlasKey, strAtlasPath);
	GetMaterial()->SetTexParam(TEX_0, pAtlasTex);

	fclose(pFile);

	return TRUE;
}

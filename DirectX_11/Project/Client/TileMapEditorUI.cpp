#include "pch.h"
#include "TileMapEditorUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CPathMgr.h>
#include <Engine/CTileMap.h>

#include "TileMapCreateUI.h"

TileMapEditorUI::TileMapEditorUI() :
	UI("##TileMapEditor"),
	m_pTileMap(nullptr),
	m_iTileCountX(1),
	m_iTileCountY(1),
	m_vSliceSize(0.f, 0.f),
	m_iSelectTile(0),
	m_iSliceCountX(1),
	m_iSliceCountY(1)
{
	SetName("TileMapEditor");
	AddWindowFlags(ImGuiWindowFlags_NoDocking);
}	

TileMapEditorUI::~TileMapEditorUI()
{
}

void TileMapEditorUI::finaltick()
{
	UI::finaltick();
}

int TileMapEditorUI::render_update()
{
	EditTile();
	ShowTileList();
	ShowTileMap();

	return TRUE;
}


void TileMapEditorUI::EditTile()
{
	ImGui::BeginDisabled(nullptr == m_pTileTex);
	if (ImGui::Button("Save"))
	{
		SaveTileMap();
	}
	ImGui::EndDisabled();

	if (nullptr != m_pTileMap)
	{
		ImGui::SameLine();
		if (ImGui::Button("Apply"))
		{
			m_pTileMap->SetTileCount(m_iTileCountX, m_iTileCountY);
			m_pTileMap->SetTiles(m_vecMap);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		LoadTileMap();
	}

	ImGui::SameLine();

	if (ImGui::Button("Create"))
	{
		TileMapCreateUI* pUI = (TileMapCreateUI*)ImGuiMgr::GetInst()->FindUI("##TileMapCreate");
		assert(pUI);
		pUI->Reset();
		pUI->SetActive(true);
	}

	int iStep = 1;
	ImVec2 vContentSize = ImGui::GetContentRegionAvail();
	if (ImGui::BeginChild("##Data", ImVec2(vContentSize.x / 2.f, 100.f)))
	{
		ImGui::Text("Row   ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.f);
		if (ImGui::InputScalar("##TileCountX", ImGuiDataType_U16, &m_iTileCountX, &iStep))
		{
			if (1 > m_iTileCountX)
				m_iTileCountX = 1;
		}

		ImGui::Text("Column");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.f);
		if (ImGui::InputScalar("##TileCountY", ImGuiDataType_U16, &m_iTileCountY, &iStep))
		{
			if (1 > m_iTileCountY)
				m_iTileCountY = 1;
		}

		ImGui::EndChild();
	}

	ImGui::SameLine();

	if(ImGui::BeginChild("##Current", ImVec2(vContentSize.x / 2.f, 100.f)))
	{
		ImGui::Text("Current Tile");
		ImGui::SetNextItemWidth(80.f);
		ImGui::SameLine();

		int iMax = (int)m_vecTile.size() - 1;
		if (0 == m_vecTile.size())
		{
			iMax = 0;
		}
		ImGui::SliderInt("##Current", (int*)&m_iSelectTile, 0, iMax);

		if (nullptr != m_pTileTex)
		{
			ImGui::Image((void*)m_pTileTex->GetSRV().Get(),
				ImVec2(50.f, 50.f),
				ImVec2(m_vecTile[m_iSelectTile].vLeftTop.x, m_vecTile[m_iSelectTile].vLeftTop.y),
				ImVec2(m_vecTile[m_iSelectTile].vLeftTop.x + m_vecTile[m_iSelectTile].vSlice.x,
					m_vecTile[m_iSelectTile].vLeftTop.y + m_vecTile[m_iSelectTile].vSlice.y),
				ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f));
		}
		ImGui::EndChild();
	}
}

void TileMapEditorUI::ShowTileList()
{
	if (nullptr == m_pTileTex)
		return;

	ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
	ImVec2 vContentSize = ImGui::GetContentRegionAvail();
	if (ImGui::BeginChild("TileList", ImVec2(vContentSize.x, 80.f), true, child_flags))
	{
		for (size_t i = 0; i < m_vecTile.size(); i++)
		{
			ImGui::PushID((int)i);
			if (m_iSelectTile == i)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 1.f, 0.f, 1.f));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 1.f));
			if (ImGui::ImageButton((void*)m_pTileTex->GetSRV().Get(),
				ImVec2(50.f, 50.f),
				ImVec2(m_vecTile[i].vLeftTop.x, m_vecTile[i].vLeftTop.y),
				ImVec2(m_vecTile[i].vLeftTop.x + m_vecTile[i].vSlice.x,
					m_vecTile[i].vLeftTop.y + m_vecTile[i].vSlice.y)))
			{
				m_iSelectTile = (UINT)i;
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();

			if (m_iSelectTile == i)
				ImGui::SetScrollHereX();

			ImGui::SameLine();
		}

		ImGui::EndChild();
	}
}

void TileMapEditorUI::ShowTileMap()
{
	if (nullptr == m_pTileTex)
		return;

	m_vecMap.resize((size_t)m_iTileCountX * m_iTileCountY);

	Vec2 vSize = Vec2(100.f, 100.f);

	ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
	ImVec2 vContentSize = ImGui::GetContentRegionAvail();
	if (ImGui::BeginChild("TileMap", vContentSize, true, child_flags))
	{
		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		vContentSize = ImGui::GetContentRegionAvail();
		for (int i = 0; i < (int)m_iTileCountY; i++)
		{
			for (int j = 0; j < (int)m_iTileCountX; j++)
			{
				int Idx = m_iTileCountX * i + j;
				ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();

				ImGui::PushID(Idx);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 1.f, 0.f, 1.f));

				void* img = (void*)m_pTileTex->GetSRV().Get();
				if (m_vecMap[Idx].vLeftTop == Vec2(0.f, 0.f) && m_vecMap[Idx].vSlice == Vec2(0.f,0.f))
				{
					img = nullptr;
				}

				if (ImGui::ImageButton(img,
					ImVec2(40.f, 40.f),
					ImVec2(m_vecMap[Idx].vLeftTop.x, m_vecMap[Idx].vLeftTop.y),
					ImVec2(m_vecMap[Idx].vLeftTop.x + m_vecMap[Idx].vSlice.x,
						m_vecMap[Idx].vLeftTop.y + m_vecMap[Idx].vSlice.y)))
				{
					m_vecMap[Idx] = m_vecTile[m_iSelectTile];
				}
				ImGui::PopStyleColor(2);

				string strIdx = "(" + std::to_string(j) + "," + std::to_string(i) + ")";
				draw_list->AddText(canvas_p0, IM_COL32(255.f, 255.f, 255.f, 255.f), strIdx.c_str());

				if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
				{
					m_vecMap[Idx] = m_vecTile[m_iSelectTile];
				}
				else if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
				{
					m_vecMap[Idx].vLeftTop = Vec2(0.f, 0.f);
					m_vecMap[Idx].vSlice = Vec2(0.f, 0.f);
				}

				ImGui::PopID();


				if (j == m_iTileCountX - 1)
					continue;

				ImGui::SameLine();
			}
		}
		ImGui::EndChild();
	}
}

void TileMapEditorUI::SetSliceCount(UINT _iSliceX, UINT _iSliceY)
{
	m_iSliceCountX = _iSliceX;
	m_iSliceCountY = _iSliceY;

	m_vSliceSize = Vec2(1.f / (float)m_iSliceCountX, 1.f / (float)m_iSliceCountY);

	m_vecTile.clear();
	m_vecTile.resize((size_t)_iSliceX * _iSliceY);

	for (int i = 0; i < (int)m_iSliceCountY; i++)
	{
		for (int j = 0; j < (int)m_iSliceCountX; j++)
		{
			int Idx = m_iSliceCountX * i + j;
			m_vecTile[Idx].vLeftTop.x = m_vSliceSize.x * j;
			m_vecTile[Idx].vLeftTop.y = m_vSliceSize.y * i;
			m_vecTile[Idx].vSlice = m_vSliceSize;
		}
	}
}

void TileMapEditorUI::SaveTileMap()
{
	// open a file name
	OPENFILENAME ofn = {};

	wstring strTileFolderPath = CPathMgr::GetInst()->GetContentPath();
	strTileFolderPath += L"tilemap\\";

	wchar_t szFilePath[256] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFilePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = L"tile\0*.tile\0ALL\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = strTileFolderPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (false == GetSaveFileName(&ofn))
		return;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	size_t start = strFilePath.length();
	wstring Buff = szFilePath;
	size_t end = Buff.length();
	wstring FilePath = Buff.substr(start, end);

	Save(FilePath);
}

void TileMapEditorUI::LoadTileMap()
{
	// open a file name
	OPENFILENAME ofn = {};

	wstring strTileFolderPath = CPathMgr::GetInst()->GetContentPath();
	strTileFolderPath += L"tilemap\\";

	wchar_t szFilePath[256] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFilePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = L"tile\0*.tile\0ALL\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = strTileFolderPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (false == GetOpenFileName(&ofn))
		return;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	size_t start = strFilePath.length();
	wstring Buff = szFilePath;
	size_t end = Buff.length();
	wstring FilePath = Buff.substr(start, end);

	Load(FilePath);
}

void TileMapEditorUI::Clear()
{
	m_pTileMap = nullptr;
	m_pTileTex = nullptr;
	m_iTileCountX = 1;
	m_iTileCountY = 1;
	m_vSliceSize = Vec2{};
	m_iSelectTile = 0;
	m_iSliceCountX = 1;
	m_iSliceCountY = 1;
	m_vecTile.clear();
	m_vecMap.clear();
}

void TileMapEditorUI::Reset(CTileMap* _pTileMap, Ptr<CTexture> _pTex, UINT _iTileCountX, UINT _iTileCountY, 
					Vec2 _m_vSliceSize, const vector<tTile> _vecTile)
{
	Clear();
	SetActive(true);
	m_pTileMap = _pTileMap;
	SetAtlasTex(_pTex);

	m_iTileCountX = _iTileCountX;
	m_iTileCountY = _iTileCountY;

	int iSliecCountX = (int)roundf((1 / _m_vSliceSize.x));
	int iSliecCountY = (int)roundf((1 / _m_vSliceSize.y));
	SetSliceCount(iSliecCountX, iSliecCountY);

	m_vecMap = _vecTile;
}

int TileMapEditorUI::Save(const wstring& _strRelativePath)
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
	fwprintf_s(pFile, m_pTileTex->GetKey().c_str());
	fwprintf_s(pFile, L"\n\n");

	fwprintf_s(pFile, L"[ATLAS_PATH]\n");
	fwprintf_s(pFile, m_pTileTex->GetRelativePath().c_str());
	fwprintf_s(pFile, L"\n\n");

	fwprintf_s(pFile, L"[TILE_SLICE]\n");
	fwprintf_s(pFile, L"%d %d\n", m_iTileCountX, m_iTileCountY);
	fwprintf_s(pFile, L"\n");

	fwprintf_s(pFile, L"[SLICE_SIZE]\n");
	fwprintf_s(pFile, L"%f %f\n", m_vSliceSize.x, m_vSliceSize.y);
	fwprintf_s(pFile, L"\n");

	fwprintf_s(pFile, L"[TILE_COUNT]\n");
	wchar_t szNum[50] = {};
	size_t iTileCount = m_vecMap.size();
	_itow_s((int)iTileCount, szNum, 50, 10);
	fwprintf_s(pFile, szNum);
	fwprintf_s(pFile, L"\n\n");

	for (size_t i = 0; i < iTileCount; i++)
	{
		fwprintf_s(pFile, L"[%zd_TILE]\n", i);

		// LEFT_TOP
		fwprintf_s(pFile, L"[LEFT_TOP]\n");
		fwprintf_s(pFile, L"%f %f\n", m_vecMap[i].vLeftTop.x, m_vecMap[i].vLeftTop.y);

		// SIZE
		fwprintf_s(pFile, L"[SIZE]\n");
		fwprintf_s(pFile, L"%f %f\n", m_vecMap[i].vSlice.x, m_vecMap[i].vSlice.y);

		fwprintf_s(pFile, L"\n");
	}

	fclose(pFile);
	return TRUE;
}

int TileMapEditorUI::Load(const wstring& _strRelativePath)
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
			// 불러온 정보로 아틀라스 텍스쳐 생성해주기
			m_pTileTex = CResMgr::GetInst()->LoadRes<CTexture>(strAtlasKey, strAtlasPath);
		}
		else if (!wcscmp(szBuffer, L"[TILE_SLICE]"))
		{
			fwscanf_s(pFile, L"%d %d\n", &m_iTileCountX, &m_iTileCountY);
		}
		else if (!wcscmp(szBuffer, L"[SLICE_SIZE]"))
		{
			fwscanf_s(pFile, L"%f %f\n", &m_vSliceSize.x, &m_vSliceSize.y);
			int iSliecCountX = (int)roundf((1 / m_vSliceSize.x));
			int iSliecCountY = (int)roundf((1 / m_vSliceSize.y));
			SetSliceCount(iSliecCountX, iSliecCountY);
		}
		else if (!wcscmp(szBuffer, L"[TILE_COUNT]"))
		{
			fwscanf_s(pFile, L"%zd", &iTileCount);

			m_vecMap.clear();
			for (size_t i = 0; i < iTileCount; i++)
			{
				tTile tile = {};
				while (true)
				{
					wchar_t szBuffer[256] = {};
					fwscanf_s(pFile, L"%s", szBuffer, 256);

					if (!wcscmp(szBuffer, L"[LEFT_TOP]"))
					{
						fwscanf_s(pFile, L"%f %f", &tile.vLeftTop.x, &tile.vLeftTop.y);
					}
					else if (!wcscmp(szBuffer, L"[SIZE]"))
					{
						fwscanf_s(pFile, L"%f %f", &tile.vSlice.x, &tile.vSlice.y);
						break;
					}
				}
				m_vecMap.push_back(tile);
			}
			break;
		}
	}

	fclose(pFile);

	return TRUE;
}
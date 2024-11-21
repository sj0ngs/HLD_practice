#include "pch.h"
#include "TileMapCreateUI.h"

#include "TileMapEditorUI.h"

TileMapCreateUI::TileMapCreateUI()	:
	UI("##TileMapCreate"),
	m_pTileTex(nullptr),
	m_iSliceCountX(1),
	m_iSliceCountY(1)
{
	SetName("TileMapCreate");
	SetSize(300.f, 500.f);
}

TileMapCreateUI::~TileMapCreateUI()
{
}


void TileMapCreateUI::finaltick()
{
	UI::finaltick();
}

int TileMapCreateUI::render_update()
{
	if (ImGui::Button("Load Texture"))
	{
		m_pTileTex = LoadTexFromFile();
	}

	ImVec2 vContentSize = ImGui::GetContentRegionAvail();
	ImVec2 vStartPos = ImGui::GetCursorScreenPos();
	if (nullptr != m_pTileTex)
	{
		float fAspect = (float)m_pTileTex->Width() / (float)m_pTileTex->Height();
		ImVec2 vImageSize = ImVec2(vContentSize.x, vContentSize.x / fAspect);
		ImGui::Image((void*)m_pTileTex->GetSRV().Get(), vImageSize, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), 
					ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f));

		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
	
		if (1 < m_iSliceCountX)
		{
			float fWidth = vContentSize.x / m_iSliceCountX;
			for (int i = 1; i < (int)m_iSliceCountX; i++)
			{
				draw_list->AddLine(ImVec2(vStartPos.x + fWidth * i, vStartPos.y), 
								ImVec2(vStartPos.x + fWidth * i, vStartPos.y + vImageSize.y), 
								IM_COL32(255, 255, 0, 255), 2.f);
			}
		}

		if (1 < m_iSliceCountY)
		{
			float fHeight = vImageSize.y / m_iSliceCountY;
			for (int i = 1; i < (int)m_iSliceCountY; i++)
			{
				draw_list->AddLine(ImVec2(vStartPos.x, vStartPos.y + fHeight * i),
					ImVec2(vStartPos.x + vImageSize.x, vStartPos.y + fHeight * i),
					IM_COL32(255, 255, 0, 255), 2.f);
			}
		}

		int iStep = 1;
		ImGui::Text("Horizontal Count");
		ImGui::SameLine();
		vContentSize = ImGui::GetContentRegionAvail();
		ImGui::SetNextItemWidth(vContentSize.x);
		if (ImGui::InputScalar("##SliceX", ImGuiDataType_U16, &m_iSliceCountX, &iStep))
		{
			if (1 > m_iSliceCountX)
				m_iSliceCountX = 1;
		}

		ImGui::Text("Vertical Count  ");
		ImGui::SameLine();
		vContentSize = ImGui::GetContentRegionAvail();
		ImGui::SetNextItemWidth(vContentSize.x);
		if (ImGui::InputScalar("##SliceY", ImGuiDataType_U16, &m_iSliceCountY, &iStep))
		{
			if (1 > m_iSliceCountY)
				m_iSliceCountY = 1;
		}

		if (ImGui::Button("Confrim"))
		{
			TileMapEditorUI* pUI = (TileMapEditorUI*)ImGuiMgr::GetInst()->FindUI("##TileMapEditor");
			assert(pUI);
			pUI->Clear();
			pUI->SetAtlasTex(m_pTileTex);
			pUI->SetSliceCount(m_iSliceCountX, m_iSliceCountY);
			
			Reset();
			SetActive(false);
		}
	}

	return TRUE;
}

void TileMapCreateUI::Reset()
{
	Clear();

	SetActive(true);

	// 위치 조정
	HWND hWnd = ImGuiMgr::GetInst()->GetMainHwnd();

	RECT rt = {};

	GetWindowRect(hWnd, &rt);

	float CenterX = (float)((rt.left + rt.right) / 2);
	float CenterY = (float)((rt.top + rt.bottom) / 2);

	SetPopUpPos(ImVec2(CenterX - GetSize().x / 2.f, CenterY - GetSize().y / 2.f));
}
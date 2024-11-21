#include "pch.h"
#include "AnimEditorUI.h"

#include <Engine/CPathMgr.h>
#include <Engine/CResMgr.h>
#include <Engine/CTimeMgr.h>

#include <Engine/CAnimator2D.h>
#include <Engine/CAnim2D.h>
#include "AtlasTextureUI.h"	

AnimEditorUI::AnimEditorUI() :
	UI("##AnimEditor"),
	m_pAnim2D(nullptr),
	m_vBackSize(0.01f, 0.01f),
	m_fFrmScrollX(0.f),
	m_iSelectFrm(0),
	m_vTexSize{},
	m_bBlockEdit(false),
	m_iMode(0),
	m_iOverlayIdx(0),
	m_bOverlay(true),
	m_bGrid(true),
	m_bDeleteFrm(false),
	m_iDeleteIdx(-1)
{
	SetName("Animation Editor");
	AddWindowFlags(ImGuiWindowFlags_NoDocking);
}

AnimEditorUI::~AnimEditorUI()
{
}

void AnimEditorUI::finaltick()
{
	UI::finaltick();
}

int AnimEditorUI::render_update()
{
	ImGui::BeginDisabled(m_bBlockEdit);
	
	// Frame Preview 
	ImVec2 ContentSize = ImGui::GetContentRegionAvail();
	ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;

	if (ImGui::RadioButton("Move", m_iMode == Mode_Move)) { m_iMode = Mode_Move; } ImGui::SameLine();
	if (ImGui::RadioButton("Swap", m_iMode == Mode_Swap)) { m_iMode = Mode_Swap; }

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	if (ImGui::BeginChild("FramePreview", ImVec2(ContentSize.x * 0.9f, 120.f), true, child_flags))
	{
		ImVec2 vWinSize = ImGui::GetWindowSize();
		FramePreview();
		float scroll_max_x = ImGui::GetScrollMaxX();
		m_fFrmScrollX = ImGui::GetScrollX();
		ImGui::EndChild();
	}
	ImGui::PopStyleVar();

	ImGui::SameLine();

	ImGui::BeginDisabled(nullptr == m_pTex);
	if (ImGui::Button("+", ImVec2(35.f, 35.f)))
	{
		AtlasTextureUI* pTexUI = (AtlasTextureUI*)ImGuiMgr::GetInst()->FindUI("##Texture");
		pTexUI->Clear();
		pTexUI->SetActive(true);
		pTexUI->SetName(string(m_pTex->GetKey().begin(), m_pTex->GetKey().end()));
		pTexUI->SetAtlasTex(m_pTex);
		pTexUI->SetVecFrm(m_vecFrm);
	}
	ImGui::EndDisabled();

	if (ImGui::BeginChild("FrameData", ImVec2(ContentSize.x * 0.35f, 400.f), true))
	{
		// Frame Editor
		FrameData();

		ImGui::EndChild();
	}

	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::BeginChild("##AnimPreview", ImVec2(ContentSize.x * 0.35f, 400.f), true))
	{
		PlayAnim();

		ImGui::EndChild();
	}

	ImGui::SameLine();

	if (ImGui::BeginChild("##Save&Load", ImVec2(ContentSize.x * 0.285f, 400.f), true))
	{
		EditAnim();
		ImGui::EndChild();
	}

	if (m_bDeleteFrm)
	{
		if (0 > m_iDeleteIdx || (int)m_vecFrm.size() <= m_iDeleteIdx)
			assert(nullptr);
	
		for (int i = m_iDeleteIdx; i < (int)m_vecFrm.size() - 1; i++)
		{
			m_vecFrm[i] = m_vecFrm[i + 1];
		}
		m_vecFrm.resize(m_vecFrm.size() - 1);

		m_bDeleteFrm = false;
		m_iDeleteIdx = -1;
	}

    return TRUE;
}

void AnimEditorUI::SetAnim2D(CAnim2D* _pAnim)
{
	assert(_pAnim);

	m_vecFrm.clear();
	m_pTex = nullptr;
	m_iSelectFrm = 0;
	m_strName = _pAnim->GetName();

	m_pAnim2D = _pAnim;
	m_vecFrm = m_pAnim2D->GetFrames();

	m_pTex = m_pAnim2D->GetAtlasTex();
	m_vTexSize = ImVec2((float)m_pTex->Width(), (float)m_pTex->Height());
	m_vBackSize = m_pAnim2D->GetBackSize();
}

void AnimEditorUI::SaveAnim()
{
	// open a file name
	OPENFILENAME ofn = {};

	wstring strTileFolderPath = CPathMgr::GetInst()->GetContentPath();
	strTileFolderPath += L"animation\\";

	wchar_t szFilePath[256] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFilePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = L"anim\0*.anim\0ALL\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = strTileFolderPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (false == GetSaveFileName(&ofn))
		return;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	size_t start = strFilePath.length();
	wstring Buff = szFilePath;
	size_t end = Buff.length();
	wstring FilePath = Buff.substr(start, end);

	if (nullptr == m_pAnim2D)
	{
		CAnim2D* pAnim2D = new CAnim2D;
		pAnim2D->Create(m_strName, m_pTex, m_vecFrm, m_vBackSize);
		pAnim2D->Save(FilePath);
		delete pAnim2D;
	}
	else
	{
		m_pAnim2D->Save(FilePath);
	}
}

void AnimEditorUI::LoadAnim()
{
	// open a file name
	OPENFILENAME ofn = {};

	wstring strTileFolderPath = CPathMgr::GetInst()->GetContentPath();
	strTileFolderPath += L"animation\\";

	wchar_t szFilePath[256] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFilePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = L"anim\0*.anim\0ALL\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = strTileFolderPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (false == GetOpenFileName(&ofn))
		return;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	size_t start = strFilePath.length();
	wstring Buff = szFilePath;
	size_t end = Buff.length();
	wstring FilePath = Buff.substr(start, end);

	// 애니메이션 교체
	CAnim2D* pAnim2D = new CAnim2D;
	pAnim2D->Load(FilePath);
	SetAnim2D(pAnim2D);
	delete pAnim2D;
}

void AnimEditorUI::LoadTex()
{
	m_pTex = LoadTexFromFile();
	if (nullptr == m_pTex)
		return;

	m_vTexSize = ImVec2((float)m_pTex->Width(), (float)m_pTex->Height());
}

void AnimEditorUI::Reset()
{
	m_pAnim2D = nullptr;
	m_vecFrm.clear();
	m_pTex = nullptr;
	m_vBackSize = Vec2(0.01f, 0.01f);
	m_strName.clear();

	m_fFrmScrollX = 0.f;
	m_iSelectFrm = 0;

	m_vTexSize = ImVec2(0.f, 0.f);
	m_iOverlayIdx = 0;
}

void AnimEditorUI::FramePreview()
{
	for (int i = 0; i < (int)m_vecFrm.size(); i++)
	{
		ImGui::PushID(i);
		ImGui::PushID(0);
		if (m_iSelectFrm == i)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 1.f));
		}
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.8f, 0.8f, 1.f));

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();

		if (ImGui::ImageButton((void*)(m_pTex->GetSRV().Get()), ImVec2(80, 80),
			ImVec2(m_vecFrm[i].LeftTopUV.x, m_vecFrm[i].LeftTopUV.y),
			ImVec2(m_vecFrm[i].LeftTopUV.x + m_vecFrm[i].SliceUV.x, m_vecFrm[i].LeftTopUV.y + m_vecFrm[i].SliceUV.y)))
		{
			m_iSelectFrm = i;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopID();
		ImGui::SameLine();

		if (m_iSelectFrm == i)
			ImGui::SetScrollHereX();

		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		string strIdx = "[" + std::to_string(i) + "]";
		draw_list->AddText(canvas_p0, IM_COL32(255.f, 255.f, 255.f, 255.f), strIdx.c_str());

		// 우클릭시 프레임 삭제
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			m_bDeleteFrm = true;
			m_iDeleteIdx = i;
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("AnimEdit", &i, sizeof(int));

			if (m_iMode == Mode_Move) { ImGui::Text("Move %d", i); }
			if (m_iMode == Mode_Swap) { ImGui::Text("Swap %d", i); }
			ImGui::ImageButton((void*)(m_pTex->GetSRV().Get()), ImVec2(60, 60),
				ImVec2(m_vecFrm[i].LeftTopUV.x, m_vecFrm[i].LeftTopUV.y),
				ImVec2(m_vecFrm[i].LeftTopUV.x + m_vecFrm[i].SliceUV.x, m_vecFrm[i].LeftTopUV.y + m_vecFrm[i].SliceUV.y));
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AnimEdit"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				if (m_iMode == Mode_Move)
				{
					int iCount = i - payload_n;
					tAnim2DFrm temp = m_vecFrm[payload_n];

					if (0 < iCount)
					{
						for (int j = payload_n; j < i; j++)
						{
							m_vecFrm[j] = m_vecFrm[j + 1];
						}
					}
					else
					{
						for (int j = payload_n; j > i; j--)
						{
							m_vecFrm[j] = m_vecFrm[j - 1];
						}
					}

					m_vecFrm[i] = temp;
				}
				if (m_iMode == Mode_Swap)
				{
					tAnim2DFrm tTemp = m_vecFrm[i];
					m_vecFrm[i] = m_vecFrm[payload_n];
					m_vecFrm[payload_n] = tTemp;
				}
				m_iSelectFrm = i;
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();
	}
}

void AnimEditorUI::FrameData()
{
	ImGui::Text("Frame Count	: %d", (int)m_vecFrm.size());

	if (0 >= m_vecFrm.size())
		return;

	ImGui::Text("Current Frame  ");
	ImGui::SameLine();
	int iStep = 1;
	ImGui::SetNextItemWidth(70.f);
	ImGui::InputScalar("##Index", ImGuiDataType_S8, &m_iSelectFrm, &iStep, NULL, "%d");

	if (0 > m_iSelectFrm)
	{
		m_iSelectFrm = (int)m_vecFrm.size() - 1;
	}
	else if (m_vecFrm.size() <= m_iSelectFrm)
	{
		m_iSelectFrm = 0;
	}

	// BackSize
	ImGui::Text("Back Size(UV)  ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	if (ImGui::InputFloat("##BackSizeUVX", &m_vBackSize.x, 0.001f))
	{
		int a = 0;
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("##BackSizeUVY", &m_vBackSize.y, 0.001f);

	int	iBackSizeX = (int)(m_vBackSize.x * m_vTexSize.x);
	int	iBakcSizeY = (int)(m_vBackSize.y * m_vTexSize.y);
	ImGui::Text("Back Size      ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::DragInt("##BackSizeX", &iBackSizeX, 1.f, 0, (int)m_vTexSize.x);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::DragInt("##BackSizeY", &iBakcSizeY, 1.f, 0, (int)m_vTexSize.y);

	m_vBackSize = Vec2((float)iBackSizeX / m_vTexSize.x, (float)iBakcSizeY / m_vTexSize.y);

	ImGui::Spacing();

	// LeftTop
	ImGui::Text("LeftTop Pos(UV)");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("##LeftTopUVX", &m_vecFrm[m_iSelectFrm].LeftTopUV.x, 0.001f);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("##LeftTopUVY", &m_vecFrm[m_iSelectFrm].LeftTopUV.y, 0.001f);

	int	iLeftTopX = (int)(m_vecFrm[m_iSelectFrm].LeftTopUV.x * m_vTexSize.x);
	int	iLeftTopY = (int)(m_vecFrm[m_iSelectFrm].LeftTopUV.y * m_vTexSize.y);
	ImGui::Text("LeftTop Pos    ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::DragInt("##LeftTopX", &iLeftTopX, 1.f, 0, (int)m_vTexSize.x);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::DragInt("##LeftTopY", &iLeftTopY, 1.f, 0, (int)m_vTexSize.y);

	m_vecFrm[m_iSelectFrm].LeftTopUV = Vec2((float)(iLeftTopX / m_vTexSize.x), (float)(iLeftTopY / m_vTexSize.y));

	ImGui::Spacing();

	// Slice
	ImGui::Text("Slice Size(UV) ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("##SliceUVX", &m_vecFrm[m_iSelectFrm].SliceUV.x, 0.001f);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("##SliceUVY", &m_vecFrm[m_iSelectFrm].SliceUV.y, 0.001f);

	int	iSliceX = (int)(m_vecFrm[m_iSelectFrm].SliceUV.x * m_vTexSize.x);
	int	iSliceY = (int)(m_vecFrm[m_iSelectFrm].SliceUV.y * m_vTexSize.y);
	ImGui::Text("Slice Size     ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::DragInt("##SliceX", &iSliceX, 1.f, 1, (int)m_vTexSize.x);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::DragInt("##SliceY", &iSliceY, 1.f, 1, (int)m_vTexSize.y);

	m_vecFrm[m_iSelectFrm].SliceUV = Vec2((float)(iSliceX / m_vTexSize.x), (float)(iSliceY / m_vTexSize.y));

	ImGui::Spacing();

	// Offset
	ImGui::Text("Offset Size(UV)");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("##OffsetUVX", &m_vecFrm[m_iSelectFrm].Offset.x, 0.001f);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("##OffsetUVY", &m_vecFrm[m_iSelectFrm].Offset.y, 0.001f);

	int	iOffsetX = (int)(m_vecFrm[m_iSelectFrm].Offset.x * m_vTexSize.x);
	int	iOffsetY = (int)(m_vecFrm[m_iSelectFrm].Offset.y * m_vTexSize.y);
	ImGui::Text("Offset         ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::DragInt("##OffsetX", &iOffsetX, 1.f, (int)m_vTexSize.x * -1, (int)m_vTexSize.x);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::DragInt("##OffsetY", &iOffsetY, 1.f, (int)m_vTexSize.y * -1, (int)m_vTexSize.y);

	m_vecFrm[m_iSelectFrm].Offset = Vec2((float)(iOffsetX / m_vTexSize.x), (float)(iOffsetY / m_vTexSize.y));

	ImGui::Spacing();

	// Duration
	ImGui::Text("Duration(Sec)  ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	ImGui::DragFloat("##DurationSec", &m_vecFrm[m_iSelectFrm].fDuration, 0.0005f, 0.00001f, FLT_MAX, "%.5f");

	ImGui::SameLine();
	if (ImGui::Button("Set All Frame"))
	{
		for (size_t i = 0; i < m_vecFrm.size(); i++)
		{
			m_vecFrm[i].fDuration = m_vecFrm[m_iSelectFrm].fDuration;
		}
	}

	float fDurSec = 1 / m_vecFrm[m_iSelectFrm].fDuration;
	int iDurStep = 1;
	ImGui::Text("Duration(FPS)  : %.f", fDurSec);
}

void AnimEditorUI::PlayAnim()
{
	if (0 >= m_vecFrm.size())
		return;

	// 애니메이션 재생
	if (ImGui::Button("Play##Play"))
	{
		m_bBlockEdit = !m_bBlockEdit;
	}

	ImGui::Text("Show Grid");
	ImGui::SameLine();
	ImGui::Checkbox("##Grid", &m_bGrid);
	ImGui::SameLine();
	ImGui::Text("       Show Overlay");
	ImGui::SameLine();
	ImGui::Checkbox("##Overlay", &m_bOverlay);

	static bool m_bFinish = false;
	static bool m_bReset = true;

	if (m_bBlockEdit)
	{
		if (m_bFinish)
		{
			if (m_bReset)
			{
				m_iSelectFrm = 0;
				m_bFinish = false;
			}
		}

		static float m_fAccTime = 0.f;
		m_fAccTime += DT;

		if (m_fAccTime > m_vecFrm[m_iSelectFrm].fDuration)
		{
			m_fAccTime = m_fAccTime - m_vecFrm[m_iSelectFrm].fDuration;

			if (m_vecFrm.size() <= ++m_iSelectFrm)
			{
				m_iSelectFrm = (int)m_vecFrm.size() - 1;

				m_bFinish = true;
			}
		}
	}

	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
	float fBackAspect = (m_vBackSize.x * m_vTexSize.x) / (m_vBackSize.y * m_vTexSize.y);
	ImVec2 canvas_sz = ImVec2(300.f, 300.f / fBackAspect);
	ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
	Vec2 vScale = Vec2(canvas_sz.x / (m_vBackSize.x * m_vTexSize.x), canvas_sz.y / (m_vBackSize.y * m_vTexSize.y));

	// Draw border and background color
	ImGuiIO& io = ImGui::GetIO();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
	draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

	// This will catch our interactions
	ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool is_hovered = ImGui::IsItemHovered(); // Hovered
	const bool is_active = ImGui::IsItemActive();   // Held
	const ImVec2 origin(canvas_p0.x, canvas_p0.y); // Lock scrolled origin
	const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

	draw_list->PushClipRect(canvas_p0, canvas_p1, true);

	Vec2 vValue = m_vBackSize - m_vecFrm[m_iSelectFrm].SliceUV;
	vValue /= 2.f;
	ImVec2 vDist = ImVec2((vValue.x + m_vecFrm[m_iSelectFrm].Offset.x) * m_vTexSize.x * vScale.x, 
							(vValue.y + m_vecFrm[m_iSelectFrm].Offset.y)  * m_vTexSize.y * vScale.y);

	if (is_active && !m_bBlockEdit && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		m_vecFrm[m_iSelectFrm].Offset.x += io.MouseDelta.x / m_vTexSize.x / vScale.x;
		m_vecFrm[m_iSelectFrm].Offset.y += io.MouseDelta.y / m_vTexSize.y / vScale.y;
	}

	draw_list->AddImage((void*)(m_pTex->GetSRV().Get()), 
		ImVec2(origin.x + vDist.x, origin.y + vDist.y), 
		ImVec2(origin.x + vDist.x + m_vecFrm[m_iSelectFrm].SliceUV.x * vScale.x * m_vTexSize.x,
				origin.y + vDist.y + m_vecFrm[m_iSelectFrm].SliceUV.y * vScale.y * m_vTexSize.y),
		ImVec2(m_vecFrm[m_iSelectFrm].LeftTopUV.x, m_vecFrm[m_iSelectFrm].LeftTopUV.y),
		ImVec2(m_vecFrm[m_iSelectFrm].LeftTopUV.x + m_vecFrm[m_iSelectFrm].SliceUV.x, m_vecFrm[m_iSelectFrm].LeftTopUV.y + m_vecFrm[m_iSelectFrm].SliceUV.y));

	if (m_bOverlay)
	{
		// 다음 프레임 오버레이 하여 그려주기
		int iNextFrm = (m_iSelectFrm + 1) % (int)m_vecFrm.size();
		vValue = m_vBackSize - m_vecFrm[iNextFrm].SliceUV;
		vValue /= 2.f;
		vDist = ImVec2((vValue.x + m_vecFrm[iNextFrm].Offset.x) * m_vTexSize.x * vScale.x,
			(vValue.y + m_vecFrm[iNextFrm].Offset.y) * m_vTexSize.y * vScale.y);

		ImVec4 vImColor(0.5f, 0.5f, 1.f, 0.5f);
		draw_list->AddImage((void*)(m_pTex->GetSRV().Get()),
			ImVec2(origin.x + vDist.x, origin.y + vDist.y),
			ImVec2(origin.x + vDist.x + m_vecFrm[iNextFrm].SliceUV.x * vScale.x * m_vTexSize.x,
				origin.y + vDist.y + m_vecFrm[iNextFrm].SliceUV.y * vScale.y * m_vTexSize.y),
			ImVec2(m_vecFrm[iNextFrm].LeftTopUV.x, m_vecFrm[iNextFrm].LeftTopUV.y),
			ImVec2(m_vecFrm[iNextFrm].LeftTopUV.x + m_vecFrm[iNextFrm].SliceUV.x, m_vecFrm[iNextFrm].LeftTopUV.y + m_vecFrm[iNextFrm].SliceUV.y),
			ImGui::GetColorU32(vImColor));
	}

	if (m_bGrid)
	{
		// 가로 중앙선
		draw_list->AddLine(ImVec2(canvas_p0.x + canvas_sz.x / 2.f, canvas_p0.y),
			ImVec2(canvas_p0.x + canvas_sz.x / 2.f, canvas_p0.y + canvas_sz.y),
			IM_COL32(200, 200, 200, 40));
		// 새로 중앙선
		draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + canvas_sz.y / 2.f),
			ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y / 2.f),
			IM_COL32(200, 200, 200, 40));
	}

	draw_list->PopClipRect();
}

void AnimEditorUI::EditAnim()
{
	char szBuff[50] = "";
	string strName = string(m_strName.begin(), m_strName.end());
	memcpy(szBuff, strName.c_str(), sizeof(char) * 50);
	ImGui::Text("Anim Name");
	ImGui::SameLine();
	ImGui::InputText("##Name", szBuff, sizeof(char) * 50);
	strName = szBuff;
	m_strName = wstring(strName.begin(), strName.end());

	ImGui::Spacing();

	if (nullptr != m_pTex)
	{
		ImGui::PushID(0);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
		ImGui::Button("Texture Size");
		ImGui::PopStyleColor(3);
		ImGui::PopID();
		ImGui::Text("Width % d / Height % d", (int)m_vTexSize.x, (int)m_vTexSize.y);

		ImGui::Spacing();

		ImGui::PushID(0);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
		ImGui::Button("Texture Name");
		ImGui::PopStyleColor(3);
		ImGui::PopID();
		string strTexName = string(m_pTex->GetKey().begin(), m_pTex->GetKey().end());
		ImGui::Text(strTexName.c_str());

		ImVec2 vContentSize = ImGui::GetContentRegionAvail();
		float fAspect = m_vTexSize.x / m_vTexSize.y;
		ImGui::Image((void*)m_pTex->GetSRV().Get(), ImVec2(vContentSize.x, vContentSize.x / fAspect), 
					ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f));
	}

	ImGui::Spacing();
	if (nullptr == m_pAnim2D && m_vecFrm.empty())
	{
		if (ImGui::Button("Load Atlas Texture"))
		{
			LoadTex();
		}
	}

	ImGui::Spacing();

	ImGui::BeginDisabled(nullptr == m_pTex || m_strName.empty() || m_vecFrm.empty());

	ImVec2 vContentSize = ImGui::GetContentRegionAvail();
	if (ImGui::Button("Save", ImVec2(vContentSize.x * 0.3f, 25.f)))
	{
		if (m_pAnim2D)
		{
			m_pAnim2D->Change(m_vecFrm, m_vBackSize);
		}

		SaveAnim();
	}

	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::Button("Load", ImVec2(vContentSize.x * 0.3f, 25)))
	{
		LoadAnim();
	}

	ImGui::SameLine();

	if (ImGui::Button("Clear", ImVec2(vContentSize.x * 0.3f, 25)))
	{
		Reset();
	}
}


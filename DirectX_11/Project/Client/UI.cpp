#include "pch.h"
#include "UI.h"

#include <Engine/CPathMgr.h>
#include <Engine/CResMgr.h>

UI::UI(const string& _strKey)	:
	m_strKey(_strKey),
	m_vPopUpPos{},
	m_pParentUI(nullptr),
	m_bActive(false),
	m_bModal(false),
	m_WndFlags(0)
{
}

UI::~UI()
{
	Safe_Del_Vec(m_vecChildUI);
}

void UI::finaltick()
{
	if (!m_bActive)
		return;

	string strFullName = m_strName + m_strKey;

	if (nullptr == m_pParentUI)
	{
		// 모달리스
		if (!m_bModal)
		{
			ImGui::Begin(strFullName.c_str(), &m_bActive, m_WndFlags);

			render_update();

			for (size_t i = 0; i < m_vecChildUI.size(); i++)
			{
				if (!m_vecChildUI[i]->IsActive())
					continue;

				m_vecChildUI[i]->finaltick();

				if (i != m_vecChildUI.size() - 1)
					ImGui::Separator();
			}
			ImGui::End();
		}
		// 모달															
		else
		{
			ImGui::SetNextWindowPos(m_vPopUpPos);
			ImGui::SetNextWindowSize(m_vSize);
			ImGui::SetNextWindowFocus();

			ImGui::OpenPopup(strFullName.c_str());
			if (ImGui::BeginPopupModal(strFullName.c_str(), &m_bActive, m_WndFlags))
			{
				render_update();

				for (size_t i = 0; i < m_vecChildUI.size(); i++)
				{
					if (m_vecChildUI[i]->m_bActive)
					{
						m_vecChildUI[i]->finaltick();

						if (i != m_vecChildUI.size() - 1)
							ImGui::Separator();
					}
				}
				ImGui::EndPopup();
			}
		}
	}
	else
	{
		ImGui::BeginChild(strFullName.c_str(), m_vSize, m_WndFlags);

		render_update();

		for (size_t i = 0; i < m_vecChildUI.size(); i++)
		{
			if (m_vecChildUI[i]->m_bActive)
			{
				if (!m_vecChildUI[i]->IsActive())
					continue;

				m_vecChildUI[i]->finaltick();

				if (i != m_vecChildUI.size() - 1)
					ImGui::Separator();
			}
		}
		ImGui::EndChild();
	}
}

void UI::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

Ptr<CTexture> UI::LoadTexFromFile()
{
	// open a file name
	OPENFILENAME ofn = {};

	wstring strTileFolderPath = CPathMgr::GetInst()->GetContentPath();
	strTileFolderPath += L"texture\\";

	wchar_t szFilePath[256] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFilePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = L"png\0*.png\0ALL\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = strTileFolderPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (false == GetOpenFileName(&ofn))
		return nullptr;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	size_t start = strFilePath.length();
	wstring Buff = szFilePath;
	size_t end = Buff.length();

	if (0 == (int)end)
		return nullptr;

	wstring FilePath = Buff.substr(start, end);

	// 파일의 이름을 키 값으로 그대로 넣어준다
	wstring strKey = FilePath;

	// 확장자명 잘라주기
	for (size_t i = strKey.length(); i > 0; i--)
	{
		if (L'.' == strKey[i])
		{
			strKey[i] = 0;
			break;
		}
	}

	// 폴더 경로 짤라주기
	start = 0;
	end = strKey.length();

	for (size_t i = 0; i < end; i++)
	{
		if (L'\\' == strKey[i])
			start = i + 1;
	}
	strKey = strKey.substr(start, end);

	return CResMgr::GetInst()->LoadRes<CTexture>(strKey, FilePath);
}

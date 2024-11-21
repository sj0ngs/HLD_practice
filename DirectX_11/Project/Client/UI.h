#pragma once

#include "ImGuiMgr.h"
#include <Engine/ptr.h>

class CTexture;

class UI
{
private:
	string				m_strName;		// 타이틀 명, ID
	const string		m_strKey;		// 고유 키

	ImVec2				m_vSize;		// UI 크기
	ImVec2				m_vPopUpPos;	// 

	UI*					m_pParentUI;	// 부모 UI
	vector<UI*>			m_vecChildUI;	// 자식 UI

	bool				m_bActive;
	bool				m_bModal;

	ImGuiWindowFlags	m_WndFlags;

public:
	void SetActive(bool _bActive) { m_bActive = _bActive; }
	bool IsActive() const { return m_bActive; }

	void SetModal(bool _bModal) { m_bModal = _bModal; }
	bool IsModal() const { return m_bModal; }

	const string& GetName() const { return m_strName; }
	void SetName(const string& _strName) { m_strName = _strName; }

	const string& GetID() const { return m_strKey; }
	
	void SetSize(float _fWidth, float _fHeight) { m_vSize = ImVec2(_fWidth, _fHeight); }
	const ImVec2 GetSize() const { return m_vSize; }
	
	ImVec2 GetPopUpPos() const { return m_vPopUpPos; }
	void SetPopUpPos(ImVec2 _vPopUpPos) { m_vPopUpPos = _vPopUpPos; }

	void SetWindowFlags(ImGuiWindowFlags _flags) { m_WndFlags = _flags; }
	void AddWindowFlags(ImGuiWindowFlags _flags) { m_WndFlags |= _flags; }

	void AddChildUI(UI* _UI)
	{
		_UI->m_pParentUI = this;
		m_vecChildUI.push_back(_UI);
	}

public:
	virtual void init() {};
	virtual void tick() {};
	virtual void finaltick();

	virtual int render_update() = 0;

public:
	UI(const string& _strKey);
	virtual ~UI();

public:
	void HelpMarker(const char* desc);

	Ptr<CTexture> LoadTexFromFile();
};


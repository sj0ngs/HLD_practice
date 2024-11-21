#pragma once

class CGameObjectEx;
class CLevel;

class CEditorObjMgr	:
	public CSingleton<CEditorObjMgr>
{
	SINGLE(CEditorObjMgr);
private:
	vector<CGameObjectEx*>		m_vecEditorObj;

	CGameObjectEx*				m_DebugShape[(UINT)SHAPE_TYPE::END];
	vector<tDebugShapeInfo>		m_DebugShapeInfo;

	CGameObjectEx*				m_pLocalCamObj;

	bool						m_bDebug;

public:
	void SwitchDebugMode() { m_bDebug = !m_bDebug; };

public:
	CGameObjectEx* GetLocalCam() const { return m_pLocalCamObj; }

public:
	void init();
	void progress();

private:
	void tick();
	void render();
};


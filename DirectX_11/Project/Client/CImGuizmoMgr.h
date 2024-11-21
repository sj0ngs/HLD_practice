#pragma once

enum class GIZMO_OP
{
	TRANSLATE,
	SCALE,
	ROTATE,
	END
};

enum class GIZMO_MODE
{
	LOCAL,
	WORLD,
	END
};

class CImGuizmoMgr :
	public CSingleton<CImGuizmoMgr>
{
private:
	float m_arrView[16];
	float m_arrProj[16];
	float m_arrIdentity[16];

	GIZMO_OP	m_GizmoOp;
	GIZMO_MODE	m_GizmoMode;

	bool m_bIsManipulating;

public:
	GIZMO_OP GetGizmoOperation() { return m_GizmoOp; }
	void SetGizmoOperation(GIZMO_OP _op) { m_GizmoOp = _op; }

	GIZMO_MODE GetGitzmoMode() { return m_GizmoMode; }
	void SetGizmoMode(GIZMO_MODE _mode) { m_GizmoMode = _mode; }

public:
	void tick();

	SINGLE(CImGuizmoMgr);
private:
	void CalcMatrix();
	void RenderGizmo();
};


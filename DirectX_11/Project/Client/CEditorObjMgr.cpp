#include "pch.h"
#include "CEditorObjMgr.h"

#include <Engine\CTimeMgr.h>
#include <Engine\CResMgr.h>
#include <Engine\CRenderMgr.h>
#include <Engine\CLevel.h>
#include <Engine\components.h>
#include <Script\CCameraMoveScript.h>
#include <Engine\CKeyMgr.h>
#include <Engine\CCollisionMgr.h>

#include "CGameObjectEx.h"
#include "ImGuiMgr.h"
#include "InspectorUI.h"

CEditorObjMgr::CEditorObjMgr() :
	m_DebugShape{},
	m_pLocalCamObj(nullptr),
	m_bDebug(true)
{

}

CEditorObjMgr::~CEditorObjMgr()
{
	Safe_Del_Vec(m_vecEditorObj);
	Safe_Del_Array(m_DebugShape);

	if (nullptr != m_pLocalCamObj)
	{
		delete m_pLocalCamObj;
	}
}


void CEditorObjMgr::init()
{
	// 디버그 쉐이프 생성
	m_DebugShape[(UINT)SHAPE_TYPE::RECT] = new CGameObjectEx;
	m_DebugShape[(UINT)SHAPE_TYPE::RECT]->AddComponent(new CTransform);
	m_DebugShape[(UINT)SHAPE_TYPE::RECT]->AddComponent(new CMeshRender);
	m_DebugShape[(UINT)SHAPE_TYPE::RECT]->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh_Debug"));
	m_DebugShape[(UINT)SHAPE_TYPE::RECT]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugShapeMtrl"));

	m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE] = new CGameObjectEx;
	m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE]->AddComponent(new CTransform);
	m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE]->AddComponent(new CMeshRender);
	m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE]->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh_Debug"));
	m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugShapeMtrl"));

	m_DebugShape[(UINT)SHAPE_TYPE::TRI] = new CGameObjectEx;
	m_DebugShape[(UINT)SHAPE_TYPE::TRI]->AddComponent(new CTransform);
	m_DebugShape[(UINT)SHAPE_TYPE::TRI]->AddComponent(new CMeshRender);
	m_DebugShape[(UINT)SHAPE_TYPE::TRI]->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"TriMesh"));
	m_DebugShape[(UINT)SHAPE_TYPE::TRI]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugShapeMtrl"));

	// EditorObj 생성
	CGameObjectEx* pEditorCamObj = new CGameObjectEx();
	pEditorCamObj->AddComponent(new CTransform);
	pEditorCamObj->AddComponent(new CCamera);
	pEditorCamObj->AddComponent(new CCameraMoveScript);

	pEditorCamObj->Camera()->SetLayerMaskAll(true);
	pEditorCamObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, -100.f));

	m_vecEditorObj.push_back(pEditorCamObj);
	CRenderMgr::GetInst()->RegisterEditorCamera(pEditorCamObj->Camera());

	// Local Level 생성
	m_pLocalCamObj = new CGameObjectEx;
	m_pLocalCamObj->AddComponent(new CTransform);
	m_pLocalCamObj->AddComponent(new CCamera);
	m_pLocalCamObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, -100.f));
	m_pLocalCamObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	m_pLocalCamObj->Camera()->SetLayerMaskAll(true);
}

void CEditorObjMgr::progress()
{
	// DebugShape 정보 가져오기
	vector<tDebugShapeInfo>& vecInfo = CRenderMgr::GetInst()->GetDebugShapeInfo();
	m_DebugShapeInfo.insert(m_DebugShapeInfo.end(), vecInfo.begin(), vecInfo.end());
	vecInfo.clear();

	tick();
	render();
}

void CEditorObjMgr::tick()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	//if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && KEY_TAP(KEY::P))
	//{
	//	if (pCurLevel->GetState() == LEVEL_STATE::PLAY)
	//		pCurLevel->ChangeState(LEVEL_STATE::STOP);
	//	else
	//		pCurLevel->ChangeState(LEVEL_STATE::PLAY);
	//}

	for (size_t i = 0; i < m_vecEditorObj.size(); i++)
	{
		m_vecEditorObj[i]->tick();
	}

	for (size_t i = 0; i < m_vecEditorObj.size(); i++)
	{
		m_vecEditorObj[i]->finaltick();
	}

	if (KEY_PRESSED(KEY::LSHIFT) && KEY_TAP(KEY::TAB))
		SwitchDebugMode();
}

void CEditorObjMgr::render()
{
	if (!m_bDebug)
		return;

	for (size_t i = 0; i < m_vecEditorObj.size(); i++)
	{
		m_vecEditorObj[i]->render();
	}

	Vec3 vCamPos;
	if (CLevelMgr::GetInst()->GetCurLevel()->GetState() != LEVEL_STATE::STOP)
	{
		CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();

		if (pCam)
			pCam->PlayProj();
	}

	// DebugShape Render
	CGameObjectEx* pShapeObj = nullptr;
	
	vector<tDebugShapeInfo>::iterator iter = m_DebugShapeInfo.begin();
	for (; iter != m_DebugShapeInfo.end();)
	{
		switch (iter->eShape)
		{
		case SHAPE_TYPE::RECT:
			pShapeObj = m_DebugShape[(UINT)SHAPE_TYPE::RECT];
			break;
		case SHAPE_TYPE::CIRCLE:
			pShapeObj = m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE];
			break;
		case SHAPE_TYPE::TRI:
			pShapeObj = m_DebugShape[(UINT)SHAPE_TYPE::TRI];
			break;
		case SHAPE_TYPE::CUBE:
			break;
		case SHAPE_TYPE::SHPERE:
			break;
		}

		if (nullptr == pShapeObj)
			return;

		if (iter->matWorld != XMMatrixIdentity())
		{
			pShapeObj->Transform()->SetWorldMat(iter->matWorld);
		}
		else
		{
			pShapeObj->Transform()->SetRelativePos(iter->vWorldPos);
			pShapeObj->Transform()->SetRelativeScale(iter->vWorldScale);
			pShapeObj->Transform()->SetRelativeRot(iter->vWorldRotation);
			pShapeObj->finaltick();
		}

		pShapeObj->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &iter->vColor);
		pShapeObj->render();

		iter->fCurTime += DT;
		if (iter->fMaxTime < iter->fCurTime)
		{
			iter = m_DebugShapeInfo.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	
	// Mouse Picking
	//if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
	//{
	//	if (LEVEL_STATE::PLAY != CLevelMgr::GetInst()->GetCurLevel()->GetState() &&
	//		KEY_TAP(KEY::LBTN))
	//	{
	//		CGameObject* pPickedObj = CCollisionMgr::GetInst()->GetPickedObject(CKeyMgr::GetInst()->GetMouseWorldPos());

	//		if (pPickedObj)
	//		{
	//			((InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector"))->SetTargetObject(pPickedObj);
	//		}
	//	}
	//}
}
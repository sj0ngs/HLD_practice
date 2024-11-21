#include "pch.h"
#include "CRoomScript.h"
#include <Engine/CEventMgr.h>
#include <Engine/CResMgr.h>
#include <Engine/CStructuredBuffer.h>
#include <Engine/CRoomCollisionShader.h>
#include <Engine/CRenderMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

#include "CPlayerScript.h"
#include "CCameraMoveScript.h"
#include "CShadowScript.h"

CRoomScript::CRoomScript()	:
	CScript((UINT)SCRIPT_TYPE::ROOMSCRIPT),
	m_pDodge(nullptr),
	m_pOverlay(nullptr),
	m_pLayer0(nullptr),
	m_pLayer1(nullptr),
	m_pLayer2(nullptr),
	m_pCollisionResultBuffer(nullptr),
	m_pPlayer(nullptr),
	m_pCollisionCS(nullptr),
	m_pShadowCS(nullptr),
	m_pDynamicShadow(nullptr)
{
	// Tex Param 설정
	AddScriptParam(m_pDodgeTex.Get(), SCRIPT_PARAM::TEX, "BG_Dodge");
	AddScriptParam(m_pOverlayTex.Get(), SCRIPT_PARAM::TEX, "BG_Overlay");
	AddScriptParam(m_pLayer0Tex.Get(), SCRIPT_PARAM::TEX, "Layer0");
	AddScriptParam(m_pLayer1Tex.Get(), SCRIPT_PARAM::TEX, "Layer1");
	AddScriptParam(m_pLayer1_invTex.Get(), SCRIPT_PARAM::TEX, "Layer1_inv");
	AddScriptParam(m_pLayer2Tex.Get(), SCRIPT_PARAM::TEX, "Layer2");

	m_pCollisionResultBuffer = new CStructuredBuffer;
	m_pCollisionResultBuffer->Create(sizeof(tRoomCollisionResult), 1, SB_TYPE::READ_WRITE, true);

	m_pCollisionCS = (CRoomCollisionShader*)(CResMgr::GetInst()->FindRes<CComputeShader>(L"RoomCollisionCS").Get());
	m_pShadowCS = (CRoomShadowShader*)(CResMgr::GetInst()->FindRes<CComputeShader>(L"RoomShadowCS").Get());
	m_pObstacleMapCS = (CObstacleMapShader*)(CResMgr::GetInst()->FindRes<CComputeShader>(L"ObstacleMapCS").Get());
}

CRoomScript::CRoomScript(const CRoomScript& _pOrigin)	:
	CScript(_pOrigin),
	m_pDodge(nullptr),
	m_pOverlay(nullptr),
	m_pLayer0(nullptr),
	m_pLayer1(nullptr),
	m_pLayer2(nullptr),
	m_pDodgeTex(_pOrigin.m_pDodgeTex),
	m_pOverlayTex(_pOrigin.m_pOverlayTex),
	m_pLayer0Tex(_pOrigin.m_pLayer0Tex),
	m_pLayer1Tex(_pOrigin.m_pLayer1Tex),
	m_pLayer1_invTex(_pOrigin.m_pLayer1_invTex),
	m_pLayer2Tex(_pOrigin.m_pLayer2Tex),
	m_pCollisionResultBuffer(nullptr),
	m_pPlayer(nullptr),
	m_pCollisionCS(_pOrigin.m_pCollisionCS),
	m_pShadowCS(_pOrigin.m_pShadowCS),
	m_pDynamicShadow(nullptr)
{
	m_pCollisionResultBuffer = new CStructuredBuffer;
	m_pCollisionResultBuffer->Create(sizeof(tRoomCollisionResult), 1, SB_TYPE::READ_WRITE, true);
}

CRoomScript::~CRoomScript()
{
	SAFE_DEL(m_pCollisionResultBuffer);
}

void CRoomScript::init()
{
	Ptr<CMaterial> pAlphaMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"AlphaBlendLightMtrl");
	Ptr<CMaterial> pStdMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DLightMtrl");

	int iLightOff = 1;
	float fAlphaDown = 0.5f;

	// 이녀석 자체가 렌더링을 진행하진 않음 대신 일종의 룸 관리자 오브젝트임
	// 아래로 갈수록 점점 카메라에 가까워져야함
	// dodge(백그라운드에 추가색)
	m_pDodge = new CGameObject;
	m_pDodge->SetName(L"Dodge");
	m_pDodge->AddComponent(new CTransform()); 
	m_pDodge->AddComponent(new CMeshRender());
	m_pDodge->Transform()->SetRelativePos(0.f, 0.f, 101.f);
	m_pDodge->MeshRender()->SetMaterial(pAlphaMtrl);
	m_pDodge->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, m_pDodgeTex);
	m_pDodge->MeshRender()->GetMaterial()->SetScalarParam(INT_3, &iLightOff);
	//m_pDodge->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_1, &fAlphaDown);
	m_pDodge->SetScriptObj(true);
	AddChildObject(GetOwner(), m_pDodge);

	if (nullptr != m_pDodgeTex)
		m_pDodge->Transform()->SetRelativeScale(m_pDodgeTex->Width() * 2.f, m_pDodgeTex->Height() * 2.f, 1.f);

	// layer 1(실제 바닥 이미지)
	m_pLayer1 = new CGameObject;
	m_pLayer1->SetName(L"Layer1");
	m_pLayer1->AddComponent(new CTransform());
	m_pLayer1->AddComponent(new CMeshRender());
	m_pLayer1->Transform()->SetRelativePos(0.f, 0.f, 799.f);
	m_pLayer1->MeshRender()->SetMaterial(pStdMtrl);
	m_pLayer1->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, m_pLayer1Tex);
	m_pLayer1->MeshRender()->GetMaterial()->SetScalarParam(INT_3, &iLightOff);
	m_pLayer1->SetScriptObj(true);
	AddChildObject(GetOwner(), m_pLayer1);
	
	if (nullptr != m_pLayer1Tex)
		m_pLayer1->Transform()->SetRelativeScale(m_pLayer1Tex->Width() * 2.f, m_pLayer1Tex->Height() * 2.f, 1.f);

	// layer 0(정적 그림자)
	m_pLayer0 = new CGameObject;
	m_pLayer0->SetName(L"Layer0");
	m_pLayer0->AddComponent(new CTransform());
	m_pLayer0->AddComponent(new CMeshRender());
	m_pLayer0->Transform()->SetRelativePos(0.f, 0.f, 798.f);
	m_pLayer0->MeshRender()->SetMaterial(pAlphaMtrl);
	m_pLayer0->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, m_pLayer0Tex);
	int iOn = 1;
	float fAlpha = 0.6f;
	m_pLayer0->MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_2, &iOn);
	m_pLayer0->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &fAlpha);
	m_pLayer0->SetScriptObj(true);
	AddChildObject(GetOwner(), m_pLayer0);

	if (nullptr != m_pLayer0Tex)
		m_pLayer0->Transform()->SetRelativeScale(m_pLayer0Tex->Width() * 2.f, m_pLayer0Tex->Height() * 2.f, 1.f);

	// shadow (동적 그림자)
	m_pDynamicShadow = new CGameObject;
	m_pDynamicShadow->SetName(L"DynamicShadow");
	m_pDynamicShadow->AddComponent(new CTransform());
	m_pDynamicShadow->AddComponent(new CMeshRender());
	m_pDynamicShadow->Transform()->SetRelativePos(0.f, 0.f, 797.f);
	m_pDynamicShadow->MeshRender()->SetMaterial(pAlphaMtrl);
	//m_pDynamicShadow->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, m_pLayer0Tex);
	m_pDynamicShadow->SetScriptObj(true);
	AddChildObject(GetOwner(), m_pDynamicShadow);

	// layer 2(벽)
	m_pLayer2 = new CGameObject;
	m_pLayer2->SetName(L"Layer2");
	m_pLayer2->AddComponent(new CTransform());
	m_pLayer2->AddComponent(new CMeshRender());
	m_pLayer2->Transform()->SetRelativePos(0.f, 0.f, 100.f);
	m_pLayer2->MeshRender()->SetMaterial(pStdMtrl);
	m_pLayer2->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, m_pLayer2Tex);
	m_pLayer2->SetScriptObj(true);
	AddChildObject(GetOwner(), m_pLayer2);

	if (nullptr != m_pLayer2Tex)
		m_pLayer2->Transform()->SetRelativeScale(m_pLayer2Tex->Width() * 2.f, m_pLayer2Tex->Height() * 2.f, 1.f);

	// overlay(빛)
	m_pOverlay = new CGameObject;
	m_pOverlay->SetName(L"Overlay");
	m_pOverlay->AddComponent(new CTransform());
	m_pOverlay->AddComponent(new CMeshRender());
	m_pOverlay->Transform()->SetRelativePos(0.f, 0.f, 99.f);
	m_pOverlay->MeshRender()->SetMaterial(pAlphaMtrl);
	m_pOverlay->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, m_pOverlayTex);
	m_pOverlay->MeshRender()->GetMaterial()->SetScalarParam(INT_3, &iLightOff);
	m_pOverlay->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_1, &fAlphaDown);
	m_pOverlay->SetScriptObj(true);
	AddChildObject(GetOwner(), m_pOverlay);

	if (nullptr != m_pOverlayTex)
		m_pOverlay->Transform()->SetRelativeScale(m_pOverlayTex->Width() * 2.f, m_pOverlayTex->Height() * 2.f, 1.f);
}

void CRoomScript::begin()
{
	m_pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScript<CPlayerScript>();
	m_pPlayer->SetRoom(this);

	CCameraMoveScript* pCam = CRenderMgr::GetInst()->GetMainCamera()->GetOwner()->GetScript<CCameraMoveScript>();

	if (pCam)
		pCam->SetRoom(this);

	if (nullptr != m_pLayer1Tex)
	{
		// 그림자 텍스쳐 생성
		wstring strKey = GetOwner()->GetName() + L"_Shadow";
		m_pLayerShadowTex = CResMgr::GetInst()->CreateTexture(strKey, m_pLayer1Tex->Width(), m_pLayer1Tex->Height(),
			DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT);

		m_pDynamicShadow->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, m_pLayerShadowTex);
		m_pDynamicShadow->Transform()->SetRelativeScale(m_pLayerShadowTex->Width() * 2.f, m_pLayerShadowTex->Height() * 2.f, 1.f);

		// 텍스쳐 기반 astar 전부 폐기 - 시간상 효율 떨어짐
		//// 장애물 텍스쳐 생성
		//strKey = GetOwner()->GetName() + L"_Obstacle";
		//m_pObstacleTex = CResMgr::GetInst()->CreateTexture(strKey, m_pLayer1Tex->Width(), m_pLayer1Tex->Height(),
		//	DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT);

		//// obstacle layer (8번) 에서 모든 충돌체의 정보를 가져옴
		//const vector<CGameObject*>& vecObj = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(8)->GetObjects();
	
		//// 콜라이더의 개수 만큼 반복문 돌려서 그림을 그려내야 할듯
		//for (size_t i = 0; i < vecObj.size(); i++)
		//{
		//	CCollider2D* pColl = dynamic_cast<CCollider2D*>(vecObj[i]->GetComponent(COMPONENT_TYPE::COLLIDER2D));
		//	if (nullptr == pColl)
		//		continue;

		//	if (nullptr != m_pObstacleMapCS)
		//	{
		//		m_pObstacleMapCS->SetDrawTex(m_pObstacleTex);
		//		m_pObstacleMapCS->SetRoomPos(Transform()->GetRelativePos().XY());
		//		m_pObstacleMapCS->SetRoomSize(m_vRoomSize);
		//		m_pObstacleMapCS->SetColliderPos(pColl->GetWorldPos());
		//		m_pObstacleMapCS->SetColliderSize(pColl->GetWorldScale());
		//		m_pObstacleMapCS->SetColliderDraw(true);
		//		m_pObstacleMapCS->Execute();
		//	}
		//}

		//// 콜라이더가 아닌 edge, wall 을 그리기
		//m_pObstacleMapCS->SetDrawTex(m_pObstacleTex);
		//m_pObstacleMapCS->SetEdgeTex(m_pLayer1_invTex);
		//m_pObstacleMapCS->SetWallTex(m_pLayer2Tex);
		//m_pObstacleMapCS->SetColliderDraw(false);
		//m_pObstacleMapCS->Execute();

	}
}

void CRoomScript::tick()
{
	DrawShadow();
}

tRoomCollisionResult& CRoomScript::RoomCollisionCheck(Vec2& _vPos, float& _fSize)
{
	tRoomCollisionResult tResult;
	tResult.iResult = 0;
	tResult.fDist = D3D11_FLOAT32_MAX;
	tResult.uDist = (UINT)1000000000;

	m_pCollisionResultBuffer->SetData(&tResult);
	m_pCollisionCS->SetCollisionBuffer(m_pCollisionResultBuffer);
	m_pCollisionCS->SetLayer1Tex(m_pLayer1_invTex);
	m_pCollisionCS->SetLayer2Tex(m_pLayer2Tex);
	m_pCollisionCS->SetObjPos(_vPos);
	m_pCollisionCS->SetObjSize(_fSize);
	m_pCollisionCS->SetRoomSize(m_vRoomSize);
	m_pCollisionCS->Execute();
	
	m_pCollisionResultBuffer->GetData(&tResult);

	return tResult;
}

void CRoomScript::DrawShadow()
{
	vector<CGameObject*> vecShadowObj;
	CLevelMgr::GetInst()->GetCurLevel()->FindObjectsByScript(SHADOWSCRIPT, vecShadowObj);

	m_pShadowCS->SetDrawTex(m_pLayerShadowTex);
	m_pShadowCS->SetLayer1Tex(m_pLayer1Tex);
	m_pShadowCS->SetEraseMode(1);
	m_pShadowCS->Execute();

	m_pShadowCS->SetEraseMode(0);
	for (size_t i = 0; i < vecShadowObj.size(); i++)
	{
		CShadowScript* pShadow = (CShadowScript*)vecShadowObj[i]->GetScript(SHADOWSCRIPT);
		m_pShadowCS->SetShadowPos(pShadow->GetShadowFinalPos());

		if (!pShadow->IsOnShadow())
			continue;

		Ptr<CTexture> pShadowTex;
		if (SMALL == pShadow->GetShadowType())
			pShadowTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_charshadow_0");
		else if (LARGE == pShadow->GetShadowType())
			pShadowTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_BirdManShadow_0");

		m_pShadowCS->SetShadowTex(pShadowTex);
		m_pShadowCS->Execute();
	}
}

void CRoomScript::RoomCollisionCheckAndMove(Vec2& _vPos, float& _fSize, CTransform* _pTargetTransform, int _iType)
{
	tRoomCollisionResult tResult = RoomCollisionCheck(_vPos, _fSize);
	if (0 != tResult.iResult)
	{
		//Vec3 vPos = _pTargetTransform->GetRelativePos();
		//Vec2 vMove = (_fSize - tResult.fDist * 2.f) * tResult.vDir;
		//vMove.y *= -1.f;

		//vPos.x += vMove.x;
		//vPos.y += vMove.y;

		if(0 == _iType || _iType == tResult.iResult)
			_pTargetTransform->BackToPrevPos();
	}
}

int CRoomScript::RoomCollisionCheckFloorType(Vec2& _vPos, float& _fSize)
{
	return RoomCollisionCheck(_vPos, _fSize).iResult;
}

void CRoomScript::SetScriptTex(const string& _strDesc, const Ptr<CTexture>& _pTex)
{
	for (auto& item : m_vecParam)
	{
		if (item.strDesc == _strDesc)
		{
			item.pData = _pTex.Get();

			if ("BG_Dodge" == _strDesc)
			{
				m_pDodgeTex = _pTex.Get();
				m_pDodge->MeshRender()->GetMaterial()->SetTexParam(TEX_0, m_pDodgeTex);
				m_pDodge->Transform()->SetRelativeScale(m_pDodgeTex->Width() * 2.f, m_pDodgeTex->Height() * 2.f, 1.f);
			}
			else if ("BG_Overlay" == _strDesc)
			{
				m_pOverlayTex = _pTex.Get();
				m_pOverlay->MeshRender()->GetMaterial()->SetTexParam(TEX_0, m_pOverlayTex);
				m_pOverlay->Transform()->SetRelativeScale(m_pOverlayTex->Width() * 2.f, m_pOverlayTex->Height() * 2.f, 1.f);
			}
			else if ("Layer0" == _strDesc)
			{
				m_pLayer0Tex = _pTex.Get();
				m_pLayer0->MeshRender()->GetMaterial()->SetTexParam(TEX_0, m_pLayer0Tex);
				m_pLayer0->Transform()->SetRelativeScale(m_pLayer0Tex->Width() * 2.f, m_pLayer0Tex->Height() * 2.f, 1.f);
			}
			else if ("Layer1" == _strDesc)
			{
				m_pLayer1Tex = _pTex.Get();
				m_pLayer1->MeshRender()->GetMaterial()->SetTexParam(TEX_0, m_pLayer1Tex);
				m_pLayer1->Transform()->SetRelativeScale(m_pLayer1Tex->Width() * 2.f, m_pLayer1Tex->Height() * 2.f, 1.f);
				m_vRoomSize = Vec2(m_pLayer1Tex->Width() * 2.f, m_pLayer1Tex->Height() * 2.f);
			}
			else if ("Layer1_inv" == _strDesc)
				m_pLayer1_invTex = _pTex.Get();
			else if ("Layer2" == _strDesc)
			{
				m_pLayer2Tex = _pTex.Get();
				m_pLayer2->MeshRender()->GetMaterial()->SetTexParam(TEX_0, m_pLayer2Tex);
				m_pLayer2->Transform()->SetRelativeScale(m_pLayer2Tex->Width() * 2.f, m_pLayer2Tex->Height() * 2.f, 1.f);
			}
		}
	}
}

void CRoomScript::LoadFromLevelFile(FILE* _pFile)
{
	CScript::LoadFromLevelFile(_pFile);

	for (size_t i = 0; i < m_vecParam.size(); i++)
	{
		int iType = 0;
		fread(&iType, sizeof(int), 1, _pFile);
		m_vecParam[i].eParam = (SCRIPT_PARAM)iType;

		int iData = 0;
		float fData = 0;
		Vec2 v2Data;
		Vec3 v3Data;
		Vec4 v4Data;
		Ptr<CTexture> pTex = nullptr;
		switch (m_vecParam[i].eParam)
		{
		case SCRIPT_PARAM::INT:
			fread(&iData, sizeof(iData), 1, _pFile);
			break;
		case SCRIPT_PARAM::FLOAT:
			fread(&fData, sizeof(fData), 1, _pFile);
			break;
		case SCRIPT_PARAM::VEC2:
			fread(&v2Data, sizeof(v2Data), 1, _pFile);
			break;
		case SCRIPT_PARAM::VEC3:
			fread(&v3Data, sizeof(v3Data), 1, _pFile);
			break;
		case SCRIPT_PARAM::VEC4:
			fread(&v4Data, sizeof(v4Data), 1, _pFile);
			break;
		case SCRIPT_PARAM::TEX:
			LoadResRef(pTex, _pFile);
			break;
		}

		Loadstring(m_vecParam[i].strDesc, _pFile);

		if (m_vecParam[i].strDesc == "BG_Dodge")
		{
			m_pDodgeTex = pTex;
			m_vecParam[i].pData = m_pDodgeTex.Get();
		}
		else if (m_vecParam[i].strDesc == "BG_Overlay")
		{
			m_pOverlayTex = pTex;
			m_vecParam[i].pData = m_pOverlayTex.Get();
		}
		else if (m_vecParam[i].strDesc == "Layer0")
		{
			m_pLayer0Tex = pTex;
			m_vecParam[i].pData = m_pLayer0Tex.Get();
		}
		else if (m_vecParam[i].strDesc == "Layer1")
		{
			m_pLayer1Tex = pTex;
			m_vecParam[i].pData = m_pLayer1Tex.Get();
			m_vRoomSize = Vec2(m_pLayer1Tex->Width() * 2.f, m_pLayer1Tex->Height() * 2.f);
		}
		else if (m_vecParam[i].strDesc == "Layer1_inv")
		{
			m_pLayer1_invTex = pTex;
			m_vecParam[i].pData = m_pLayer1_invTex.Get();
		}
		else if (m_vecParam[i].strDesc == "Layer2")
		{
			m_pLayer2Tex = pTex;
			m_vecParam[i].pData = m_pLayer2Tex.Get();
		}
	}
}

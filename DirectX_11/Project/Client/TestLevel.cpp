#include "pch.h"
#include "TestLevel.h"

#include <Engine\CDevice.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CResMgr.h>
#include <Engine\CCollisionMgr.h>
#include <Engine\components.h>
#include <Engine\CMesh.h>
#include <Engine\CGraphicsShader.h>
#include <Engine\CMaterial.h>
#include <Engine\CGameObject.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CSetColorShader.h>
#include <Engine\CStructuredBuffer.h>

#include <Script\CAimScript.h>
#include <Script\CCameraMoveScript.h>
#include <Script\CMissileScript.h>
#include <Script\CMonsterScript.h>
#include <Script\CPlayerScript.h>

#include "CLevelSaveLoad.h"

void CreateTestLevel()
{
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();

	pLevel->GetLayer(0)->SetName(L"Default");
	pLevel->GetLayer(1)->SetName(L"TileMap");
	pLevel->GetLayer(2)->SetName(L"Player");
	pLevel->GetLayer(3)->SetName(L"Monster");
	pLevel->GetLayer(4)->SetName(L"PlayerProjectile");
	pLevel->GetLayer(5)->SetName(L"MonsterProjectile");
	pLevel->GetLayer(31)->SetName(L"ViewportUI");

	// 충돌 레이어 지정
	CCollisionMgr::GetInst()->LayerCheck(2, 3);
	CCollisionMgr::GetInst()->LayerCheck(4, 3);

	CreateCamera();
	CreateLight2D();
	CreateObject();

	//CLevelSaveLoad::SaveLevel(L"level\\testlevel.lv", pLevel);

	//CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(L"level\\testlevel.level");

	//Ptr<CMaterial> pNewMtrl = new CMaterial;
	//pNewMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std2DLightShader"));
	//pNewMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PlayerTex"));
	//CResMgr::GetInst()->AddRes(L"NewMtrl", pNewMtrl);
	//pNewMtrl->Save(L"material\\test.mtrl");

	//CResMgr::GetInst()->LoadRes<CMaterial>(L"NewMtrl", L"material\\test.mtrl");
}

void CreateCamera()
{
	// Main Camera
	CGameObject* pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCamera");

	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CCameraMoveScript);

	pMainCam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pMainCam->Camera()->SetCameraIndex(0);
	pMainCam->Camera()->SetLayerMaskAll(true);
	pMainCam->Camera()->SetLayerMask(31, false);

	SpawnGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), 0);

	// UI Camera
	CGameObject* pUICam = new CGameObject;
	pUICam->SetName(L"UICamera");

	pUICam->AddComponent(new CTransform);
	pUICam->AddComponent(new CCamera);

	pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pUICam->Camera()->SetCameraIndex(1);
	pUICam->Camera()->SetLayerMask(31, true);

	SpawnGameObject(pUICam, Vec3(0.f, 0.f, 0.f), 0);
}

void CreateLight2D()
{
	CGameObject* pLightObj = nullptr;
	//Directional Light
	pLightObj = new CGameObject;
	pLightObj->SetName(L"Directional_Light");
	pLightObj->AddComponent(new CTransform);
	pLightObj->AddComponent(new CLight2D);

	pLightObj->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -XM_PI / 2.f));

	pLightObj->Light2D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pLightObj->Light2D()->SetLightDiffuse(Vec3(0.5f, 0.5f, 0.5f));
	//pLightObj->Light2D()->SetLightAmbient(Vec3(0.1f, 0.1f, 0.1f));

	SpawnGameObject(pLightObj, Vec3(0.f, 0.f, 0.f), 0);
}

void CreateObject()
{
	// 텍스쳐 생성하기
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	Ptr<CTexture> pCreateTex = CResMgr::GetInst()->CreateTexture(L"SampleTexture",
		1280, 768,
		DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		D3D11_USAGE_DEFAULT);

	// Compute Shader 로 색 변경
	Ptr<CSetColorShader> pCS = (CSetColorShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"SetColorCS").Get();
	pCS->SetTargetTexture(pCreateTex);
	pCS->SetColor(Vec3(1.f, 1.f, 0.f));
	pCS->Execute();


	// Resource Load
	Ptr<CMesh> RectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	Ptr<CMesh> CircleMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh");

	Ptr<CMaterial> TestMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TestMtrl");
	Ptr<CMaterial> Std2DMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DMtrl");
	Ptr<CMaterial> Std2DAnimMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DAnimMtrl");
	Ptr<CMaterial> DebugShapeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DebugShapeMtrl");
	Ptr<CMaterial> Std2DLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DLightMtrl");
	Ptr<CMaterial> Std2DAnimLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DAnimLightMtrl");
	Ptr<CMaterial> MapMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MapMtrl");
	


	Ptr<CMaterial> GunMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"GunMtrl");
	Ptr<CMaterial> Mon1Mtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Mon1Mtrl");
	Ptr<CMaterial> MeshMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MeshMtrl");

	Ptr<CTexture> PlayerTex = CResMgr::GetInst()->FindRes<CTexture>(L"link");
	//Ptr<CTexture> SmokeTex = CResMgr::GetInst()->FindRes<CTexture>(L"SmokeTex");
	Ptr<CTexture> TileAtlasTex = CResMgr::GetInst()->FindRes<CTexture>(L"TILE");

	Ptr<CTexture> CharTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_char_9");
	//Ptr<CTexture> CenterMapTex = CResMgr::GetInst()->FindRes<CTexture>(L"Center_1");
	Ptr<CTexture> GunTex = CResMgr::GetInst()->FindRes<CTexture>(L"spr_AimPistolSide_14");

	//Ptr<CTexture> DeadCellTex = CResMgr::GetInst()->FindRes<CTexture>(L"DeadCell");
	//Ptr<CTexture> DeadCellTex_N = CResMgr::GetInst()->FindRes<CTexture>(L"DeadCell_N");
	//Ptr<CTexture> PistolTex = CResMgr::GetInst()->FindRes<CTexture>(L"CharTex");

	// Map 
	CGameObject* pMap = new CGameObject;
	pMap->SetName(L"Map");
	pMap->AddComponent(new CTransform);
	pMap->AddComponent(new CTileMap);

	pMap->Transform()->SetRelativePos(Vec3(0.f, 0.f, 800.f));
	pMap->Transform()->SetRelativeScale(Vec3(500.f, 500.f, 1.f));

	pMap->TileMap()->GetMaterial()->SetTexParam(TEX_0, TileAtlasTex);
	//pMap->TileMap()->SetSliceSize(Vec2(0.125f, 0.166f));
	pMap->TileMap()->SetSliceSize(8, 6);
	pMap->TileMap()->SetTileCount(8, 6);
	//pMap->TileMap()->Load(L"tilemap\\testtile.tile");

	SpawnGameObject(pMap, Vec3(0.f, 0.f, 800.f), L"TileMap");

	// Player
	CGameObject* pPlayer = new CGameObject;
	pPlayer->SetName(L"Player");
	pPlayer->AddComponent(new CTransform);
	pPlayer->AddComponent(new CMeshRender);
	pPlayer->AddComponent(new CPlayerScript);
	pPlayer->AddComponent(new CCollider2D);
	pPlayer->AddComponent(new CLight2D);
	pPlayer->AddComponent(new CAnimator2D);

	Std2DLightMtrl->SetTexParam(TEX_0, CharTex);
	Std2DLightMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"Dissolve"));
	//Std2DLightMtrl->SetTexParam(TEX_1, DeadCellTex_N);
	//Std2DLightMtrl->SetTexParam(TEX_0, CharTex);

	//pPlayer->Transform()->SetRelativeScale(Vec3(CharTex->GetDesc().Width * 3.f, CharTex->GetDesc().Height * 3.f, 1.f));
	pPlayer->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));
	//pPlayer->Transform()->SetRelativeScale(Vec3(DeadCellTex->GetDesc().Width, DeadCellTex->GetDesc().Height, 1.f));

	pPlayer->MeshRender()->SetMesh(RectMesh);
	pPlayer->MeshRender()->SetMaterial(Std2DAnimLightMtrl);
	//pPlayer->MeshRender()->SetMaterial(Anim2DMtrl);

	pPlayer->Collider2D()->SetAbsolute(true);
	pPlayer->Collider2D()->SetOffsetScale(Vec2(150.f, 150.f));
	pPlayer->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);

	pPlayer->Light2D()->SetLightType(LIGHT_TYPE::POINT);
	pPlayer->Light2D()->SetLightDiffuse(Vec3(1.f, 1.f, 1.f));
	pPlayer->Light2D()->SetRadius(150.f);

	//pPlayer->Animator2D()->CreateAnim(L"IdleDown", PlayerTex, Vec2(0.f, 0.f), Vec2(120.f, 130.f), Vec2(300.f, 300.f), 3, 6);
	//pPlayer->Animator2D()->CreateAnim(L"IdleLeft", PlayerTex, Vec2(0.f, 130.f), Vec2(120.f, 130.f), Vec2(300.f, 300.f), 3, 6);
	//pPlayer->Animator2D()->CreateAnim(L"IdleUp", PlayerTex, Vec2(0.f, 260.f), Vec2(120.f, 130.f), Vec2(300.f, 300.f), 1, 6);
	//pPlayer->Animator2D()->CreateAnim(L"IdleRight", PlayerTex, Vec2(0.f, 390.f), Vec2(120.f, 130.f), Vec2(300.f, 300.f), 3, 6);
	//pPlayer->Animator2D()->CreateAnim(L"WalkDown", PlayerTex, Vec2(0.f, 520.f), Vec2(120.f, 130.f), Vec2(300.f, 300.f), 10, 14);
	//pPlayer->Animator2D()->CreateAnim(L"WalkLeft", PlayerTex, Vec2(0.f, 650.f), Vec2(120.f, 130.f), Vec2(300.f, 300.f), 10, 14);
	//pPlayer->Animator2D()->CreateAnim(L"WalkUp", PlayerTex, Vec2(0.f, 780.f), Vec2(120.f, 130.f), Vec2(300.f, 300.f), 10, 14);
	//pPlayer->Animator2D()->CreateAnim(L"WalkRight", PlayerTex, Vec2(0.f, 910.f), Vec2(120.f, 130.f), Vec2(300.f, 300.f), 10, 14);

	pPlayer->Animator2D()->LoadAnim(L"animation\\IdleDown.anim");
	pPlayer->Animator2D()->LoadAnim(L"animation\\IdleLeft.anim");
	pPlayer->Animator2D()->LoadAnim(L"animation\\IdleRight.anim");
	pPlayer->Animator2D()->LoadAnim(L"animation\\IdleUp.anim");
	pPlayer->Animator2D()->LoadAnim(L"animation\\WalkDown.anim");
	pPlayer->Animator2D()->LoadAnim(L"animation\\WalkLeft.anim");
	pPlayer->Animator2D()->LoadAnim(L"animation\\WalkRight.anim");
	pPlayer->Animator2D()->LoadAnim(L"animation\\WalkUp.anim");

	pPlayer->Animator2D()->Play(L"WalkDown", true);

	SpawnGameObject(pPlayer, Vec3(0.f, 0.f, 500.f), 1);

	//// Particle Object
	//CGameObject* pParticle = new CGameObject;
	//
	//pParticle->SetName(L"Particle");
	//pParticle->AddComponent(new CTransform);
	//pParticle->AddComponent(new CParticleSystem);
	//pParticle->ParticleSystem()->Load(L"particle\\Rain.particle");

	//SpawnGameObject(pParticle, Vec3(0.f, 300.f, 500.f), 0);

	//pParticle = new CGameObject;
	//pParticle->SetName(L"ParticleFire");
	//pParticle->AddComponent(new CTransform);
	//pParticle->AddComponent(new CParticleSystem);
	//pParticle->ParticleSystem()->Load(L"particle\\Fire.particle");

	//SpawnGameObject(pParticle, Vec3(100.f, 0.f, 500.f), 0);

	//pParticle = new CGameObject;
	//pParticle->SetName(L"ParticleBlood");
	//pParticle->AddComponent(new CTransform);
	//pParticle->AddComponent(new CParticleSystem);
	//pParticle->ParticleSystem()->Load(L"particle\\Blood.particle");

	////SpawnGameObject(pParticle, Vec3(0.f, 0.f, 500.f), 0);
	//pPlayer->AddChild(pParticle);

	//pParticle = new CGameObject;
	//pParticle->SetName(L"ParticleRipple");
	//pParticle->AddComponent(new CTransform);
	//pParticle->AddComponent(new CParticleSystem);
	//pParticle->ParticleSystem()->Load(L"particle\\Ripple.particle");

	//SpawnGameObject(pParticle, Vec3(0.f, 0.f, 700.f), 0);

	// Aim Object
	CGameObject* pAim = new CGameObject;
	pAim->SetName(L"Gun");
	pAim->AddComponent(new CTransform);
	pAim->AddComponent(new CMeshRender);
	pAim->AddComponent(new CAimScript);
	pAim->AddComponent(new CLight2D);

	GunMtrl->SetTexParam(TEX_0, GunTex);

	pAim->Transform()->SetRelativeScale(Vec3(GunTex->GetDesc().Width * 3.f, GunTex->GetDesc().Height * 3.f, 1.f));
	pAim->Transform()->SetAbsolute(true);
	pAim->MeshRender()->SetMesh(RectMesh);
	pAim->MeshRender()->SetMaterial(GunMtrl);

	pAim->Light2D()->SetLightType(LIGHT_TYPE::SPOT);
	pAim->Light2D()->SetLightDiffuse(Vec3(1.f, 1.f, 1.f));
	pAim->Light2D()->SetRadius(600.f);
	pAim->Light2D()->SetAngle(45.f);
	pAim->Light2D()->SetOffsetPos(Vec3(0.3f, 0.f, 0.f));

	CAimScript* pAimScript = pAim->GetScript<CAimScript>();

	pPlayer->AddChild(pAim);
	//SpawnGameObject(pAim, Vec3(0.f, 0.f, 500.f), 0);

	// Monster
	CGameObject* pMonster = new CGameObject;
	pMonster->SetName(L"Monster_Object");
	pMonster->AddComponent(new CTransform);
	pMonster->AddComponent(new CMeshRender);
	pMonster->AddComponent(new CCollider2D);

	pMonster->Transform()->SetRelativePos(Vec3(0.f, 200.f, -1.f));
	pMonster->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));
	pMonster->Transform()->SetAbsolute(true);

	Std2DMtrl->SetTexParam(TEX_0, pCreateTex);

	pMonster->MeshRender()->SetMesh(RectMesh);
	pMonster->MeshRender()->SetMaterial(Std2DMtrl);

	pMonster->Collider2D()->SetAbsolute(true);
	pMonster->Collider2D()->SetOffsetScale(Vec2(150.f, 150.f));
	pMonster->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

	SpawnGameObject(pMonster, Vec3(0.f, 200.f, -1.f), 3);

	// PostProcess
	//CGameObject* pPostProcess = new CGameObject;
	//pPostProcess->SetName(L"PostProcess");
	//pPostProcess->AddComponent(new CTransform);
	//pPostProcess->AddComponent(new CMeshRender);

	//pPostProcess->MeshRender()->SetMesh(RectMesh);
	//Ptr<CMaterial> pGrayMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"GrayMtrl");
	//pPostProcess->MeshRender()->SetMaterial(pGrayMtrl);

	//SpawnGameObject(pPostProcess, Vec3(0.f, 0.f, 0.f), 3);

	CGameObject* pDistortion = new CGameObject;
	pDistortion->SetName(L"Distortion");
	pDistortion->AddComponent(new CTransform);
	pDistortion->AddComponent(new CMeshRender);
	pDistortion->Transform()->SetRelativeScale(200.f, 200.f, 1.f);
	pDistortion->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pDistortion->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"));
	SpawnGameObject(pDistortion, Vec3(0.f, 0.f, 500.f), 0);

	// Pyramid
	CGameObject* pPyra = new CGameObject;
	pPyra->SetName(L"TriPyra");
	pPyra->AddComponent(new CTransform);
	pPyra->AddComponent(new CMeshRender);

	pPyra->Transform()->SetRelativePos(Vec3(0.f, -200.f, 100.f));
	pPyra->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

	pPyra->MeshRender()->SetMaterial(TestMtrl);
	pPyra->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"TriPyra"));

	SpawnGameObject(pPyra, Vec3(0.f, -200.f, 100.f), 2);

	// Sphere
	CGameObject* pSphere = new CGameObject;
	pSphere->SetName(L"Sphere");
	pSphere->AddComponent(new CTransform);
	pSphere->AddComponent(new CMeshRender);

	pSphere->Transform()->SetRelativePos(Vec3(-200.f, 0.f, 100.f));
	pSphere->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

	pSphere->MeshRender()->SetMaterial(MeshMtrl);
	pSphere->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"Sphere"));

	SpawnGameObject(pSphere, Vec3(-200.f, 0.f, 100.f), 2);
}

void LoadStartLevel()
{
	CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(L"Level\\title.level");
	tEvent evn = {};
	evn.Type = EVENT_TYPE::LEVEL_CHANGE;
	evn.wParam = (DWORD_PTR)pLoadedLevel;

	CEventMgr::GetInst()->AddEvent(evn);
}

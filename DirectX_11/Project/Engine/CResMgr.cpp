#include "pch.h"
#include "CResMgr.h"

#include "CPathMgr.h"

#include "components.h"

#include "CGameObject.h"

#include "CTexture.h"


CResMgr::CResMgr()	:
	m_bChanged(false)
{
}

CResMgr::~CResMgr()
{
}

void CResMgr::init()
{
	InitSound();

	CreateDefaultMesh();
	CreateDefaultGraphicsShader();
	CreateDefaultComputeShader();

	// DebugShape Material

	Ptr<CMaterial> pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"DebugShapeMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DebugShapeShader"));
	AddRes(pMtrl->GetKey(), pMtrl);

	//CreateDefaultMaterial();
}

void CResMgr::tick()
{
	m_bChanged = false;
}

void CResMgr::InitSound()
{
	FMOD::System_Create(&CSound::g_pFMOD);

	if (nullptr == CSound::g_pFMOD)
	{
		assert(nullptr);
	}

	// 32개 채널 생성
	CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);
}

void CResMgr::CreateDefaultMesh()
{
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;
	Vtx v;

	Ptr<CMesh> pMesh = nullptr;

	// ==========
	// Point Mesh
	// ==========
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

	UINT idx = 0;

	pMesh = new CMesh(true);
	pMesh->Create(&v, 1, &idx, 1);
	AddRes(L"PointMesh", pMesh);

	// =============
	// RectMesh 생성
	// =============
	// 0 --- 1 
	// |  \  |
	// 3 --- 2
	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = ChangeNDCtoUV(v.vPos);
	//v.vUV = Vec2(0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = ChangeNDCtoUV(v.vPos);
	//v.vUV = Vec2(1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = ChangeNDCtoUV(v.vPos);
	//v.vUV = Vec2(1.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 0.f, 1.f);
	v.vUV = ChangeNDCtoUV(v.vPos);
	//v.vUV = Vec2(0.f, 1.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(2);
	vecIdx.push_back(3);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	pMesh = new CMesh(true);
	pMesh->SetKey(L"RectMesh");
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(pMesh->GetKey(), pMesh);

	// Debug Rect
	vecIdx.clear();
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);
	vecIdx.push_back(3);
	vecIdx.push_back(0);

	pMesh = new CMesh(true);
	pMesh->SetKey(L"RectMesh_Debug");
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(pMesh->GetKey(), pMesh);

	vecVtx.clear();
	vecIdx.clear();

	// ===========
	// Circle Mesh
	// ===========
	// 12시 위치 부터 시작해서 시계방향으로 회전하는 원
	
	// 중심점
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.5f, 0.5f);
	vecVtx.push_back(v);

	// 반지름
	float fradius = 0.5f;

	// 각도
	UINT iSlice = 40;
	float fTheta = XM_2PI / (float)iSlice;

	for (UINT i = 0; i < iSlice; i++)
	{
		v.vPos = Vec3(fradius * cosf(fTheta * i), fradius * sinf(fTheta * i), 0.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		v.vUV = Vec2(v.vPos.x + 0.5f, -1.f * v.vPos.y + 0.5f);
		vecVtx.push_back(v);
	}

	// 인덱스 설정
	for (UINT i = 0; i < iSlice - 1; i++)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);		
	}

	// 마지막 정점
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(iSlice);

	pMesh = new CMesh(true);
	pMesh->SetKey(L"CircleMesh");
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(pMesh->GetKey(), pMesh);

	// Debug Circle
	vecIdx.clear();
	for (UINT i = 0; i < iSlice; i++)
	{
		vecIdx.push_back(i + 1);
	}
	vecIdx.push_back(1);

	pMesh = new CMesh(true);
	pMesh->SetKey(L"CircleMesh_Debug");
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(pMesh->GetKey(), pMesh);

	vecVtx.clear();
	vecIdx.clear();

	// =============
	// Triangle Mesh
	// =============
	//   0
	//  / \
	// 2---1

	v.vPos = Vec3(0.f, (float)(sqrt(3.f) / 3.f), 0.f);
	v.vUV = ChangeNDCtoUV(v.vPos);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, (float)(sqrt(3.f) / -6.f), 0.f);
	v.vUV = ChangeNDCtoUV(v.vPos);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, (float)(sqrt(3.f) / -6.f), 0.f);
	v.vUV = ChangeNDCtoUV(v.vPos);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);
	vecIdx.push_back(0);

	pMesh = new CMesh(true);
	pMesh->SetKey(L"TriMesh");
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(pMesh->GetKey(), pMesh);

	vecVtx.clear();
	vecIdx.clear();

	// =========
	// Line Mesh
	// =========
	// 0 -- 1
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = ChangeNDCtoUV(v.vPos);
	vecVtx.push_back(v);

	v.vPos = Vec3(1.f, 0.f, 0.f);
	v.vUV = ChangeNDCtoUV(v.vPos);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(1);

	pMesh = new CMesh(true);
	pMesh->SetKey(L"LineDebugMesh");
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(pMesh->GetKey(), pMesh);

	vecVtx.clear();
	vecIdx.clear();

	// ================
	// Triangle Pyramid
	// 3 ----- 4
	// | \   / |
	// |   0   |
	// | /   \ |
	// 2 ----- 1
	// ================
	v.vPos = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);	
	vecVtx.push_back(v);

	v.vPos = Vec3(1.f, -1.f, -1.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-1.f, -1.f, -1.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-1.f, -1.f, 1.f);
	v.vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(1.f, -1.f, 1.f);
	v.vColor = Vec4(0.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	vecIdx.push_back(0);
	vecIdx.push_back(2);
	vecIdx.push_back(3);

	vecIdx.push_back(0);
	vecIdx.push_back(3);
	vecIdx.push_back(4);

	vecIdx.push_back(0);
	vecIdx.push_back(4);
	vecIdx.push_back(1);

	vecIdx.push_back(1);
	vecIdx.push_back(4);
	vecIdx.push_back(3);

	vecIdx.push_back(1);
	vecIdx.push_back(3);
	vecIdx.push_back(2);

	pMesh = new CMesh(true);
	pMesh->SetKey(L"TriPyra");
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes<CMesh>(pMesh->GetKey(), pMesh);

	vecVtx.clear();
	vecIdx.clear();

	// ===========
	// Sphere Mesh
	// ===========
	fradius = 0.5f;
	iSlice = 10;		// 경도
	UINT iStack = 10;		// 위도

	// South Pole
	v.vPos = Vec3(0.f, fradius, 0.f);
	v.vColor = Vec3(1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	float fSliceAngle = 2 * XM_PI / iSlice;
	float fStackAngle = XM_PI / iStack;

	for (UINT i = 1; i <= iStack; i++)
	{
		float fStack = fStackAngle * i;
		for (UINT j = 0; j < iSlice; j++)
		{
			float fSlice = fSliceAngle * j;
			v.vPos = Vec3(sinf(fStack) * cosf(fSlice), cosf(fStack), sinf(fStack) * sinf(fSlice)) * fradius;
			v.vColor = Vec3(1.f, 1.f, 1.f);
			vecVtx.push_back(v);
		}
	}

	v.vPos = Vec3(0.f, -1.f * fradius, 0.f);
	v.vColor = Vec3(1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	// Top Stack
	for (UINT i = 1; i < iSlice; i++)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 1);
		vecIdx.push_back(i);
	}
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(iSlice);

	// Center Stacks
	for (UINT i = 0; i < iStack - 1; i++)
	{
		for (UINT j = 1; j < iSlice; j++)
		{
			vecIdx.push_back(i * iSlice + j);
			vecIdx.push_back((i + 1) * iSlice + j + 1);
			vecIdx.push_back((i + 1) * iSlice + j);

			vecIdx.push_back(i * iSlice + j);
			vecIdx.push_back(i * iSlice + j + 1);
			vecIdx.push_back((i + 1) * iSlice + j + 1);
		}

		vecIdx.push_back((i + 1) * iSlice);
		vecIdx.push_back((i + 1) * iSlice + 1);
		vecIdx.push_back((i + 2) * iSlice);

		vecIdx.push_back((i + 1) * iSlice);
		vecIdx.push_back(i * iSlice + 1);
		vecIdx.push_back((i + 1) * iSlice + 1);
	}

	// Bottom Stack
	UINT iBottomIdx = iSlice * (iStack - 2) + 1;
	for (UINT i = 1; i < iSlice; i++)
	{
		vecIdx.push_back(iBottomIdx);
		vecIdx.push_back(iBottomIdx - iSlice);
		vecIdx.push_back(iBottomIdx - iSlice + 1);
	}
	vecIdx.push_back(iBottomIdx);
	vecIdx.push_back(iBottomIdx - 1);
	vecIdx.push_back(iBottomIdx - iSlice);

	pMesh = new CMesh(true);
	pMesh->SetKey(L"Sphere");
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes<CMesh>(pMesh->GetKey(), pMesh);

	vecVtx.clear();
	vecIdx.clear();
}


void CResMgr::CreateDefaultGraphicsShader()
{
	// Shader 생성
	Ptr<CGraphicsShader> pShader = nullptr;

	// ===========
	// Test Shader
	// ===========
	pShader = new CGraphicsShader();
	pShader->SetKey(L"TestShader");
	pShader->CreateVertexShader(L"shader\\test.fx", "VS_Test");
	pShader->CreatePixelShader(L"shader\\test.fx", "PS_Test");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	//pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	pShader->AddTexParam(TEX_0, "Output Texture");

	AddRes(pShader->GetKey(), pShader);

	// ==================================
	// MeshShader
	// RasterizerState   : Wire-frame
	// DepthStencilState : Less
	// BlendState        : Mask
	//===================================
	pShader = new CGraphicsShader();
	pShader->SetKey(L"MeshShader");
	pShader->CreateVertexShader(L"shader\\test.fx", "VS_Test");
	pShader->CreatePixelShader(L"shader\\test.fx", "PS_Test");
	pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::MASK);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	AddRes(pShader->GetKey(), pShader);

	// ===========
	// SetColor Shader
	// ===========
	pShader = new CGraphicsShader();
	pShader->SetKey(L"SetColorShader");
	pShader->CreateVertexShader(L"shader\\test.fx", "VS_SetColor");
	pShader->CreatePixelShader(L"shader\\test.fx", "PS_SetColor");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes(pShader->GetKey(), pShader);

	// ==================================
	// Std2DShader
	// RasterizerState   : None
	// DepthStencilState : Less
	// BlendState        : Mask
	//
	// Paramter
	// g_tex_0           : Output Texture
	//===================================
	pShader = new CGraphicsShader();
	pShader->SetKey(L"Std2DShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	pShader->AddTexParam(TEX_0, "Output Texture");

	AddRes(pShader->GetKey(), pShader);

	// ==================
	// DebugShape Shader
	// Topology : Line_Strip
	// RS_Type  : CULL_NONE
	// DS_Type  : NO_TEST_NO_WRITE
	// BS_Type  : Default
	// g_vec4_0 : OutColor
	// ==================
	pShader = new CGraphicsShader();
	pShader->SetKey(L"DebugShapeShader");
	pShader->CreateVertexShader(L"shader\\debugshape.fx", "VS_DebugShape");
	pShader->CreatePixelShader(L"shader\\debugshape.fx", "PS_DebugShape");
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	pShader->AddScalarParam(VEC4_0, "Out Color");

	AddRes(pShader->GetKey(), pShader);


	// ==================================
	// Std2DLightShader
	// RasterizerState   : None
	// BlendState        : Mask
	// DepthStencilState : Less
	//
	// Paramter
	// g_tex_0           : Output Texture
	// g_tex_1           : Normal Texture
	//===================================
	pShader = new CGraphicsShader();
	pShader->SetKey(L"Std2DLightShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2DLight");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2DLight");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	pShader->AddTexParam(TEX_0, "Output Texture");
	pShader->AddScalarParam(INT_3, "Test_int3");
	pShader->AddScalarParam(FLOAT_3, "Test_float3");
	pShader->AddScalarParam(VEC2_3, "Test_vec2_3");
	pShader->AddScalarParam(VEC4_3, "Test_vec4_3");
	pShader->AddScalarParam(MAT_3, "Test_Mat_3");

	AddRes(pShader->GetKey(), pShader);

	// ============================
	// TileMap Shader
	// 
	// RS_TYPE : CULL_NONE
	// DS_TYPE : LESS
	// BS_TYPE : MASK
	// 
	// Parameter
	// g_tex_0 : Tile Atlas Texture
	// g_int_0 : X
	// g_int_1 : Y
	// ============================
	pShader = new CGraphicsShader();
	pShader->SetKey(L"TileMapShader");
	pShader->CreateVertexShader(L"shader\\tilemap.fx", "VS_TileMap");
	pShader->CreatePixelShader(L"shader\\tilemap.fx", "PS_TileMap");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::MASK);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	AddRes(pShader->GetKey(), pShader);

	// ============================
	// ParticleRender Shader
	// 
	// RS_TYPE	: CULL_NONE
	// DS_TYPE	: NO_WRITE
	// BS_TYPE	: ALPHA_BLEND
	// Domain	: TRANSPARENT
	// 
	// Parameter
	// g_int_0	: Particle Index
	// ============================
	pShader = new CGraphicsShader();
	pShader->SetKey(L"ParticleRenderShader");
	pShader->CreateVertexShader(L"shader\\particle_render.fx", "VS_ParticleRender");
	pShader->CreateGeometryShader(L"shader\\particle_render.fx", "GS_ParticleRender");
	pShader->CreatePixelShader(L"shader\\particle_render.fx", "PS_ParticleRender");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	AddRes(pShader->GetKey(), pShader);

	// ====================
	// GrayShader
	// RS_TYPE	: CULL_NONE
	// DS_TYPE	: NO_TEST_NO_WRITE
	// BS_TYPE	: DEFUALT
	// Domain	: POSTPROCESS
	//====================	
	pShader = new CGraphicsShader();
	pShader->SetKey(L"GrayShader");
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_GrayShader");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_GrayShader");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	//pShader->AddTexParam(TEX_0, "Render Target");
	AddRes(pShader->GetKey(), pShader);

	// ====================
	// DistortionShader
	// RS_TYPE	: CULL_NONE
	// DS_TYPE	: NO_TEST_NO_WRITE
	// BS_TYPE	: DEFUALT
	// Domain	: POSTPROCESS
	//====================	
	pShader = new CGraphicsShader();
	pShader->SetKey(L"DistortionShader");
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_DistortionShader");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_DistortionShader");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	//pShader->AddTexParam(TEX_0, "Render Target");
	AddRes(pShader->GetKey(), pShader);

	// ==================================
	// UIShader
	// RasterizerState   : None
	// BlendState        : ALPHA_BLEND
	// DepthStencilState : NO_TEST_NO_WRITE
	// Domain			: UI
	//
	// Paramter
	// g_int_0          : Color Change
	// g_vec4_0         : default color
	// g_vec4_1         : changed color
	// g_int_1          : use alpha blend
	// g_float_0        : alpha value
	// g_tex_0          : Output Texture
	//===================================

	pShader = new CGraphicsShader();
	pShader->SetKey(L"UIShader");
	pShader->CreateVertexShader(L"shader\\ui.fx", "VS_UIShader");
	pShader->CreatePixelShader(L"shader\\ui.fx", "PS_UIShader");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_UI);
	pShader->AddTexParam(TEX_0, "Output Texture");
	AddRes(pShader->GetKey(), pShader);


	// ==================================
	// AlphaBlendLight
	// RasterizerState   : None
	// BlendState        : ALPHA_BLEND
	// DepthStencilState : Less
	//
	// Paramter
	//===================================

	pShader = new CGraphicsShader();
	pShader->SetKey(L"ABLightShader");
	pShader->CreateVertexShader(L"shader\\AlphaBlend.fx", "VS_ABLight");
	pShader->CreatePixelShader(L"shader\\AlphaBlend.fx", "PS_ABLight");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	pShader->AddTexParam(TEX_0, "Output Texture");
	AddRes(pShader->GetKey(), pShader);

	// ==================================
	// FadeShader
	// Mesh : RectMesh
	// Domain : PostProcess
	// g_tex_0 : RenderTarget Copy
	// g_float_0 : Black Value
	// g_vec4_0 : Mix Color
	//===================================
	pShader = new CGraphicsShader();
	pShader->SetKey(L"FadeShader");
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_FadeShader");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_FadeShader");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddRes(pShader->GetKey(), pShader);

	// ==================================
	// LetterBoxShader
	// Mesh : RectMesh
	// Domain : PostProcess
	// g_tex_0 : RenderTarget Copy
	// g_float_0 : LetterBox Size
	// g_vec4_0 : LetterBox Color
	//===================================
	pShader = new CGraphicsShader();
	pShader->SetKey(L"LetterBoxShader");
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_LetterBoxShader");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_LetterBoxShader");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddRes(pShader->GetKey(), pShader);
}

#include "CSetColorShader.h"
#include "CParticleUpdateShader.h"
#include "CRoomCollisionShader.h"
#include "CRoomShadowShader.h"
#include "CObstacleMapShader.h"

void CResMgr::CreateDefaultComputeShader()
{
	Ptr<CComputeShader> pCS = nullptr;
	// Texture 색상 변경 쉐이더

	pCS = new CSetColorShader(32, 32, 1);
	pCS->SetKey(L"SetColorCS");
	pCS->CreateComputeShader(L"shader\\setcolor.fx", "CS_SetColor");
	AddRes(pCS->GetKey(), pCS);

	// Particle Update 쉐이더
	pCS = new CParticleUpdateShader(128, 1, 1);
	pCS->SetKey(L"ParticleUpdateCS");
	pCS->CreateComputeShader(L"shader\\particle_update.fx", "CS_ParticleUpdate");
	AddRes(pCS->GetKey(), pCS);

	// RoomCollison
	pCS = new CRoomCollisionShader(32, 32, 1);
	pCS->SetKey(L"RoomCollisionCS");
	pCS->CreateComputeShader(L"shader\\room_collision.fx", "CS_RoomCollision");
	AddRes(pCS->GetKey(), pCS);

	// RoomShadow
	pCS = new CRoomShadowShader(32, 32, 1);
	pCS->SetKey(L"RoomShadowCS");
	pCS->CreateComputeShader(L"shader\\room_shadow.fx", "CS_RoomShadow");
	AddRes(pCS->GetKey(), pCS);

	// ObstacleMap
	pCS = new CObstacleMapShader(32, 32, 1);
	pCS->SetKey(L"ObstacleMapCS");
	pCS->CreateComputeShader(L"shader\\obstacle_map.fx", "CS_ObstacleMap");
	AddRes(pCS->GetKey(), pCS);
}

void CResMgr::CreateDefaultMaterial()
{
	Ptr<CMaterial> pMtrl = nullptr;

	//// Std2D Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"Std2DMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// Std2DAnim Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"Std2DAnimMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// Std2DLight Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"Std2DLightMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// Std2DAnimLight Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"Std2DAnimLightMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// DebugShape Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"DebugShapeMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DebugShapeShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// TileMap Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"TileMapMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"TileMapShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// Missile Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"MissileMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// Particle Render Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"ParticleRenderMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"ParticleRenderShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());






	// 내가 만든 머티리얼
	// Mesh Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"MeshMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"MeshShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// Aim Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"AimDiamondMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// Map Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"MapMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// Gun Material
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"GunMtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());

	// Monster1 Mtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetKey(L"Mon1Mtrl");
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(pMtrl->GetKey(), pMtrl);
	pMtrl->Save(L"material\\" + pMtrl->GetKey());
}

Vec2 CResMgr::ChangeNDCtoUV(Vec3 _vPos)
{
	Vec4 vPos = { _vPos.x, _vPos.y, _vPos.z, 1 };

	vPos.x += 0.5f;
	vPos.y = vPos.y * -1.f + 0.5f;

	return Vec2(vPos.x, vPos.y);
}

void CResMgr::DeleteRes(RES_TYPE _type, const wstring _strKey)
{
	map<wstring, Ptr<CRes>>::iterator iter = m_arrRes[(UINT)_type].find(_strKey);
	assert(!(iter == m_arrRes[(UINT)_type].end()));
	m_arrRes[(UINT)_type].erase(iter);
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, UINT _iWidth, UINT _iHeight, 
	DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage)
{
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);

	if (nullptr != pTex)
		return pTex;

	pTex = new CTexture(true);
	if (FAILED(pTex->Create(_iWidth, _iHeight, _Format, _BindFlag, _Usage)))
	{
		assert(nullptr);
	}

	AddRes<CTexture>(_strKey, pTex);

	return pTex;
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _Tex2D)
{
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);

	assert(nullptr == pTex);

	pTex = new CTexture(true);
	if (FAILED(pTex->Create(_Tex2D)))
	{
		assert(nullptr);
	}

	AddRes<CTexture>(_strKey, pTex);

	return pTex;
}
	
#include "pch.h"
#include "CParticleSystem.h"

#include "CDevice.h"
#include "CResMgr.h"
#include "CTimeMgr.h"
#include "CPathMgr.h"
#include "CStructuredBuffer.h"

#include "CTransform.h"
#include "CMaterial.h"

CParticleSystem::CParticleSystem()	:
	CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM),
	m_ParticleBuffer(nullptr),
	m_RWBuffer(nullptr),
	m_ModuleDataBuffer(nullptr),
	m_ModuleData{},
	m_fAccTime(0.f)
{
	m_bIsFrustumCull = false;

	m_ModuleData.MaxParticleCount = 1000;
	m_ModuleData.SpawnRate = 10;
	m_ModuleData.vSpawnColorMin = Vec4(1.f, 1.f, 1.f, 1.f);
	m_ModuleData.vSpawnColorMax = Vec4(1.f, 1.f, 1.f, 1.f);
	m_ModuleData.vBoxShapeScale = Vec3(100.f, 100.f, 1.f);
	m_ModuleData.fSphereShapeRadius = 100.f;
	m_ModuleData.vSpawnScaleMin = Vec3(10.f, 10.f, 1.f);
	m_ModuleData.vSpawnScaleMax = Vec3(10.f, 10.f, 1.f);
	m_ModuleData.MinLifeTime = 1.f;
	m_ModuleData.MaxLifeTime = 1.f;
	m_ModuleData.StartScale = 1.f;
	m_ModuleData.EndScale = 1.f;
	m_ModuleData.vStartColor = Vec4(1.f, 1.f, 1.f, 1.f);
	m_ModuleData.vEndColor = Vec4(1.f, 1.f, 1.f, 1.f);
	m_ModuleData.MinSpeed = 100.f;
	m_ModuleData.MaxSpeed = 100.f;
	m_ModuleData.vVelocityDir = Vec3(1.f, 0.f, 0.f);
	m_ModuleData.StartDrag = 100.f;
	m_ModuleData.EndDrag = 0.f;
	m_ModuleData.fNoiseTerm = 1.f;
	m_ModuleData.fNoiseForce = 100.f;
	m_ModuleData.MaxVelocityScale = Vec3(1.f, 1.f, 1.f);
	m_ModuleData.fMaxSpeed = 100.f;
	m_ModuleData.TileX = 1;
	m_ModuleData.TileY = 1;
	m_ModuleData.fDuration = 1.f;
	m_ModuleData.vGravityDir = Vec3(0.f, -1.f, 0.f);
	m_ModuleData.GravityScale = 9.8f;

	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));
	SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl"));
	m_UpdateCS = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateCS").Get();

	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_ModuleData.MaxParticleCount, SB_TYPE::READ_WRITE, false);

	m_RWBuffer = new CStructuredBuffer;
	m_RWBuffer->Create(sizeof(tRWParticleBuffer), 1, SB_TYPE::READ_WRITE, true);

	m_ModuleDataBuffer = new CStructuredBuffer;
	m_ModuleDataBuffer->Create(sizeof(tParticleModule), 1, SB_TYPE::READ_ONLY, true);

	GetDynamicMaterial();
}

CParticleSystem::CParticleSystem(const CParticleSystem& _Origin)	:
	CRenderComponent(_Origin),
	m_ModuleData(_Origin.m_ModuleData),
	m_UpdateCS(_Origin.m_UpdateCS),
	m_fAccTime(0.f)
{
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_ModuleData.MaxParticleCount, SB_TYPE::READ_WRITE, false);

	m_RWBuffer = new CStructuredBuffer;
	m_RWBuffer->Create(sizeof(tRWParticleBuffer), 1, SB_TYPE::READ_WRITE, true);

	m_ModuleDataBuffer = new CStructuredBuffer;
	m_ModuleDataBuffer->Create(sizeof(tParticleModule), 1, SB_TYPE::READ_ONLY, true);

	GetDynamicMaterial();

	Ptr<CTexture> pTex = _Origin.GetMaterial()->GetTexParam(TEX_0);
	if(nullptr != pTex)
		GetDynamicMaterial()->SetTexParam(TEX_0, pTex);
}

CParticleSystem::~CParticleSystem()
{
	if (nullptr != m_ParticleBuffer)
		delete m_ParticleBuffer;

	if (nullptr != m_RWBuffer)
		delete m_RWBuffer;

	if (nullptr != m_ModuleDataBuffer)
		delete m_ModuleDataBuffer;
}

void CParticleSystem::finaltick()
{
	// SpawnRate 계산
	float fTimePerCount = 1.f / (float)m_ModuleData.SpawnRate;
	m_fAccTime += DT;
	if (fTimePerCount < m_fAccTime)
	{
		float fData = m_fAccTime / fTimePerCount;
		m_fAccTime -= floorf(fData) * fTimePerCount;

		tRWParticleBuffer rwBuffer = { (int)fData, };
		m_RWBuffer->SetData(&rwBuffer);
	}

	m_ModuleDataBuffer->SetData(&m_ModuleData);

	m_UpdateCS->SetParticleBuffer(m_ParticleBuffer);
	m_UpdateCS->SetRWParticleBuffer(m_RWBuffer);
	m_UpdateCS->SetModuleData(m_ModuleDataBuffer);
	m_UpdateCS->SetNoiseTexture(CResMgr::GetInst()->FindRes<CTexture>(L"noise_01"));
	m_UpdateCS->SetParticleObjectPos(Transform()->GetWorldPos());

	m_UpdateCS->Execute();
}

void CParticleSystem::render()
{
	Transform()->UpdateData();

	m_ParticleBuffer->UpdateData(20, PIPELINE_STAGE::PS_ALL);
	m_ModuleDataBuffer->UpdateData(21, PIPELINE_STAGE::PS_GEOMETRY | PS_PIXEL);

	GetMaterial()->UpdateData();
	GetMesh()->render_particle(m_ModuleData.MaxParticleCount);

	m_ParticleBuffer->Clear();
	m_ModuleDataBuffer->Clear();
}

void CParticleSystem::SetModuleData(tParticleModule& _ModuleData)
{
	if (m_ModuleData.MaxParticleCount != _ModuleData.MaxParticleCount)
	{
		m_ParticleBuffer->Create(sizeof(tParticle), _ModuleData.MaxParticleCount, SB_TYPE::READ_WRITE, false);
	}

	m_ModuleData = _ModuleData;
}

void CParticleSystem::SetBufferMaxCount(UINT _MaxCount)
{
	m_ModuleData.MaxParticleCount = _MaxCount;
	m_ParticleBuffer->Create(sizeof(tParticle), m_ModuleData.MaxParticleCount, SB_TYPE::READ_WRITE, false);
}

void CParticleSystem::Save(const wstring& _strPath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strPath;

	// 파일 입출력
	FILE* pFile = nullptr;
	errno_t iErrNo = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Particle Save 실패, Error Number = %d", iErrNo);
		MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
		return;
	}

	// Material 저장
	CRenderComponent::SaveToLevelFile(pFile);

	// ModuleData 저장
	fwrite(&m_ModuleData, sizeof(tParticleModule), 1, pFile);

	// Compute Shader 저장
	SaveResRef(m_UpdateCS.Get(), pFile);

	fclose(pFile);
}

void CParticleSystem::Load(const wstring& _strPath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strPath;

	// 파일 입출력
	FILE* pFile = nullptr;
	errno_t iErrNo = _wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Particle Load 실패, Error Number = %d", iErrNo);
		MessageBox(nullptr, szStr, L"파일 불러오기 실패", MB_OK);
		return;
	}

	// Material Load
	CRenderComponent::LoadFromLevelFile(pFile);

	// ModuleData Load
	fread(&m_ModuleData, sizeof(tParticleModule), 1, pFile);

	// Compute Shader Load
	int i = 0;
	fread(&i, sizeof(i), 1, pFile);

	if (i)
	{
		wstring strKey, strRelativePath;
		LoadWString(strKey, pFile);
		LoadWString(strRelativePath, pFile);

		m_UpdateCS = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(strKey).Get();
	}
	
	fclose(pFile);
}

void CParticleSystem::SaveToLevelFile(FILE* _pFile)
{
	CRenderComponent::SaveToLevelFile(_pFile);

	fwrite(&m_ModuleData, sizeof(tParticleModule), 1, _pFile);
	SaveResRef(m_UpdateCS.Get(), _pFile);
}

void CParticleSystem::LoadFromLevelFile(FILE* _pFile)
{
	CRenderComponent::LoadFromLevelFile(_pFile);

	fread(&m_ModuleData, sizeof(tParticleModule), 1, _pFile);

	int i = 0;
	fread(&i, sizeof(i), 1, _pFile);

	if (i)
	{
		wstring strKey, strRelativePath;
		LoadWString(strKey, _pFile);
		LoadWString(strRelativePath, _pFile);

		m_UpdateCS = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(strKey).Get();
	}
}

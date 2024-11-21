#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CTimeMgr.h"
#include "CResMgr.h"
#include "CConstBuffer.h"
#include "CCamera.h"
#include "CTransform.h"

#include "CStructuredBuffer.h"

CRenderMgr::CRenderMgr()	:
	m_pEditorCam(nullptr),
	m_Light2DBuffer(nullptr),
	RENDER_FUNC(nullptr),
	m_bIsPlayingLevelChange(false)
{
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	m_RTCopyTex = CResMgr::GetInst()->CreateTexture(L"RTCopyTex"
		, (UINT)vResolution.x, (UINT)vResolution.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE
		, D3D11_USAGE_DEFAULT);
}

CRenderMgr::~CRenderMgr()
{
	if (nullptr != m_Light2DBuffer)
		delete m_Light2DBuffer;
}

Vec3 CRenderMgr::GetEditorCamPos()
{
	if (nullptr != m_pEditorCam)
	{
		Vec3 vPos = m_pEditorCam->Transform()->GetRelativePos();
		vPos.z = 0.f;
		return vPos;
	}
	else
		return Vec3();
}

void CRenderMgr::init()
{
	// ���� ����ȭ ���� ����
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light2DBuffer->Create(sizeof(tLightInfo), 10, SB_TYPE::READ_ONLY, true);
}

void CRenderMgr::render()
{
	// ������ ����
	float arrColor[4] = { 0.f, 0.f, 0.f, 1.f };
	CDevice::GetInst()->ClearTarget(arrColor);

	// ��� Ÿ�� ����
	CDevice::GetInst()->OMSet();

	// ���� �� ���� ������ ������Ʈ �� ���ε�
	UpdateData();

	// ���� �Լ� ȣ��
	if (false == m_bIsPlayingLevelChange)
		(this->*RENDER_FUNC)();
	else
		m_bIsPlayingLevelChange = false;

	// ���� ����
	Clear();
}

int CRenderMgr::RegisterCamera(CCamera* _Cam, int _Idx)
{
	if (m_vecCam.size() <= _Idx)
	{
		m_vecCam.resize(_Idx + 1);
	}

	m_vecCam[_Idx] = _Cam;
	return _Idx;
}

void CRenderMgr::SetRenderFunc(bool _bIsPlay)
{
	if (_bIsPlay)
		RENDER_FUNC = &CRenderMgr::render_play;
	else
		RENDER_FUNC = &CRenderMgr::render_editor;
}

void CRenderMgr::CopyRenderTarget()
{
	Ptr<CTexture> pRTTex = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_RTCopyTex->GetTex2D().Get(), pRTTex->GetTex2D().Get());
}

void CRenderMgr::UpdateData()
{
	// ���� ������ �۷ι� ������ۿ� ����
	GlobalData.Light2DCount = (UINT)m_vecLight2D.size();

	// ����ȭ ������ ũ�Ⱑ ���ڶ�� �� ũ�� ���� �����
	if (m_Light2DBuffer->GetElementCount() < m_vecLight2D.size())
	{
		m_Light2DBuffer->Create(sizeof(tLightInfo), (UINT)m_vecLight2D.size(), SB_TYPE::READ_ONLY, true);
	}

	m_Light2DBuffer->SetData(m_vecLight2D.data(), sizeof(tLightInfo) * (UINT)m_vecLight2D.size());
	m_Light2DBuffer->UpdateData(12, PIPELINE_STAGE::PS_PIXEL);

	CConstBuffer* pGlobalBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	pGlobalBuffer->SetData(&GlobalData, sizeof(tGlobal));
	pGlobalBuffer->UpdateData();
	pGlobalBuffer->UpdateData_CS();
}

void CRenderMgr::render_play()
{
	for (size_t i = 0; i < m_vecCam.size(); i++)
	{
		if (nullptr == m_vecCam[i])
			continue;

		m_vecCam[i]->SortObject();
		m_vecCam[i]->render();
	}
}

void CRenderMgr::render_editor()
{
	assert(m_pEditorCam);

	m_pEditorCam->SortObject();
	m_pEditorCam->render();
}

void CRenderMgr::Clear()
{
	m_vecLight2D.clear();
}

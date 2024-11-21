#include "pch.h"
#include "CCamera.h"

#include "CDevice.h"
#include "CRenderMgr.h"
#include "CLevelMgr.h"

#include "CTransform.h"
#include "CRenderComponent.h"
#include "CLevel.h"

#include "CLayer.h"
#include "CGameObject.h"
#include "CMaterial.h"
#include "CGraphicsShader.h"

CCamera::CCamera() :
	CComponent(COMPONENT_TYPE::CAMERA),
	m_fAspectRatio(1.f),
	m_fScale(1.f),
	m_ProjType(PROJ_TYPE::ORTHOGRAPHIC),
	m_iLayerMask(0),
	m_iCamIdx(-1)
{
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();
	m_fAspectRatio = vRenderResolution.x / vRenderResolution.y;
}

CCamera::CCamera(const CCamera& _Other)	:
	CComponent(_Other),
	m_fAspectRatio(_Other.m_fAspectRatio),
	m_fScale(_Other.m_fScale),
	m_ProjType(_Other.m_ProjType),
	m_iLayerMask(_Other.m_iLayerMask),
	m_iCamIdx(-1)
{
}

CCamera::~CCamera()
{
}

void CCamera::SetLayerMask(int _iLayer, bool _Visible)
{
	if (_Visible)
	{
		m_iLayerMask |= 1 << _iLayer;
	}
	else
	{
		m_iLayerMask &= ~(1 << _iLayer);
	}
}

void CCamera::SetLayerMaskAll(bool _Visible)
{
	if (_Visible)
	{
		m_iLayerMask = 0xffffffff;
	}
	else
	{
		m_iLayerMask = 0;
	}
}

void CCamera::SetCameraIndex(int _idx)
{
	m_iCamIdx = _idx;
	CRenderMgr::GetInst()->RegisterCamera(this, m_iCamIdx);
}

void CCamera::begin()
{
	if (-1 != m_iCamIdx)
	{
		CRenderMgr::GetInst()->RegisterCamera(this, m_iCamIdx);
	}
}

void CCamera::finaltick()
{
	CalcViewMat();
	CalcProjMat();

	m_vCamRightTopPos = Transform()->GetRelativePos().XY();
	m_vCamRightTopPos += m_vProjSize / 2.f;
	m_vCamLeftBottomPos = Transform()->GetRelativePos().XY();
	m_vCamLeftBottomPos -= m_vProjSize / 2.f;
}

void CCamera::SortObject()
{
	clear();
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// 레이어 마스크 확인
		if (m_iLayerMask & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);

			const vector<CGameObject*>& vecObject = pLayer->GetObjects();

			for (size_t j = 0; j < vecObject.size(); j++)
			{
				CRenderComponent* pRenderCom = vecObject[j]->GetRenderComponent();

				if (nullptr == pRenderCom ||
					!FrustrumCheck(pRenderCom) || 
					nullptr == pRenderCom->GetMaterial() ||
					nullptr == pRenderCom->GetMaterial()->GetShader())
				{
					continue;
				}

				SHADER_DOMAIN eDomain = pRenderCom->GetMaterial()->GetShader()->GetDomain();

				switch (eDomain)
				{
				case SHADER_DOMAIN::DOMAIN_OPAQUE:
					m_vecOpaque.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_MASK:
					m_vecMask.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
					m_vecTransparent.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
					m_vecPost.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_UI:
					m_vecUI.push_back(vecObject[j]);
					break;
				}
			}
		}
	}
}

void CCamera::render()
{
	// Update Transformation Matrix
	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;

	render_opaque();
	render_mask();
	render_transparent();
	render_ui();
	render_postprocess();
}

void CCamera::SaveToLevelFile(FILE* _pFile)
{
	fwrite(&m_fAspectRatio, sizeof(m_fAspectRatio), 1, _pFile);
	fwrite(&m_fScale, sizeof(m_fScale), 1, _pFile);
	fwrite(&m_ProjType, sizeof(m_ProjType), 1, _pFile);
	fwrite(&m_iLayerMask, sizeof(m_iLayerMask), 1, _pFile);
	fwrite(&m_iCamIdx, sizeof(m_iCamIdx), 1, _pFile);
}

void CCamera::LoadFromLevelFile(FILE* _pFile)
{
	fread(&m_fAspectRatio, sizeof(m_fAspectRatio), 1, _pFile);
	fread(&m_fScale, sizeof(m_fScale), 1, _pFile);
	fread(&m_ProjType, sizeof(m_ProjType), 1, _pFile);
	fread(&m_iLayerMask, sizeof(m_iLayerMask), 1, _pFile);
	fread(&m_iCamIdx, sizeof(m_iCamIdx), 1, _pFile);
}

void CCamera::clear()
{
	m_vecOpaque.clear();
	m_vecMask.clear();
	m_vecTransparent.clear();
	m_vecPost.clear();
	m_vecUI.clear();
}

void CCamera::CalcViewMat()
{
	// View Matrix
	m_matView = XMMatrixIdentity();

	// 원점으로 이동
	Vec3 vCamPos = Transform()->GetRelativePos() + m_vOffset;
	Matrix matViewTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

	// z축을 바라보도록 지정
	Matrix matViewRot = XMMatrixIdentity();

	Vec3 vR = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vU = Transform()->GetWorldDir(DIR_TYPE::UP);
	Vec3 vF = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	matViewRot._11 = vR.x; matViewRot._12 = vU.x; matViewRot._13 = vF.x;
	matViewRot._21 = vR.y; matViewRot._22 = vU.y; matViewRot._23 = vF.y;
	matViewRot._31 = vR.z; matViewRot._32 = vU.z; matViewRot._33 = vF.z;

	//Vec3 vCamRot = Transform()->GetRelativeRot();
	//Matrix matViewRot1 = XMMatrixRotationX(vCamRot.x) * XMMatrixRotationY(vCamRot.y) * XMMatrixRotationZ(vCamRot.z);
	//matViewRot1 = matViewRot1.Invert();

	m_matView = matViewTrans * matViewRot;
}

void CCamera::CalcProjMat()
{
	// Projection Matrix
	m_matProj = XMMatrixIdentity();

	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		//직교 투영
		Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
		m_vProjSize = Vec2(vResolution.x / m_fScale, vResolution.y / m_fScale);
		m_matProj = XMMatrixOrthographicLH(m_vProjSize.x, m_vProjSize.y, 1.f, 10000.f);
	}
	else
	{
		// 원근 투영
		m_matProj = XMMatrixPerspectiveFovLH(XM_PI / 2.f, m_fAspectRatio, 1.f, 10000.f);
	}
}

void CCamera::render_opaque()
{
	for (size_t i = 0; i < m_vecOpaque.size(); i++)
	{
		m_vecOpaque[i]->render();
	}
}

void CCamera::render_mask()
{
	for (size_t i = 0; i < m_vecMask.size(); i++)
	{
		m_vecMask[i]->render();
	}
}

int compare(const void* _lParam, const void* _RParam)
{
	const float leftZ = (*(CGameObject**)_lParam)->Transform()->GetRelativePos().z;
	const float rightZ = (*(CGameObject**)_RParam)->Transform()->GetRelativePos().z;

	if (leftZ > rightZ)
		return -1;
	else if (leftZ < rightZ)
		return 1;

	return 0;
}

void CCamera::render_transparent()
{
	qsort(m_vecTransparent.data(), m_vecTransparent.size(), sizeof(CGameObject*), compare);

	for (size_t i = 0; i < m_vecTransparent.size(); i++)
	{
		m_vecTransparent[i]->render();
	}
}

void CCamera::render_postprocess()
{
	for (size_t i = 0; i < m_vecPost.size(); i++)
	{
		CRenderMgr::GetInst()->CopyRenderTarget();
		m_vecPost[i]->render();
	}
}

void CCamera::render_ui()
{
	for (size_t i = 0; i < m_vecUI.size(); i++)
	{
		m_vecUI[i]->render();
	}
}

bool CCamera::FrustrumCheck(CRenderComponent* _pCom)
{
	if (nullptr == _pCom)
		return false;

	if (true == _pCom->IsFrustumCulling())
	{
		Vec3 vPos = _pCom->Transform()->GetRelativePos();
		Vec3 vScale = _pCom->Transform()->GetRelativeScale() / 2.f;
		float fRadius = sqrt(vScale.x * vScale.x + vScale.y * vScale.y);

		if (vPos.x <= (m_vCamRightTopPos.x + fRadius) && vPos.y <= (m_vCamRightTopPos.y + fRadius) &&
			vPos.x >= (m_vCamLeftBottomPos.x - fRadius) && vPos.y >= (m_vCamLeftBottomPos.y - fRadius))
			return true;
		else
			return false;
	}
	else
		return true;
}

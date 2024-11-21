#include "pch.h"
#include "CRenderComponent.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)	:
	CComponent(_Type),
	m_bIsFrustumCull(true)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _Origin) :
	CComponent(_Origin),
	m_pMesh(_Origin.m_pMesh),
	m_pSharedMtrl(_Origin.m_pSharedMtrl),
	m_pDynamicMtrl(nullptr),
	m_pCurrentMtrl(m_pSharedMtrl),
	m_bIsFrustumCull(_Origin.m_bIsFrustumCull)
{
	if (nullptr != _Origin.m_pDynamicMtrl)
	{
		m_pDynamicMtrl = _Origin.m_pDynamicMtrl->Clone();
		m_pCurrentMtrl = m_pDynamicMtrl;
	}
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _pMaterial)
{
	m_pSharedMtrl = _pMaterial;
	
	if (nullptr != m_pDynamicMtrl)
	{
		m_pDynamicMtrl = m_pSharedMtrl->Clone();
		m_pCurrentMtrl = m_pDynamicMtrl;
	}
	else
	{
		m_pCurrentMtrl = m_pSharedMtrl;
	}
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
	if (nullptr == m_pSharedMtrl)
	{
		m_pCurrentMtrl = nullptr;
		return m_pCurrentMtrl;
	}

	if (nullptr == m_pDynamicMtrl)
	{
		m_pDynamicMtrl = m_pSharedMtrl->Clone();
	}

	m_pCurrentMtrl = m_pDynamicMtrl;

	return m_pCurrentMtrl;
}

void CRenderComponent::SaveToLevelFile(FILE* _pFile)
{
	SaveResRef(m_pMesh.Get(), _pFile);
	SaveResRef(m_pSharedMtrl.Get(), _pFile);

	bool hasDynamic = nullptr != m_pDynamicMtrl;
	fwrite(&hasDynamic, sizeof(hasDynamic), 1, _pFile);

	if (hasDynamic)
	{
		m_pDynamicMtrl->SaveDynamicMaterial(_pFile);
	}
}

void CRenderComponent::LoadFromLevelFile(FILE* _pFile)
{
	LoadResRef(m_pMesh, _pFile);
	LoadResRef(m_pSharedMtrl, _pFile);

	bool hasDynamic;
	fread(&hasDynamic, sizeof(hasDynamic), 1, _pFile);

	SetMaterial(m_pSharedMtrl);
	if (hasDynamic)
	{
		m_pDynamicMtrl = m_pSharedMtrl->Clone();
		m_pDynamicMtrl->LoadDynamicMaterial(_pFile);
		m_pCurrentMtrl = m_pDynamicMtrl;
	}
}

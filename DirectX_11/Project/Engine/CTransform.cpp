#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"

#include "CConstBuffer.h"

CTransform::CTransform() :
	CComponent(COMPONENT_TYPE::TRANSFORM),
	m_vRelativeScale(Vec3(1.f, 1.f, 1.f)),
	m_bAbsolute(false),
	m_vRelatveDir
	{ 
		Vec3(1.f, 0.f, 0.f), 
		Vec3{0.f, 1.f, 0.f}, 
		Vec3{0.f, 0.f, 1.f} 
	}
{
	SetName(L"Transform");
}

CTransform::~CTransform()
{
}

void CTransform::finaltick()
{
	m_matWorldScale = XMMatrixIdentity();
	m_matWorldScale = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);

	Matrix matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationX(m_vRelativeRot.x);
	matRot *= XMMatrixRotationY(m_vRelativeRot.y);
	matRot *= XMMatrixRotationZ(m_vRelativeRot.z);

	Matrix matTranslation = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);

	m_matWorld = m_matWorldScale * matRot * matTranslation;
	
	// ���⺤�� ȸ��
	Vec3 vDefaultDir[3] =
	{
		Vec3(1.f, 0.f, 0.f),
		Vec3{0.f, 1.f, 0.f},
		Vec3{0.f, 0.f, 1.f}
	};

	for (int i = 0; i < 3; i++)
	{
		//XMVector3TransformCoord(vDefaultDir[i], matRot);	// ��ǥ ���� ��ȯ
		m_vRelatveDir[i] = XMVector3TransformNormal(vDefaultDir[i], matRot);	// ���� ���� ��ȯ
		m_vWorldDir[i] = m_vRelatveDir[i].Normalize();
	}

	// �θ� Ȯ��
	CGameObject* pPlayer = GetOwner()->GetParent();
	if (pPlayer)
	{
		if (m_bAbsolute)
		{
			Matrix matParentWorld = pPlayer->Transform()->m_matWorld;
			Matrix matParentScale = pPlayer->Transform()->m_matWorldScale;
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, matParentScale);

			// ����  = ���� ���� * �θ� �� ��� ��� * �θ����
			m_matWorld = m_matWorld* matParentScaleInv* matParentWorld;
		}
		else
		{
			m_matWorldScale *= pPlayer->Transform()->m_matWorldScale;
			m_matWorld *= pPlayer->Transform()->m_matWorld;
		}

		for (int i = 0; i < 3; i++)
		{
			m_vWorldDir[i] = XMVector3TransformNormal(vDefaultDir[i], m_matWorld);
			m_vWorldDir[i].Normalize();
		}
	}

	m_vPrevPos = m_vRelativePos;
}

void CTransform::UpdateData()
{
	CConstBuffer* pTransformBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);
	g_transform.matWorld = m_matWorld;
	g_transform.matWV = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP = g_transform.matWV * g_transform.matProj;

	pTransformBuffer->SetData(&g_transform, sizeof(tTransform));
	pTransformBuffer->UpdateData();
}

void CTransform::SaveToLevelFile(FILE* _pFile)
{
	fwrite(&m_vRelativePos, sizeof(m_vRelativePos), 1, _pFile);
	fwrite(&m_vRelativeScale, sizeof(m_vRelativeScale), 1, _pFile);
	fwrite(&m_vRelativeRot, sizeof(m_vRelativeRot), 1, _pFile);
	fwrite(&m_bAbsolute, sizeof(m_bAbsolute), 1, _pFile);
}

void CTransform::LoadFromLevelFile(FILE* _pFile)
{
	fread(&m_vRelativePos, sizeof(m_vRelativePos), 1, _pFile);
	fread(&m_vRelativeScale, sizeof(m_vRelativeScale), 1, _pFile);
	fread(&m_vRelativeRot, sizeof(m_vRelativeRot), 1, _pFile);
	fread(&m_bAbsolute, sizeof(m_bAbsolute), 1, _pFile);
}

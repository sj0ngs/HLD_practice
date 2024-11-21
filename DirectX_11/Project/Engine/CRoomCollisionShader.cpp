#include "pch.h"
#include "CRoomCollisionShader.h"
#include "CStructuredBuffer.h"

CRoomCollisionShader::CRoomCollisionShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup)	:
	CComputeShader(_iThreadXPerGroup, _iThreadYPerGroup, _iThreadZPerGroup),
	m_pCollisionBuffer(nullptr),
	m_fObjSize(0.f)
{

}

CRoomCollisionShader::~CRoomCollisionShader()
{
}

void CRoomCollisionShader::UpdateData()
{
	if (nullptr == m_pLayer1Tex || nullptr == m_pLayer2Tex)
		assert(nullptr);

	m_pCollisionBuffer->UpdateData_CS(0, false);
	m_pLayer1Tex->UpdateDate_CS(0, true);
	m_pLayer2Tex->UpdateDate_CS(1, true);

	// �״�� �ѱ��� ���� gpu ���� �����ϱ� ���ϰ� �ѹ� ó���ؼ� �����°� ���ƺ���
	// �ϴ� uv �� �����ϱ� ���� ���� �»���� �������� �ϴ� ��ǥ�� ����
	Vec2 vResol = Vec2((float)m_pLayer1Tex->Width(), (float)m_pLayer1Tex->Height());
	Vec2 vSizeValue = vResol / m_vRoomSize;
	Vec2 vRoomObjPos = m_vObjPos * vSizeValue;
	vRoomObjPos -= Vec2(vResol.x / -2.f, vResol.y / 2.f);
	vRoomObjPos.y *= -1.f;

	m_Const.arrV2[0] = vResol;
	m_Const.arrV2[1] = m_vRoomSize;
	m_Const.arrV2[2] = vRoomObjPos;
	m_Const.arrFloat[0] = m_fObjSize * vSizeValue.x;

	// �׷� ��
	m_iGroupX = (UINT)m_pLayer1Tex->Width() / m_iThreadXPerGroup;
	m_iGroupY = (UINT)m_pLayer1Tex->Height() / m_iThreadYPerGroup;
	m_iGroupZ = 1;

	if (0 < (UINT)m_pLayer1Tex->Width() % m_iThreadXPerGroup)
		m_iGroupX++;

	if (0 < (UINT)m_pLayer1Tex->Height() % m_iThreadYPerGroup)
		m_iGroupY++;
}

void CRoomCollisionShader::Clear()
{
	m_pCollisionBuffer->Clear_CS(false);
	m_pLayer1Tex->Clear_CS(true);
	m_pLayer2Tex->Clear_CS(true);
}


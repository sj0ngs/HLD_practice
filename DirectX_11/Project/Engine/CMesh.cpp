#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"

CMesh::CMesh(bool _bEngine)	:
	CRes(RES_TYPE::MESH, _bEngine),
	m_tVBDesc{},
	m_VtxCount(0),
	m_tIBDesc{},
	m_IdxCount(0),
	m_pVtxSys(nullptr),
	m_pIdxSys(nullptr)
{
}

CMesh::~CMesh()
{
	SAFE_DEL(m_pVtxSys);

	SAFE_DEL(m_pIdxSys);
}

void CMesh::Create(void* _VtxSystem, UINT _iVtxCount, void* _IdxSystem, UINT _iIdxCount)
{
	m_VtxCount = _iVtxCount;
	m_IdxCount = _iIdxCount;
	
	// SystemMem 데이터 복사
	m_pVtxSys = new Vtx[m_VtxCount];
	memcpy(m_pVtxSys, _VtxSystem, sizeof(Vtx) * m_VtxCount);

	m_pIdxSys = new UINT[m_IdxCount];
	memcpy(m_pIdxSys, _IdxSystem, sizeof(UINT) * m_IdxCount);

	// Vertex 버퍼 생성
	m_tVBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	m_tVBDesc.CPUAccessFlags = 0;
	m_tVBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tVBDesc.ByteWidth = sizeof(Vtx) * m_VtxCount;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = m_pVtxSys;
	if (FAILED(DEVICE->CreateBuffer(&m_tVBDesc, &tSub, m_VB.GetAddressOf())))
		assert(true);

	// Index 버퍼 생성
	m_tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	m_tIBDesc.CPUAccessFlags = 0;
	m_tIBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tIBDesc.ByteWidth = sizeof(UINT) * m_IdxCount;

	tSub.pSysMem = m_pIdxSys;
	if (FAILED(DEVICE->CreateBuffer(&m_tIBDesc, &tSub, m_IB.GetAddressOf())))
	{
		assert(true);
	}
}

void CMesh::UpdateData()
{
	UINT iStride = sizeof(Vtx);
	UINT iOffset = 0;

	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
	CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::render()
{
	UpdateData();

	CONTEXT->DrawIndexed(m_IdxCount, 0, 0);
}

void CMesh::render_particle(UINT _iParticleCount)
{
	UpdateData();

	// 인스턴싱
	CONTEXT->DrawIndexedInstanced(m_IdxCount, _iParticleCount, 0, 0, 0);
}

#include "pch.h"
#include "CConstBuffer.h"

#include "CDevice.h"

CConstBuffer::CConstBuffer(UINT _iResisterNum)	:
	m_Desc{},
	m_iResisterNum(_iResisterNum),
	m_iElementSize(0),
	m_iElementCount(0)
{
}

CConstBuffer::~CConstBuffer()
{
}

void CConstBuffer::Create(UINT _iElementSize, UINT _iElementCount)
{
	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCount;

	UINT iBufferSize = m_iElementSize * m_iElementCount;

	// 16 바이트 단위 메모리 정렬
	assert(!(iBufferSize % 16));

	m_Desc.ByteWidth = iBufferSize;
	m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&m_Desc, nullptr, m_CB.GetAddressOf())))
	{
		assert(false);
	}
}

void CConstBuffer::SetData(void* _pSrc, UINT _iSize)
{
	// 크기를 지정하지 않은 데이터는 상수 버퍼 크기로 본다
	UINT size = _iSize;
	if (0 == size)
		size = m_iElementSize * m_iElementCount;

	// 상수 버퍼 크기보다 더 큰 데이터가 입력으로 들어온 경우
	assert(!(size > m_iElementSize* m_iElementCount));

	// SysMem -> GPU Mem
	D3D11_MAPPED_SUBRESOURCE tSubRes = {};
	if (!FAILED(CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tSubRes)))
	{
		memcpy(tSubRes.pData, _pSrc, size);
		CONTEXT->Unmap(m_CB.Get(), 0);
	}
}

void CConstBuffer::UpdateData()
{
	CONTEXT->VSSetConstantBuffers(m_iResisterNum, 1, m_CB.GetAddressOf());
	CONTEXT->HSSetConstantBuffers(m_iResisterNum, 1, m_CB.GetAddressOf());
	CONTEXT->DSSetConstantBuffers(m_iResisterNum, 1, m_CB.GetAddressOf());
	CONTEXT->GSSetConstantBuffers(m_iResisterNum, 1, m_CB.GetAddressOf());
	CONTEXT->PSSetConstantBuffers(m_iResisterNum, 1, m_CB.GetAddressOf());
}

void CConstBuffer::UpdateData_CS()
{
	CONTEXT->CSSetConstantBuffers(m_iResisterNum, 1, m_CB.GetAddressOf());
}

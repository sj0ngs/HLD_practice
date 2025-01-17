#include "pch.h"
#include "CStructuredBuffer.h"

#include "CDevice.h"

CStructuredBuffer::CStructuredBuffer() :
	m_iElementSize(0),
	m_iElementCount(0),
	m_tDesc{},
	m_iRecentRegisterNum(0)
{
}

CStructuredBuffer::~CStructuredBuffer()
{
}

void CStructuredBuffer::Create(UINT _iElementSize, UINT _iElementCount, 
						SB_TYPE _Type, bool _bUseSysAccess, void* _pSysMem)
{
	m_SB = nullptr;
	m_SRV = nullptr;
	m_UAV = nullptr;

	m_SB_CPU_Read = nullptr;
	m_SB_CPU_Write = nullptr;

	m_Type = _Type;
	m_bSysAccess = _bUseSysAccess;

	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCount;

	UINT iBufferSize = m_iElementSize * m_iElementCount;

	// 16 바이트 단위 메모리 정렬
	assert(!(iBufferSize % 16));

	m_tDesc.ByteWidth = iBufferSize;
	m_tDesc.StructureByteStride = m_iElementSize;

	if (SB_TYPE::READ_ONLY == m_Type)
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	}

	m_tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	m_tDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tDesc.CPUAccessFlags = 0;

	// 초기 데이터 세팅
	if (nullptr == _pSysMem)
	{
		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_SB.GetAddressOf())))
		{
			assert(nullptr);
		}
	}
	else
	{
		D3D11_SUBRESOURCE_DATA tSubData = {};
		tSubData.pSysMem = _pSysMem;

		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, &tSubData, m_SB.GetAddressOf())))
		{
			assert(nullptr);
		}
	}

	// SRV 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

	tSRVDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFEREX;
	tSRVDesc.BufferEx.NumElements = m_iElementCount;

	if (FAILED(DEVICE->CreateShaderResourceView(m_SB.Get(), &tSRVDesc, m_SRV.GetAddressOf())))
	{
		assert(nullptr);
	}

	// UAV 생성
	if (SB_TYPE::READ_WRITE == m_Type)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};

		tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		tUAVDesc.Buffer.NumElements = m_iElementCount;

		if (FAILED(DEVICE->CreateUnorderedAccessView(m_SB.Get(), &tUAVDesc, m_UAV.GetAddressOf())))
		{
			assert(nullptr);
		}
	}

	// CPU Access 보조 버퍼
	if (m_bSysAccess)
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		// GPU -> CPU Read
		m_tDesc.Usage = D3D11_USAGE_DEFAULT;
		m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;

		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_SB_CPU_Read.GetAddressOf())))
		{
			assert(nullptr);
		}

		// CPU -> GPU Write
		m_tDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_SB_CPU_Write.GetAddressOf())))
		{
			assert(nullptr);
		}
	}
}

void CStructuredBuffer::SetData(void* _pSrc, UINT _iSize)
{
	if (nullptr == _pSrc)
		return;

	UINT iSize = _iSize;
	if (0 == _iSize)
	{
		iSize = GetBufferSize();
	}

	// CPU -> CPU WriteBuffer
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_SB_CPU_Write.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, _pSrc, iSize);
	CONTEXT->Unmap(m_SB_CPU_Write.Get(), 0);

	// CPU Write buffer -> main buffer
	CONTEXT->CopyResource(m_SB.Get(), m_SB_CPU_Write.Get());
}

void CStructuredBuffer::GetData(void* _pDest)
{
	// main buffer -> CPU Read buffer
	CONTEXT->CopyResource(m_SB_CPU_Read.Get(), m_SB.Get());

	// CPU ReadBuffer -> CPU
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_SB_CPU_Read.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &tSub);
	memcpy(_pDest, tSub.pData, GetBufferSize());
	CONTEXT->Unmap(m_SB_CPU_Read.Get(), 0);
}

void CStructuredBuffer::UpdateData(UINT _iRegisterNum, UINT _iPipeLineStage)
{
	m_iRecentRegisterNum = _iRegisterNum;

	if (PS_VERTEX & _iPipeLineStage)
	{
		CONTEXT->VSSetShaderResources(m_iRecentRegisterNum, 1, m_SRV.GetAddressOf());
	}
	if (PS_HULL & _iPipeLineStage)
	{
		CONTEXT->HSSetShaderResources(m_iRecentRegisterNum, 1, m_SRV.GetAddressOf());
	}
	if (PS_DOMAIN & _iPipeLineStage)
	{
		CONTEXT->DSSetShaderResources(m_iRecentRegisterNum, 1, m_SRV.GetAddressOf());
	}
	if (PS_GEOMETRY & _iPipeLineStage)
	{
		CONTEXT->GSSetShaderResources(m_iRecentRegisterNum, 1, m_SRV.GetAddressOf());
	}
	if (PS_PIXEL & _iPipeLineStage)
	{
		CONTEXT->PSSetShaderResources(m_iRecentRegisterNum, 1, m_SRV.GetAddressOf());
	}
}

void CStructuredBuffer::UpdateData_CS(UINT _iRegisterNum, bool _bIsShaderRes)
{
	UINT i = -1;
	m_iRecentRegisterNum = _iRegisterNum;

	if (_bIsShaderRes)
	{
		CONTEXT->CSSetShaderResources(m_iRecentRegisterNum, 1, m_SRV.GetAddressOf());
	}
	else
	{
		CONTEXT->CSSetUnorderedAccessViews(m_iRecentRegisterNum, 1, m_UAV.GetAddressOf(), &i);
	}

}

void CStructuredBuffer::Clear()
{
	ID3D11ShaderResourceView* pSRV = nullptr;

	CONTEXT->VSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
}															   

void CStructuredBuffer::Clear_CS(bool _bIsShaderRes)
{
	if (_bIsShaderRes)
	{
		ID3D11ShaderResourceView* pSRV = nullptr;
		CONTEXT->CSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	}
	else
	{
		ID3D11UnorderedAccessView* pUAV = nullptr;
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(m_iRecentRegisterNum, 1, &pUAV, &i);
	}
}

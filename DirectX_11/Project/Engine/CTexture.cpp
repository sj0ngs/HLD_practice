#include "pch.h"
#include "CTexture.h"

#include "CDevice.h"

CTexture::CTexture(bool _bEngine)	:
	CRes(RES_TYPE::TEXTURE, _bEngine),
	m_Desc{}
{
}

CTexture::~CTexture()
{
}

void CTexture::UpdateData()
{
}

void CTexture::UpdateData(int _iRegisterNum, int _PipelineStage)
{
	m_iRecentNum = _iRegisterNum;

	if (PS_VERTEX & _PipelineStage)
	{
		CONTEXT->VSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	if (PS_HULL & _PipelineStage)
	{
		CONTEXT->HSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	if (PS_DOMAIN & _PipelineStage)
	{
		CONTEXT->DSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	if (PS_GEOMETRY & _PipelineStage)
	{
		CONTEXT->GSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	if (PS_PIXEL & _PipelineStage)
	{
		CONTEXT->PSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
}

void CTexture::UpdateDate_CS(int _iRegisterNum, bool _bShaderRes)
{
	m_iRecentNum = _iRegisterNum;

	if (_bShaderRes)
	{
		CONTEXT->CSSetShaderResources(m_iRecentNum, 1, m_SRV.GetAddressOf());
	}
	else
	{
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(m_iRecentNum, 1, m_UAV.GetAddressOf(), &i);
	}
}

void CTexture::Clear()
{
	ID3D11ShaderResourceView* pSRV = nullptr;

	CONTEXT->VSSetShaderResources(m_iRecentNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(m_iRecentNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(m_iRecentNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(m_iRecentNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(m_iRecentNum, 1, &pSRV);
}

void CTexture::Clear(int _iRegisterNum)
{
	ID3D11ShaderResourceView* pSRV = nullptr;

	CONTEXT->VSSetShaderResources(_iRegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(_iRegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(_iRegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(_iRegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(_iRegisterNum, 1, &pSRV);
}

void CTexture::Clear_CS(bool _bShaderRes)
{

	if (_bShaderRes)
	{
		ID3D11ShaderResourceView* pSRV = nullptr;
		CONTEXT->CSSetShaderResources(m_iRecentNum, 1, &pSRV);
	}
	else
	{
		ID3D11UnorderedAccessView* pUAV = nullptr;
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(m_iRecentNum, 1, &pUAV, &i);
	}
}

int CTexture::Load(const wstring& _strFilePath)
{
	wchar_t szExt[50] = L"";
	_wsplitpath_s(_strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);
	wstring strExt = szExt;

	HRESULT hr = S_OK;
	if (L".dds" == strExt || L".DDS" == strExt)
	{
		hr = LoadFromDDSFile(_strFilePath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_Image);
	}
	else if (L".tga" == strExt || L".TGA" == strExt)
	{
		hr = LoadFromTGAFile(_strFilePath.c_str(), TGA_FLAGS::TGA_FLAGS_NONE, nullptr, m_Image);
	}
	else
	{
		hr = LoadFromWICFile(_strFilePath.c_str(), WIC_FLAGS::WIC_FLAGS_IGNORE_SRGB, nullptr, m_Image);
	}

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"리소스 로딩 실패", L"텍스쳐 로딩 실패", MB_OK);
		return E_FAIL;
	}

	hr = CreateShaderResourceView(DEVICE, 
		m_Image.GetImages(), 
		m_Image.GetImageCount(), 
		m_Image.GetMetadata(), 
		m_SRV.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"ShaderResourceView 생성 실패", L"텍스쳐 로딩 실패", MB_OK);
		return E_FAIL;
	}

	m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());
	m_Tex2D->GetDesc(&m_Desc);

	return S_OK;
}

int CTexture::Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage)
{
	// DepthStencil 용 텍스쳐 생성
	m_Desc = {};

	m_Desc.Format = _Format;

	// 렌더타겟과 반드시 같은 해상도록 설정해야 함
	m_Desc.Width = _iWidth;
	m_Desc.Height = _iHeight;
	m_Desc.ArraySize = 1;

	// 텍스쳐의 리소스 타입
	m_Desc.BindFlags = _BindFlag;
	m_Desc.Usage = _Usage;

	m_Desc.CPUAccessFlags = 0;
	if(D3D11_USAGE::D3D11_USAGE_DYNAMIC == _Usage)
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else if(D3D11_USAGE::D3D11_USAGE_STAGING == _Usage)
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	// 레벨을 올리면 올릴수록 절반 해상도의 저화질 버전을 같이 저장하게 된다
	// 물체의 거리에 따라 출력할 해상도를 변경시킨다
	m_Desc.MipLevels = 1;

	// 주변 픽셀말고 하나의 픽셀만 검사할 것이다
	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;

	// SubResource는 텍스쳐의 초기 데이터를 설정하는 부분이다
	if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
	{
		return E_FAIL;
	}

	if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL)
	{
		if (FAILED(DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf())))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE)
		{
			if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET)
		{
			if (FAILED(DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
		{
			if (FAILED(DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), nullptr, m_UAV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

int CTexture::Create(ComPtr<ID3D11Texture2D> _Tex2D)
{
	m_Tex2D = _Tex2D;
	_Tex2D->GetDesc(&m_Desc);

	if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL)
	{
		if (FAILED(DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf())))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE)
		{
			if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET)
		{
			if (FAILED(DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
		{
			if (FAILED(DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), nullptr, m_UAV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}
	}
	return S_OK;
}

int CTexture::Save(const wstring& _strRelativePath)
{
	return S_OK;
}
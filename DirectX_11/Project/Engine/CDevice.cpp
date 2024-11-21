#include "pch.h"
#include "CDevice.h"

#include "CEngine.h"

#include "CConstBuffer.h"

#include "CResMgr.h"

CDevice::CDevice()	:
	m_hWnd(nullptr),
	m_ViewPort{},
	m_vRenderResolution{},
	m_arrConstBuffer{},
	m_arrSampler{}
{
}

CDevice::~CDevice()
{
	Safe_Del_Array(m_arrConstBuffer);
}

int CDevice::init(HWND _hWnd, UINT _iWidth, UINT _iHeight)
{
	m_hWnd = _hWnd;
	m_vRenderResolution = Vec2((float)_iWidth, (float)_iHeight);

	GlobalData.Resolution = m_vRenderResolution;

	int iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL eLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

	if (FAILED(D3D11CreateDevice(nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, iFlag, 
		nullptr, 0, 
		D3D11_SDK_VERSION, 
		m_Device.GetAddressOf(), &eLevel,
		m_Context.GetAddressOf())))
	{
		MessageBox(nullptr, L"Device, Context 생성 실패", L"Device 초기화 에러", MB_OK);
		return E_FAIL;
	}

	if (FAILED(CreateSwapChain()))
	{
		MessageBox(nullptr, L"SwapChain 생성 실패", L"Device 초기화 에러", MB_OK);
		return E_FAIL;
	}

	if (FAILED(CreateView()))
	{
		MessageBox(nullptr, L"View 생성 실패", L"Device 초기화 에러", MB_OK);
		return E_FAIL;
	}

	// ViewPort 설정
	// 실제 윈도우에 출력될 영역을 지정하고 싶을 때 사용한다
	// 출력은 1 대 1로 되지만 뷰포트 영역에 맞춰서 그릴수 있는 공간이 자체가 변하게 된다
	SetViewPort();

	// Rasterizer 생성
	if (FAILED(CreateRasterizer()))
	{
		MessageBox(nullptr, L"Rasterizer 생성 실패", L"Device 초기화 에러", MB_OK);
		return E_FAIL;
	}

	// DepthStencilState 생성
	if (FAILED(CreateDepthStencilState()))
	{
		MessageBox(nullptr, L"DepthStencil State 생성 실패", L"Device 초기화 에러", MB_OK);
		return E_FAIL;
	}

	// BlendState 생성
	if (FAILED(CreateBlendState()))
	{
		MessageBox(nullptr, L"Blend State 생성 실패", L"Device 초기화 에러", MB_OK);
		return E_FAIL;
	}

	// 상수 버퍼 생성
	CreateConstBuffer();

	// 셈플러 생성
	if (FAILED(CreateSampler()))
	{
		MessageBox(nullptr, L"Sampler 생성 실패", L"Device 초기화 에러", MB_OK);
		return E_FAIL;
	}
	return S_OK; // E_FAIL;
}

int CDevice::CreateSwapChain()
{
	// DXGI 가 붙으면 DX 버전 상관없이 사용한다

	// 스왑체인 설정
	DXGI_SWAP_CHAIN_DESC tDesc = {};

	tDesc.OutputWindow = m_hWnd;	// 출력 할 윈도우
	tDesc.Windowed = true;			// 창모드 설정

	// 후면버퍼의 개수 
	tDesc.BufferCount = 1;	
	// 버퍼 용도 : 현재는 출력 용도이므로 출력 옵션을 잡아준다
	tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; 

	tDesc.BufferDesc.Width = (UINT)m_vRenderResolution.x;
	tDesc.BufferDesc.Height = (UINT)m_vRenderResolution.y;
	// 쉐이더에서 비율 값으로 정규화하여 사용하기 위한 포맷
	tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 화면 갱신률을 1/60으로 잡아준다
	tDesc.BufferDesc.RefreshRate.Denominator = 1;
	tDesc.BufferDesc.RefreshRate.Numerator = 60;
	tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
	// 버퍼가 바뀔때마다 버퍼의 그려진 것을 버리겠다는 옵션
	tDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;

	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Flags = 0;

	// 스왑체인 생성
	ComPtr<IDXGIDevice> pDXGIDevice = nullptr;
	ComPtr<IDXGIAdapter> pAdaptor = nullptr;
	ComPtr<IDXGIFactory> pFactory = nullptr;

	HRESULT hr = S_OK;

	hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf());
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdaptor.GetAddressOf());
	hr = pAdaptor->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf());
	hr = pFactory->CreateSwapChain(m_Device.Get(), &tDesc, m_SwapChain.GetAddressOf());

	return hr;
}

int CDevice::CreateView()
{
	ComPtr<ID3D11Texture2D> Tex2D;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)Tex2D.GetAddressOf());

	m_RTTex = CResMgr::GetInst()->CreateTexture(L"RenderTargetTex", Tex2D);

	// DepthStencil View 생성
	m_DSTex = CResMgr::GetInst()->CreateTexture(L"DepthStencilTex", (UINT)m_vRenderResolution.x, (UINT)m_vRenderResolution.y,
		DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);

	return S_OK;
}

void CDevice::SetViewPort()
{
	// 실제 윈도우에 출력될 영역을 지정하고 싶을 때 사용한다
	// 출력은 1 대 1로 되지만 뷰포트 영역에 맞춰서 그릴수 있는 공간 자체가 변하게 된다
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;

	m_ViewPort.Width = m_vRenderResolution.x;
	m_ViewPort.Height = m_vRenderResolution.y;

	m_ViewPort.MinDepth = 0.f;
	m_ViewPort.MaxDepth = 1.f;

	m_Context->RSSetViewports(1, &m_ViewPort);
}

void CDevice::SetViewPort(UINT _iWidth, UINT _iHeight)
{
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;

	m_ViewPort.Width = (float)_iWidth;
	m_ViewPort.Height = (float)_iHeight;

	m_ViewPort.MinDepth = 0.f;
	m_ViewPort.MaxDepth = 1.f;

	m_Context->RSSetViewports(1, &m_ViewPort);
}

int CDevice::CreateRasterizer()
{
	m_RSState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

	HRESULT hr = S_OK;

	D3D11_RASTERIZER_DESC Desc = {};

	Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
	Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	hr =  DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());

	Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	hr =  DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

	Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	hr = DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());

	return hr;
}

int CDevice::CreateDepthStencilState()
{
	m_DSState[(UINT)DS_TYPE::LESS] = nullptr;

	HRESULT hr = S_OK;

	D3D11_DEPTH_STENCIL_DESC Desc = {};

	// Less Eqaul
	Desc.DepthEnable = true;
	Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	Desc.StencilEnable = false;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	hr =  DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());

	// Greater
	Desc.DepthEnable = true;
	Desc.DepthFunc = D3D11_COMPARISON_GREATER;
	Desc.StencilEnable = false;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	hr = DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::GREATER].GetAddressOf());

	// Greater Eqaul
	Desc.DepthEnable = true;
	Desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	Desc.StencilEnable = false;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	hr = DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::GREATER_EQUAL].GetAddressOf());

	// No Write
	Desc.DepthEnable = true;
	Desc.DepthFunc = D3D11_COMPARISON_LESS;
	Desc.StencilEnable = false;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_WRITE].GetAddressOf());

	// No Test No Write
	Desc.DepthEnable = false;
	Desc.StencilEnable = false;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf());

	return hr;
}

int CDevice::CreateBlendState()
{
	// No Blend
	m_BSState[(UINT)BS_TYPE::DEFAULT] = nullptr;

	D3D11_BLEND_DESC Desc = {};

	HRESULT hr = S_OK;

	// Mask
	Desc.AlphaToCoverageEnable = true;
	Desc.IndependentBlendEnable = false;

	Desc.RenderTarget[0].BlendEnable = true;
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;

	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::MASK].GetAddressOf());

	// Alpha Blend
	Desc.AlphaToCoverageEnable = false;
	Desc.IndependentBlendEnable = false;

	Desc.RenderTarget[0].BlendEnable = true;
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;		
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	

	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ALPHA_BLEND].GetAddressOf());

	// One One
	Desc.AlphaToCoverageEnable = false;
	Desc.IndependentBlendEnable = false;

	Desc.RenderTarget[0].BlendEnable = true;
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE	;	
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;	

	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ONE_ONE].GetAddressOf());

	return hr;
}

void CDevice::CreateConstBuffer()
{
	m_arrConstBuffer[(UINT)CB_TYPE::TRANSFORM] = new CConstBuffer((UINT)CB_TYPE::TRANSFORM);
	m_arrConstBuffer[(UINT)CB_TYPE::TRANSFORM]->Create(sizeof(tTransform), 1);

	m_arrConstBuffer[(UINT)CB_TYPE::MATERIAL] = new CConstBuffer((UINT)CB_TYPE::MATERIAL);
	m_arrConstBuffer[(UINT)CB_TYPE::MATERIAL]->Create(sizeof(tMtrlConst), 1);

	m_arrConstBuffer[(UINT)CB_TYPE::GLOBAL] = new CConstBuffer((UINT)CB_TYPE::GLOBAL);
	m_arrConstBuffer[(UINT)CB_TYPE::GLOBAL]->Create(sizeof(tGlobal), 1);
}

int CDevice::CreateSampler()
{
	D3D11_SAMPLER_DESC tSamDesc = {};

	tSamDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	DEVICE->CreateSamplerState(&tSamDesc, m_arrSampler[0].GetAddressOf());

	tSamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	if (FAILED(DEVICE->CreateSamplerState(&tSamDesc, m_arrSampler[1].GetAddressOf())))
	{
		return E_FAIL;
	}

	CONTEXT->VSSetSamplers(0, 1, m_arrSampler[0].GetAddressOf());
	CONTEXT->HSSetSamplers(0, 1, m_arrSampler[0].GetAddressOf());
	CONTEXT->DSSetSamplers(0, 1, m_arrSampler[0].GetAddressOf());
	CONTEXT->GSSetSamplers(0, 1, m_arrSampler[0].GetAddressOf());
	CONTEXT->PSSetSamplers(0, 1, m_arrSampler[0].GetAddressOf());

	CONTEXT->VSSetSamplers(1, 1, m_arrSampler[1].GetAddressOf());
	CONTEXT->HSSetSamplers(1, 1, m_arrSampler[1].GetAddressOf());
	CONTEXT->DSSetSamplers(1, 1, m_arrSampler[1].GetAddressOf());
	CONTEXT->GSSetSamplers(1, 1, m_arrSampler[1].GetAddressOf());
	CONTEXT->PSSetSamplers(1, 1, m_arrSampler[1].GetAddressOf());

	return S_OK;
}

// 특정 색상으로 랜더타겟을 칠해준다
void CDevice::ClearTarget(float(&_color)[4])
{
	m_Context->ClearRenderTargetView(m_RTTex->GetRTV().Get(), _color);
	m_Context->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

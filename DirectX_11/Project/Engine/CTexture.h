#pragma once
#include "CRes.h"

#include <DirectXTex\DirectXTex.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex//DirectXTex_debug")
#else
#pragma comment(lib, "DirectXTex//DirectXTex")
#endif // 


class CTexture :
    public CRes
{
private:
    ComPtr<ID3D11Texture2D>             m_Tex2D;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;
    ComPtr<ID3D11RenderTargetView>      m_RTV;
    ComPtr<ID3D11DepthStencilView>      m_DSV;
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;

    D3D11_TEXTURE2D_DESC                m_Desc;

    ScratchImage                        m_Image;

    UINT                                m_iRecentNum;
   
public:
    const D3D11_TEXTURE2D_DESC& GetDesc() const { return m_Desc; }

    ComPtr<ID3D11ShaderResourceView> GetSRV() const { return m_SRV; }
    ComPtr<ID3D11RenderTargetView> GetRTV() const { return m_RTV; }
    ComPtr<ID3D11DepthStencilView> GetDSV() const { return m_DSV; }
    ComPtr<ID3D11UnorderedAccessView> GetUAV() const { return m_UAV; }
    ComPtr<ID3D11Texture2D> GetTex2D() const { return m_Tex2D; }

    int Width() const { return m_Desc.Width; }
    int Height() const { return m_Desc.Height; }

    const ScratchImage& GetImage() { return m_Image; }

private:
    virtual void UpdateData();

public:
    // _PipelineStage : PIPELINE_STAGE
    void UpdateData(int _iRegisterNum, int _PipelineStage);

    void UpdateDate_CS(int _iRegisterNum, bool _bShaderRes);

    void Clear();
    static void Clear(int _iRegisterNum);
    void Clear_CS(bool _bShaderRes);

private:
    virtual int Load(const wstring& _strFilePath);

public:
    int Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage);
    int Create(ComPtr<ID3D11Texture2D> _Tex2D);
    virtual int Save(const wstring& _strRelativePath);

public:
    CTexture(bool _bEngine = false);
    ~CTexture();
};


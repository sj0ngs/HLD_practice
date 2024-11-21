#pragma once
#include "CShader.h"

struct tScalarParam
{
    SCALAR_PARAM    eParam;
    string         strDesc;
};

struct tTexParam
{
    TEX_PARAM   eParam;
    string      strDesc;
};

class CGraphicsShader :
    public CShader
{
private:
    ComPtr<ID3DBlob>                m_VSBlob;
    ComPtr<ID3DBlob>                m_HSBlob;
    ComPtr<ID3DBlob>                m_DSBlob;
    ComPtr<ID3DBlob>                m_GSBlob;
    ComPtr<ID3DBlob>                m_PSBlob;
    
    ComPtr<ID3D11VertexShader>      m_VS;
    ComPtr<ID3D11HullShader>        m_HS;
    ComPtr<ID3D11DomainShader>      m_DS;
    ComPtr<ID3D11GeometryShader>    m_GS;
    ComPtr<ID3D11PixelShader>       m_PS;

    ComPtr<ID3D11InputLayout>       m_Layout;
    D3D11_PRIMITIVE_TOPOLOGY        m_Topoloy;

    RS_TYPE                         m_RSType;
    DS_TYPE                         m_DSType;
    BS_TYPE                         m_BSType;

    SHADER_DOMAIN                   m_Domain;

    vector<tScalarParam>            m_vecScalarParam;
    vector<tTexParam>               m_vecTexParam;

public:
    void CreateVertexShader(const wstring& _strFileName, const string& _strFuncName);
    void CreatePixelShader(const wstring& _strFileName, const string& _strFuncName);
    void CreateGeometryShader(const wstring& _strFileName, const string& _strFuncName);
    
    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _Topology) { m_Topoloy = _Topology; }
    D3D11_PRIMITIVE_TOPOLOGY GetTopology() const { return m_Topoloy; }

    void SetRSType(RS_TYPE _Type) { m_RSType = _Type; }
    RS_TYPE GetRSType() const { return m_RSType; }

    void SetDSType(DS_TYPE _Type) { m_DSType = _Type; }
    DS_TYPE GetDSType() const { return m_DSType; }

    void SetBSType(BS_TYPE _Type) { m_BSType = _Type; }
    BS_TYPE GetBSType() const { return m_BSType; }

    void SetDomain(SHADER_DOMAIN _Domain) { m_Domain = _Domain; }
    SHADER_DOMAIN GetDomain() const { return m_Domain; }

    void AddScalarParam(SCALAR_PARAM _Param, const string& _Desc) { m_vecScalarParam.push_back(tScalarParam{ _Param, _Desc }); }
    const vector<tScalarParam>& GetScalarParam() const { return m_vecScalarParam; }

    void AddTexParam(TEX_PARAM _Param, const string& _Desc) { m_vecTexParam.push_back(tTexParam{ _Param, _Desc }); }
    const vector<tTexParam>& GetTexParam() const { return m_vecTexParam; }

public:
    virtual void UpdateData() override;

public:
    CGraphicsShader();
    ~CGraphicsShader();
};


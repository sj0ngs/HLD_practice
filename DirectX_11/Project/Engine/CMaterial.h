#pragma once
#include "CRes.h"

#include "ptr.h"
#include "CGraphicsShader.h"
#include "CTexture.h"

class CMaterial :
    public CRes
{
private:
    Ptr<CGraphicsShader>    m_pShader;

    tMtrlConst              m_Const;
    Ptr<CTexture>           m_arrTex[TEX_END];

public:
    void SetScalarParam(SCALAR_PARAM _Param, const void* _Src);
    void GetScalarParam(SCALAR_PARAM _Param, void* _pData);

    void SetTexParam(TEX_PARAM _Param, const Ptr<CTexture>& _Tex);
    Ptr<CTexture> GetTexParam(TEX_PARAM _Param) const { return m_arrTex[_Param]; }

    void SetShader(Ptr<CGraphicsShader> _pShader) { assert(_pShader.Get()); m_pShader = _pShader; }

    Ptr<CGraphicsShader> GetShader() const { return m_pShader; }

public:
    virtual void UpdateData() override;

    CLONE(CMaterial);
private:
    virtual int Load(const wstring& _strFilePath) override;

public:
    virtual int Save(const wstring& _strRelativeFilePath) override;
    void SaveToLevelFile(FILE* _pFile);
    void LoadFromLevelFile(FILE* _pFile);

public:
    void SaveDynamicMaterial(FILE* _pFile);
    void LoadDynamicMaterial(FILE* _pFile);

public:
    CMaterial(bool _bEngine = false);
    ~CMaterial();
};


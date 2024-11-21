#pragma once
#include "CShader.h"

class CComputeShader :
    public CShader
{
private:
    ComPtr<ID3D11ComputeShader>     m_CS;
    ComPtr<ID3DBlob>                m_CSBlob;

protected:
    tMtrlConst                      m_Const;

    UINT                            m_iGroupX;
    UINT                            m_iGroupY;
    UINT                            m_iGroupZ;

    UINT                            m_iThreadXPerGroup;
    UINT                            m_iThreadYPerGroup;
    UINT                            m_iThreadZPerGroup;

public:
    void CreateComputeShader(const wstring& _strFileName, const string& _strFuncName);
    void Execute();

    Vec3 GetThreadGroup() const { return Vec3((float)m_iGroupX, (float)m_iGroupY, (float)m_iGroupZ); }
    Vec3 GetThreadPerGroup() const { return Vec3((float)m_iThreadXPerGroup, (float)m_iThreadYPerGroup, (float)m_iThreadZPerGroup); }

private:
    virtual void UpdateData() = 0;
    virtual void Clear() = 0;

    CLONE_DISABLE(CComputeShader);
public:
    CComputeShader(UINT _iThreadXPerGroup, UINT _iThreadYPerGroup, UINT _iThreadZPerGroup);
    ~CComputeShader();
};


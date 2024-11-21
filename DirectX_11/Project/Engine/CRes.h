#pragma once
#include "CEntity.h"

#include "CPathMgr.h"

class CRes :
    public CEntity
{
private:
    const RES_TYPE  m_Type;
    int             m_iRefCount;

    wstring         m_strKey;
    wstring         m_strRelativePath;

    bool            m_bEngine;

private:

    void SetKey(const wstring& _strKey) { m_strKey = _strKey; }
    void SetRelativePath(const wstring& _strPath) { m_strRelativePath = _strPath; }

    void AddRef() { ++m_iRefCount; }
    void Release();


public:
    const RES_TYPE GetType() const { return m_Type; }
    const wstring& GetKey() { return m_strKey; }
    const wstring& GetRelativePath() { return m_strRelativePath; };
    bool IsEngineRes() const { return m_bEngine; }

public:
    virtual void UpdateData() = 0;

private:
    virtual int Load(const wstring& _strFilePath) = 0;

public:
    virtual int Save(const wstring& _strRelativeFilePath) = 0;

    virtual CRes* Clone() { return nullptr; assert(nullptr); }

public:
    CRes(RES_TYPE _type, bool _bEngine = false);
    CRes(const CRes& _Other);
    virtual ~CRes();

    friend class CResMgr;

    template<typename T>
    friend class Ptr;
};


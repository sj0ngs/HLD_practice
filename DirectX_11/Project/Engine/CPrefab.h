#pragma once
#include "CRes.h"

class CGameObject;

class CPrefab :
    public CRes
{
private:
    CGameObject*    m_ProtoObj;

private:
public:
    virtual int Load(const wstring& _strPath);
    virtual int Save(const wstring& _strRelativePath);

private:
    int SaveGameObject(CGameObject* _pObj, FILE* _pFile);
    CGameObject* LoadGameObject(FILE* _pFile);

private:
    virtual void UpdateData() override;

public:
    CGameObject* GetProtoObj() const { return m_ProtoObj; }
    void RegisterProtoObject(CGameObject* _Proto);
    CGameObject* Instantiate();

public:
    CPrefab(bool _bIsEngine = false);
    ~CPrefab();
};


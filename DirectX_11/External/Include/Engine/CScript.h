#pragma once
#include "CComponent.h"

#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CResMgr.h"
#include "CLevelMgr.h"

#include "CFontMgr.h"

#include "components.h"

#include "CLevel.h"
#include "CLayer.h"

#include "CGameObject.h"

enum class SCRIPT_PARAM
{
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    TEX
};

// ========
// Delegate
// ========
typedef void (CScript::*SCRIPT_DELEGATE)(void);
typedef void (CScript::*SCRIPT_DELEGATE_1)(DWORD_PTR);
typedef void (CScript::*SCRIPT_DELEGATE_2)(DWORD_PTR, DWORD_PTR);
typedef void (CScript::*SCRIPT_DELEGATE_3)(DWORD_PTR, DWORD_PTR, DWORD_PTR);

struct tScriptParam
{
    void*           pData;
    SCRIPT_PARAM    eParam;
    string          strDesc;
};

class CScript :
    public CComponent
{
protected:
    UINT                    m_iScriptType;
    vector<tScriptParam>    m_vecParam;

public:
    UINT GetScriptType() const { return m_iScriptType; }
    const vector<tScriptParam>& GetScriptParam() const { return m_vecParam; }

public:
    virtual void init() {};
    virtual void end() {};
    virtual void finaltick() final {};

    virtual void BeginOverlap(CCollider2D* _Other) {};
    virtual void OnOverlap(CCollider2D* _Other) {};
    virtual void EndOverlap(CCollider2D* _Other) {};

public:
    virtual void SetScriptTex(const string& _strDesc, const Ptr<CTexture>& _pTex) {};
    virtual void SaveToLevelFile(FILE* _pFile);
    virtual void LoadFromLevelFile(FILE* _pFile);

protected:
    void AddScriptParam(void* _pData, SCRIPT_PARAM eParam, const string& _strDesc);

public:
    CScript(UINT _iScriptType);
    ~CScript();

public:
    void Destory() { DestoryObject(GetOwner()); }
    void SetLifeSpan(float _ftime) { GetOwner()->SetLifeSpan(_ftime); }
};


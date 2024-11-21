#pragma once
#include "CSingleton.h"

#include "ptr.h"
#include "CMesh.h"
#include "CTexture.h"
#include "CGraphicsShader.h"
#include "CComputeShader.h"
#include "CMaterial.h"
#include "CPrefab.h"
#include "CSound.h"

class CPathMgr;

class CResMgr :
    public CSingleton<CResMgr>
{
    SINGLE(CResMgr);
private:
    map<wstring, Ptr<CRes>> m_arrRes[(UINT)RES_TYPE::END];
    bool                    m_bChanged;

public:
    bool IsResourceChanged() const { return m_bChanged; }

public:
    void init();
    void tick();

private:
    void InitSound();
    void CreateDefaultMesh();
    void CreateDefaultGraphicsShader();
    void CreateDefaultComputeShader();
    void CreateDefaultMaterial();

private:
    Vec2 ChangeNDCtoUV(Vec3 _vPos);

    void DeleteRes(RES_TYPE _type, const wstring _strKey);

    friend class CEventMgr;

public:
    const map<wstring, Ptr<CRes>>& GetResources(RES_TYPE _Type) const { return m_arrRes[(UINT)_Type]; }

    // _BindFlag = D3D11_BIND_FLAG
    Ptr<CTexture> CreateTexture(const wstring& _strKey, UINT _iWidth, UINT _iHeight, 
        DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage);

    Ptr<CTexture> CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _Tex2D);

    template<typename T>
    Ptr<T> FindRes(const wstring& _strKey);

    template<typename T>
    void AddRes(const wstring& _strKey, Ptr<T>& _Res);

    template<typename T>
    Ptr<T> LoadRes(const wstring& _strKey, const wstring& _strRelativePath);
};

template<typename T>
RES_TYPE GetResType()
{
    const type_info& mesh = typeid(CMesh);
    //const type_info& meshdata = typeid(CMeshData);
    const type_info& material = typeid(CMaterial);
    const type_info& texture = typeid(CTexture);
    const type_info& sound = typeid(CSound);
    const type_info& prefab = typeid(CPrefab);
    const type_info& gs = typeid(CGraphicsShader);
    const type_info& cs = typeid(CComputeShader);

    if (typeid(T).hash_code() == mesh.hash_code())
        return RES_TYPE::MESH;
    if (typeid(T).hash_code() == gs.hash_code())
        return RES_TYPE::GRAPHICS_SHADER;
    if (typeid(T).hash_code() == texture.hash_code())
        return RES_TYPE::TEXTURE;
    if (typeid(T).hash_code() == material.hash_code())
        return RES_TYPE::MATERIAL;
    if (typeid(T).hash_code() == prefab.hash_code())
        return RES_TYPE::PREFAB;
    if (typeid(T).hash_code() == cs.hash_code())
        return RES_TYPE::COMPUTE_SHADER;
    if (typeid(T).hash_code() == sound.hash_code())
        return RES_TYPE::SOUND;

    return RES_TYPE::END;
}

template<typename T>
inline Ptr<T> CResMgr::FindRes(const wstring& _strKey)
{
    RES_TYPE type = GetResType<T>();

    map<wstring, Ptr<CRes>>::iterator iter = m_arrRes[(UINT)type].find(_strKey);
    if (iter == m_arrRes[(UINT)type].end())
        return nullptr;

    return (T*)iter->second.Get();
}

template<typename T>
inline void CResMgr::AddRes(const wstring& _strKey, Ptr<T>& _Res)
{
    assert(!FindRes<T>(_strKey).Get());

    RES_TYPE type = GetResType<T>();
    m_arrRes[(UINT)type].insert(make_pair(_strKey, _Res.Get()));
    _Res->SetKey(_strKey);

    m_bChanged = true;
}

template<typename T>
inline Ptr<T> CResMgr::LoadRes(const wstring& _strKey, const wstring& _strRelativePath)
{
    Ptr<CRes> pRes = FindRes<T>(_strKey).Get();

    if (nullptr != pRes)
        return (T*)pRes.Get();

    pRes = new T;
    pRes->SetKey(_strKey);
    pRes->SetRelativePath(_strRelativePath);

    wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
    strFilePath += _strRelativePath;
    
    if (FAILED(pRes->Load(strFilePath)))
    {
        assert(nullptr);
    }

    RES_TYPE type = GetResType<T>();
    m_arrRes[(UINT)type].insert(make_pair(_strKey, pRes));

    m_bChanged = true;

    return (T*)pRes.Get();
}

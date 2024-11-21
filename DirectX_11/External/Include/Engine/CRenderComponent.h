#pragma once
#include "CComponent.h"

#include "ptr.h"
#include "CMesh.h"
#include "CMaterial.h"

class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh>              m_pMesh;

    Ptr<CMaterial>          m_pSharedMtrl;
    Ptr<CMaterial>          m_pDynamicMtrl;
    Ptr<CMaterial>          m_pCurrentMtrl;

protected:
    bool                    m_bIsFrustumCull;

public:
    virtual void render() = 0;

public:
    void SetMesh(Ptr<CMesh> _pMesh) { assert(_pMesh.Get()); m_pMesh = _pMesh; }
    void SetMaterial(Ptr<CMaterial> _pMaterial);

    Ptr<CMesh> GetMesh() const { return m_pMesh; }
    Ptr<CMaterial> GetMaterial() const { return m_pCurrentMtrl; }
    Ptr<CMaterial> GetSharedMaterial() const { return m_pSharedMtrl; }
    Ptr<CMaterial> GetDynamicMaterial();
    
    bool HasDynamciMaterial() const { return (nullptr != m_pDynamicMtrl); };
    void RemoveDynamicMaterial() { m_pCurrentMtrl = m_pSharedMtrl; m_pDynamicMtrl = nullptr; }

    bool IsFrustumCulling() { return m_bIsFrustumCull; }

public:
    virtual void SaveToLevelFile(FILE* _pFile);
    virtual void LoadFromLevelFile(FILE* _pFile);

public:
    CRenderComponent(COMPONENT_TYPE _Type);
    CRenderComponent(const CRenderComponent& _Origin);
    ~CRenderComponent();
};


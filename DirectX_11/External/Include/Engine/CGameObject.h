#pragma once
#include "CEntity.h"

class CComponent;
class CTransform;
class CCamera;
class CCollider2D;
class CLight2D;
class CAnimator2D;
class CMeshRender;
class CParticleSystem;
class CRenderComponent;
class CTileMap;
class CScript;

// 메크로 함수
#define GET_COMPONENT(Type, TYPE) C##Type* Type() const { return (C##Type*)m_arrCom[(UINT)COMPONENT_TYPE::TYPE]; }

class CGameObject :
    public CEntity
{
private:
    CComponent*             m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent*       m_RenderCom;

    vector<CScript*>        m_vecScript;

    CGameObject*            m_Parent;
    vector<CGameObject*>    m_vecChild;

    int                     m_iLayerIdx;

    bool                    m_bDead;

    float                   m_LifeTime;
    float                   m_CurLifeTime;
    bool                    m_bLifeSpan;

    bool                    m_bIsScirptObj;

public:
    void begin();   // 레벨이 시작될 때 or 이미 시작된 레벨에 합류할 때
    void end();
    void tick();
    virtual void finaltick();

    void render();

public:
    void AddComponent(CComponent* _Component);
    void AddChild(CGameObject* _Object);
    bool IsAncestor(CGameObject* _Target);

    CComponent* GetComponent(COMPONENT_TYPE _Type) { return m_arrCom[(UINT)_Type]; }
    void DeleteComponent(COMPONENT_TYPE _Type);
    const vector<CGameObject*>& GetChild() { return m_vecChild; }

    bool IsLifeSpan() { return m_bLifeSpan; }

    bool IsScriptObj() { return m_bIsScirptObj; }
    void SetScriptObj(bool _bSet) { m_bIsScirptObj = _bSet; }

    CGameObject* GetParent() const { return m_Parent; }

    GET_COMPONENT(Transform, TRANSFORM);
    GET_COMPONENT(Camera, CAMERA);
    GET_COMPONENT(Collider2D, COLLIDER2D);
    GET_COMPONENT(Light2D, LIGHT2D);
    GET_COMPONENT(Animator2D, ANIMATOR2D);
    GET_COMPONENT(MeshRender, MESHRENDER);
    GET_COMPONENT(TileMap, TILEMAP);
    GET_COMPONENT(ParticleSystem, PARTICLESYSTEM);

    CRenderComponent* GetRenderComponent() { return m_RenderCom; }

    int GetLayerIndex() const { return m_iLayerIdx; }

    template<typename T>
    T* GetScript();

    CScript* GetScript(UINT _iScriptType);

    const vector<CScript*>& GetScripts() const { return m_vecScript; }

public:
    void DeleteScript(UINT _iScriptType);

    void SetLifeSpan(float _ftime) 
    { 
        if (m_bLifeSpan)
            return;  

        m_LifeTime = _ftime;
        m_bLifeSpan = true;
    }

    bool IsDead() const { return m_bDead; }

    void InitAllScript();

    CLONE(CGameObject);

private:
    void DisconnectFromParent();
    void ChangeToChildType();
    void AddParentList();

public:
    CGameObject();
    CGameObject(const CGameObject& _Other);
    ~CGameObject();

    friend class CLayer;
    friend class CEventMgr;
};

template<typename T>
inline T* CGameObject::GetScript()
{
    for (size_t i = 0; i < m_vecScript.size(); i++)
    {
        T* pScript = dynamic_cast<T*>(m_vecScript[i]);

        if (pScript)
        {
            return pScript;
        }
    }
    return nullptr;
}

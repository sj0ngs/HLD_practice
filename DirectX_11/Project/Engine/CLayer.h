#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{
private:
    vector<CGameObject*>    m_vecParentObj; // 레이어에 속해있는 부모 오브젝트
    vector<CGameObject*>    m_vecObject;    // 계층에 상관없이 해당 레이어에 속하는 모든 오브젝트

    int                     m_iLayerIdx;    // 레이어 번호(0 ~ 31)

public:
    int GetLayerIdx() const { return m_iLayerIdx; }

public:
    void begin();
    void end();
    void tick();
    void finaltick();

public:
    // _bMove 부모를 따라 레이어를 이동할 것인지 여부
    void AddGameObject(CGameObject* _Object, bool _bMove);
    const vector<CGameObject*>& GetParentObject() const { return m_vecParentObj; }
    const vector<CGameObject*>& GetObjects() const { return m_vecObject; }

    void RegisterObject(CGameObject* _Object);
    void RemoveFromParentList(CGameObject* _Object);
    void AddParentList(CGameObject* _Object);

    void Clear();

    CLONE(CLayer);
public:
    CLayer();
    ~CLayer();

    friend class CLevel;
};


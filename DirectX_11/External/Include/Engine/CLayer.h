#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{
private:
    vector<CGameObject*>    m_vecParentObj; // ���̾ �����ִ� �θ� ������Ʈ
    vector<CGameObject*>    m_vecObject;    // ������ ������� �ش� ���̾ ���ϴ� ��� ������Ʈ

    int                     m_iLayerIdx;    // ���̾� ��ȣ(0 ~ 31)

public:
    int GetLayerIdx() const { return m_iLayerIdx; }

public:
    void begin();
    void end();
    void tick();
    void finaltick();

public:
    // _bMove �θ� ���� ���̾ �̵��� ������ ����
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


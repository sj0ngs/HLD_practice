#pragma once
#include "CEntity.h"

class CLayer;
class CGameObject;

class CLevel :
    public CEntity
{
private:
    CLayer*     m_arrLayer[MAX_LAYER];
    LEVEL_STATE m_eState;

    vector<pair<UINT, UINT>> m_vecCollisionPair;

public:
    void begin();
    void end();
    void tick();
    void finaltick();

public:
    CLayer* FindLayerByName(const wstring& _strName);

    int GetLayerIndex(const wstring& _strName);

    LEVEL_STATE GetState() const { return m_eState; }
    void ChangeState(LEVEL_STATE _eState);

    void AddGameObject(CGameObject* _Object, int _iLayerIdx, bool _bMove);
    void AddGameObject(CGameObject* _Object, const wstring& _LayerName, bool _bMove);
    CLayer* GetLayer(int _iLayerIdx) const { assert(!(_iLayerIdx < 0)); return m_arrLayer[_iLayerIdx]; }

    CGameObject* FindObjectByName(const wstring& _strName);
    void FindObjectByName(const wstring& _strName, _Inout_ vector<CGameObject*>& _vec);

    const vector<pair<UINT, UINT>>& GetCollisionPair() const { return m_vecCollisionPair; }
    void AddCollisionPair(UINT _iLeftLayer, UINT _iRightLayer);
    void DeleteCollisionPair(size_t _iPairIdx);

    void MoveObjectLayer(CGameObject* _pObj, int _iLayer);

    // Ư�� ��ũ��Ʈ�� ������ ������Ʈ�� ���� ã���� ������Ʈ�� ��ȯ
    CGameObject* FindObjectByScript(UINT _iScriptType);

    // Ư�� ��ũ��Ʈ�� ������ ������Ʈ���� ��� ã�Ƽ� ���Ϳ� �����
    void FindObjectsByScript(UINT _iScriptType, vector<CGameObject*>& _vecObj);

    CLONE(CLevel);
private:
    // ��ϵ� GameObject ����
    void Clear();
     
    void UpdateCollisionPair();

public:
    CLevel();
    ~CLevel();

    friend class CLevelMgr;
};


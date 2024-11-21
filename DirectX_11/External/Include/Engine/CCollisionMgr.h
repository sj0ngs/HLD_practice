#pragma once
#include "CSingleton.h"

class CLayer;
class CCollider2D;

union CollisionID
{
    struct
    {
        UINT leftID;
        UINT rightID;
    };

    UINT_PTR id;
};

class CCollisionMgr :
    public CSingleton<CCollisionMgr>
{
    SINGLE(CCollisionMgr)
private:
    UINT                m_matrix[MAX_LAYER];
    map<UINT_PTR, bool> m_mapColID;

public:
    void tick();

public:
    void LayerCheck(UINT _left, UINT _right);
    void LayerCheck(const wstring& _strLeftLayer, const wstring& _strRightLayer);

    void Clear() { memset(m_matrix, 0, sizeof(UINT) * MAX_LAYER); }

    CGameObject* GetPickedObject(Vec3 _vPickPos);

private:
    void CollisionBtwLayer(CLayer* _LeftLayer, CLayer* _RightLayer);
    void CollisionBtwObj(CGameObject* _LeftObj, CGameObject* _RightObj);
    bool CollisionBtwCollider(CCollider2D* _LeftCol, CCollider2D* _RightCol);

private:
    // Collision between two boxes
    bool CollisionBtB(CCollider2D* _LeftCol, CCollider2D* _RightCol);

    // Collision between two circles
    bool CollisionCtC(CCollider2D* _LeftCol, CCollider2D* _RightCol);

    // Collision between two triangles
    bool CollisionTtT(CCollider2D* _LeftCol, CCollider2D* _RightCol);

    // Collision between a circle and a box
    // Left : Circle / Right : Box
    bool CollisionCtB(CCollider2D* _LeftCol, CCollider2D* _RightCol);
};


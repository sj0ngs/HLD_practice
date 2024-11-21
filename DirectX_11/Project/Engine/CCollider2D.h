#pragma once
#include "CComponent.h"

class CCollider2D :
    public CComponent
{
private:
    Vec3                m_vOffsetPos;
    Vec3                m_vOffsetScale;
    bool                m_bAbsolute;
    COLLIDER2D_TYPE     m_Shape;

    Matrix              m_matCollider2D;    // Collider 월드행렬
    Matrix              m_matWorldScale;    // Collider Scale 행렬

    int                 m_iCollisionCount;

    bool                m_bOn;

public:
    virtual void finaltick() override;

public:
    void SetOffsetPos(Vec2 _vOffsetPos) { m_vOffsetPos = Vec3(_vOffsetPos.x, _vOffsetPos.y, 0.f); }
    Vec2 GetOffsetPos() const { return Vec2(m_vOffsetPos.x, m_vOffsetPos.y); }

    void SetOffsetScale(Vec2 _vOffsetScale) { m_vOffsetScale = Vec3(_vOffsetScale.x, _vOffsetScale.y, 1.f); }
    Vec2 GetOffsetScale() const { return Vec2(m_vOffsetScale.x, m_vOffsetScale.y); }

    Vec2 GetWorldPos();
    Vec2 GetWorldScale();

    void SetAbsolute(bool _bSet) { m_bAbsolute = _bSet; }
    bool IsAbsolute() const { return m_bAbsolute; }

    bool IsOnCollision() { return m_bOn; }
    void OffCollision() { m_bOn = false; }
    void OnCollision() { m_bOn = true; }

    void SetCollider2DType(COLLIDER2D_TYPE _Type) { m_Shape = _Type; }
    // 스케일의 x 값을 반지름으로 봄
    COLLIDER2D_TYPE GetCollider2DType() const { return m_Shape; }

    const Matrix GetColliderWorldMatrix() const { return m_matCollider2D; }
    const Matrix GetColliderWorldScale() const { return m_matWorldScale; }

    float GetRadius() const;
    void SetRadius(float _fRadius);

public:
    CCollider2D();
    CCollider2D(const CCollider2D& _Other);
    ~CCollider2D();

    CLONE(CCollider2D);

public:
    virtual void SaveToLevelFile(FILE* _pFile);
    virtual void LoadFromLevelFile(FILE* _pFile);

public:
    void BeginOverlap(CCollider2D* _Other);
    void OnOverlap(CCollider2D* _Other);
    void EndOverlap(CCollider2D* _Other);
};


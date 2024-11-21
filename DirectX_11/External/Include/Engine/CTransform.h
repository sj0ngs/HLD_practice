#pragma once
#include "CComponent.h"

class CTransform :
    public CComponent
{
private:
    Vec3    m_vRelativePos;
    Vec3    m_vRelativeScale;

    Vec3    m_vRelativeRot;

    bool    m_bAbsolute;

    Vec3    m_vRelatveDir[3];
    Vec3    m_vWorldDir[3];     // 월드상의 최종 방향

    Matrix  m_matWorldScale;
    Matrix  m_matWorld;
    
    Vec3    m_vPrevPos;

public:
    void SetRelativePos(Vec3 _vPos) { m_vRelativePos = _vPos; }
    void SetRelativeScale(Vec3 _vScale) { m_vRelativeScale = _vScale; }
    void SetRelativeRot(Vec3 _vRot) { m_vRelativeRot = _vRot; }

    void SetRelativePos(float _x, float _y, float _z) { m_vRelativePos = Vec3(_x, _y, _z); }
    void SetRelativeScale(float _x, float _y, float _z) { m_vRelativeScale = Vec3(_x, _y, _z); }
    void SetRelativeRot(float _x, float _y, float _z) { m_vRelativeRot = Vec3(_x, _y, _z); }

    void SetAbsolute(bool _Set) { m_bAbsolute = _Set; }
    bool IsAbsolute() const { return m_bAbsolute; }

    void SetWorldMat(const Matrix& _mat) { m_matWorld = _mat; }

    Vec3 GetRelativePos() { return m_vRelativePos; }
    Vec3 GetRelativeScale() { return m_vRelativeScale; }
    Vec3 GetRelativeRot() { return m_vRelativeRot; }
    Vec3 GetRelativeDir(DIR_TYPE _Type) const { return m_vRelatveDir[(UINT)_Type]; }
    Vec3 GetWorldDir(DIR_TYPE _Type) const { return m_vWorldDir[(UINT)_Type]; }
    Vec3 GetWorldPos() const { return m_matWorld.Translation(); }

    const Matrix& GetWorldScale() const { return m_matWorldScale; }
    const Matrix& GetWorldMat() const { return m_matWorld; }

public:
    virtual void finaltick() override;
    void UpdateData();

    CLONE(CTransform);

public:
    virtual void SaveToLevelFile(FILE* _pFile);
    virtual void LoadFromLevelFile(FILE* _pFile);

public:
    CTransform();
    ~CTransform();

public:
    void BackToPrevPos() { m_vRelativePos = m_vPrevPos; }
};


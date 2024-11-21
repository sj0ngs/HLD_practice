#pragma once
#include "CComponent.h"

class CLight2D :
    public CComponent
{
private:
    tLightInfo          m_LightInfo;

    Vec3                m_vOffsetPos;
    bool                m_bAbsolute;

public:
    void SetLightType(LIGHT_TYPE _Type) { m_LightInfo.LightType = (UINT)_Type; }
    void SetLightDiffuse(Vec3 _vDiffuse) { m_LightInfo.Color.vDiffuse = _vDiffuse; }
    void SetLightAmbient(Vec3 _vAmbient) { m_LightInfo.Color.vAmbient = _vAmbient; }
    void SetLightDirection(Vec3 _fDir) { m_LightInfo.vWorldDir = _fDir; }
    void SetRadius(float _fRadius) { m_LightInfo.Radius = _fRadius; }
    void SetAngle(float _fAngle) { m_LightInfo.Angle = _fAngle; }

    void SetOffsetPos(Vec3 _vOffset) { m_vOffsetPos = _vOffset; }
    Vec3 GetOffsetPos() const { return m_vOffsetPos; }

    const tLightInfo& GetLightInfo() const { return m_LightInfo; }

public:
    virtual void finaltick() override;

    CLONE(CLight2D);

public:
    virtual void SaveToLevelFile(FILE* _pFile);
    virtual void LoadFromLevelFile(FILE* _pFile);

public:
    CLight2D();
    ~CLight2D();
};


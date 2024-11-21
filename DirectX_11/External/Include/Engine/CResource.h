#pragma once
#include "CEntity.h"

class CRes :
    public CEntity
{
private:
    const RES_TYPE  m_Type;
    UINT            m_iRefCount;

public:
    CRes(RES_TYPE _type);
    ~CRes();
};


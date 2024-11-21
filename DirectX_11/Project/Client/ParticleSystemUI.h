#pragma once
#include "ComponentUI.h"

class ParticleSystemUI :
    public ComponentUI
{
public:
    virtual int render_update() override;

public:
    ParticleSystemUI();
    ~ParticleSystemUI();

public:
    void SelectParticleTexture(DWORD_PTR _strKey);
    
private:
    void module_render();
    void Save();
};


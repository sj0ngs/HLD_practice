#include "pch.h"
#include "ParticleSystemUI.h"

#include "ListUI.h"

#include <Engine\CResMgr.h>
#include <Engine\CPathMgr.h>
#include <Engine\CParticleSystem.h>

ParticleSystemUI::ParticleSystemUI()    :
    ComponentUI("##ParticleSystem", COMPONENT_TYPE::PARTICLESYSTEM)
{
    SetName("Particle System");
}

ParticleSystemUI::~ParticleSystemUI()
{
}

int ParticleSystemUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    module_render();

    Ptr<CTexture> pTexture = GetTarget()->ParticleSystem()->GetDynamicMaterial()->GetTexParam(TEX_0);

    if (pTexture.Get())
    {
        ImGui::Text("Current Texture : ");
        ImGui::SameLine();
        string strTexName = string(pTexture->GetKey().begin(), pTexture->GetKey().end()).c_str();

        ImGui::InputText("##CurTex", (char*)strTexName.c_str(), strTexName.size(), ImGuiInputTextFlags_ReadOnly);
        
        float fAspect = (float)pTexture->Height() / (float)pTexture->Width();
        ImGui::Image(pTexture->GetSRV().Get(), ImVec2(100.f, 100.f * fAspect), 
                    ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f,1.f),
                    ImVec4(1.f, 1.f, 1.f, 1.f));

        ImGui::SameLine();
    }

    if (ImGui::Button("Change Texture"))
    {
        const map<wstring, Ptr<CRes>>& mapTex = CResMgr::GetInst()->GetResources(RES_TYPE::TEXTURE);

        ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
        pListUI->Reset("Texture List", ImVec2(300.f, 500.f));

        for (const auto& pair : mapTex)
        {
            string strName = string(pair.first.begin(), pair.first.end());
            pListUI->AddItem(strName.c_str());
        }

        pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&ParticleSystemUI::SelectParticleTexture);
    }

    if (ImGui::Button("Save Particle"))
    {
        Save();
    }

    if (ImGui::Button("Load Particle"))
    {
        wstring strParticle;
        if (S_OK == GetLoadFilePath(L"\\particle", strParticle))
        {
            GetTarget()->ParticleSystem()->Load(strParticle);
        }
    }
    
    return TRUE;
}

void ParticleSystemUI::module_render()
{
    tParticleModule tModuleData = GetTarget()->ParticleSystem()->GetModuleData();
    ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_AlphaPreview;

    int iMaxCount = tModuleData.MaxParticleCount;
    ImGui::Text("Max Particle Count");
    ImGui::SameLine();
    if (ImGui::InputInt("##MaxParticleCount", &iMaxCount, 0, INT_MAX))
    {
        tModuleData.MaxParticleCount = iMaxCount;
    }

    // Spawn
    bool bModule = tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN];
    ImGui::Checkbox("##SpawnModule", &bModule);
    tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = bModule;
    ImGui::SameLine();
    ImGui::BeginDisabled(!bModule);
    if (ImGui::CollapsingHeader("Spawn"))
    {
        // Spawn Rate
        int iSpawnRate = tModuleData.SpawnRate;
        ImGui::Text("Spawn Rate   ");
        ImGui::SameLine();
        ImGui::DragInt("##Spawn Rate", &iSpawnRate, 1.f, 0, INT_MAX);
        tModuleData.SpawnRate = iSpawnRate;

        // Space
        const char* arrSpaceType[2] =
        {
            "World",
            "Simulation"
        };
        int iSpace = tModuleData.Space;

        ImGui::Text("Space        ");
        ImGui::SameLine();
        ImGui::Combo("##Space", &iSpace, arrSpaceType, 2);
        tModuleData.Space = iSpace;

        // Life Time
        float fTime = tModuleData.MinLifeTime;
        ImGui::Text("Life Time Min");
        ImGui::SameLine();
        ImGui::DragFloat("##MinLifeTime", &fTime, 1.f, 0.f, FLT_MAX);
        tModuleData.MinLifeTime = fTime;

        fTime = tModuleData.MaxLifeTime;
        ImGui::Text("Life Time Max");
        ImGui::SameLine();
        ImGui::DragFloat("##MaxLifeTime", &fTime, 1.f, 0.f, FLT_MAX);
        tModuleData.MaxLifeTime = fTime;

        // Spawn Color
        Vec4 vColor = tModuleData.vSpawnColorMin;
        ImGui::Text("Color1       ");
        ImGui::SameLine();
        ImGui::ColorEdit4("##pawn Color Min", vColor, misc_flags);
        tModuleData.vSpawnColorMin = vColor;

        vColor = tModuleData.vSpawnColorMax;
        ImGui::Text("Color2       ");
        ImGui::SameLine();
        ImGui::ColorEdit4("##pawn Color Max", vColor, misc_flags);
        tModuleData.vSpawnColorMax = vColor;

        // Spawn Scale
        Vec3 vScale = tModuleData.vSpawnScaleMin;
        ImGui::Text("Scale Min    ");
        ImGui::SameLine();
        ImGui::DragFloat3("##vSpawnScaleMin", vScale);
        tModuleData.vSpawnScaleMin = vScale;

        vScale = tModuleData.vSpawnScaleMax;
        ImGui::Text("Scale Max    ");
        ImGui::SameLine();
        ImGui::DragFloat3("##vSpawnScaleMax", vScale);
        tModuleData.vSpawnScaleMax = vScale;

        int iSpawnShape = tModuleData.SpawnShapeType;

        const char* arrShapeType[2] =
        {
            "Box",
            "Circle"
        };

        ImGui::Text("Type         ");
        ImGui::SameLine();
        ImGui::Combo("##SpawnShape", &iSpawnShape, arrShapeType, 2);
        tModuleData.SpawnShapeType = iSpawnShape;

		switch (iSpawnShape)
		{
		case 0: // Box
		{
            vScale = tModuleData.vBoxShapeScale;
            ImGui::Text("Box Size     ");
            ImGui::SameLine();
            ImGui::DragFloat3("##Box Scale", vScale);
            tModuleData.vBoxShapeScale = vScale;
			break;
		}
		case 1: // Sphere
		{
            float fRadius = tModuleData.fSphereShapeRadius;
            ImGui::Text("Sphere Radius");
            ImGui::SameLine();
            ImGui::DragFloat("##Sphere Radius", &fRadius);
            tModuleData.fSphereShapeRadius = fRadius;
			break;
		}
		}
    }
    ImGui::EndDisabled();

    // Color Change
    bModule = tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::COLOR_CHANGE];
    ImGui::Checkbox("##ColorChangeModule", &bModule);
    tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::COLOR_CHANGE] = bModule;
    ImGui::SameLine();
    ImGui::BeginDisabled(!bModule);
    if (ImGui::CollapsingHeader("Color Change"))
    {
        Vec4 vColor = tModuleData.vStartColor;
        ImGui::Text("Start Color");
        ImGui::SameLine();
        ImGui::ColorEdit4("##Start Color", vColor, misc_flags);
        tModuleData.vStartColor = vColor;

        vColor = tModuleData.vEndColor;
        ImGui::Text("End Color  ");
        ImGui::SameLine();
        ImGui::ColorEdit4("##End Color", vColor, misc_flags);
        tModuleData.vEndColor = vColor;
    }
    ImGui::EndDisabled();

    // Scale Change
    bModule = tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::SCALE_CHANGE];
    ImGui::Checkbox("##ScaleChangeModule", &bModule);
    tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::SCALE_CHANGE] = bModule;
    ImGui::SameLine();
    ImGui::BeginDisabled(!bModule);
    if (ImGui::CollapsingHeader("Scale Change"))
    {
        float fValue = tModuleData.StartScale;
        ImGui::Text("Start Scale");
        ImGui::SameLine();
        ImGui::DragFloat("##Start Scale", &fValue, 0.01f, 0.f, FLT_MAX);
        tModuleData.StartScale = fValue;

        fValue = tModuleData.EndScale;
        ImGui::Text("End Scale  ");
        ImGui::SameLine();
        ImGui::DragFloat("##End Scale", &fValue, 0.01f, 0.f, FLT_MAX);
        tModuleData.EndScale = fValue;
    }
    ImGui::EndDisabled();

    // Add Velocity
    bModule = tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY];
    ImGui::Checkbox("##AddVelocityModule", &bModule);
    tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = bModule;
    ImGui::SameLine();
    ImGui::BeginDisabled(!bModule);
    if (ImGui::CollapsingHeader("Movement"))
    {
        // Speed
        float fSpeed = tModuleData.MinSpeed;
        ImGui::Text("Speed Min   ");
        ImGui::SameLine();
        ImGui::DragFloat("##MinSpeed", &fSpeed, 1.f, 0.f, FLT_MAX);
        tModuleData.MinSpeed = fSpeed;

        fSpeed = tModuleData.MaxSpeed;
        ImGui::Text("Speed Max   ");
        ImGui::SameLine();
        ImGui::DragFloat("##MaxSpeed", &fSpeed, 1.f, 0.f, FLT_MAX);
        tModuleData.MaxSpeed = fSpeed;

        int iAddVelocityType = tModuleData.AddVelocityType;

        const char* arrAddVelocityType[3] =
        {
            "Spread",
            "Converge",
            "Fixed"
        };

        ImGui::Text("Type        ");
        ImGui::SameLine();
        ImGui::Combo("##AddVelocityType", &iAddVelocityType, arrAddVelocityType, 3);
        tModuleData.AddVelocityType = iAddVelocityType;

        if (2 == iAddVelocityType)
        {
            float fAngle = tModuleData.OffsetAngle;
            ImGui::Text("Offset Angle");
            ImGui::SameLine();
            ImGui::DragFloat("##OffsetAngle", &fAngle, 1.f, 0.f, 360.f);
            tModuleData.OffsetAngle = fAngle;

            Vec3 vDir = tModuleData.vVelocityDir;
            ImGui::Text("Fixed Dir   ");
            ImGui::SameLine();
            ImGui::DragFloat3("##vVelocityDir", vDir);
            tModuleData.vVelocityDir = vDir;
        }
    }
    ImGui::EndDisabled();

    // Drag
    bModule = tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::DRAG];
    ImGui::Checkbox("##DragModule", &bModule);
    tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::DRAG] = bModule;
    ImGui::SameLine();
    ImGui::BeginDisabled(!bModule);
    if (ImGui::CollapsingHeader("Drag"))
    {
        float fSpeed = tModuleData.StartDrag;
        ImGui::Text("Start Drag");
        ImGui::SameLine();
        ImGui::DragFloat("##StartDrag", &fSpeed, 1.f);
        tModuleData.StartDrag = fSpeed;

        fSpeed = tModuleData.EndDrag;
        ImGui::Text("End Drag  ");
        ImGui::SameLine();
        ImGui::DragFloat("##EndDrag", &fSpeed, 1.f);
        tModuleData.EndDrag = fSpeed;
    }
    ImGui::EndDisabled();

    // NOISE_FORCE
    bModule = tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE];
    ImGui::Checkbox("##NoiseForceModule", &bModule);
    tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE] = bModule;
    ImGui::SameLine();
    ImGui::BeginDisabled(!bModule);
    if (ImGui::CollapsingHeader("Random Move"))
    {
        float fTime = tModuleData.fNoiseTerm;
        ImGui::Text("Delay");
        ImGui::SameLine();
        ImGui::DragFloat("##fNoiseTerm", &fTime, 0.01f, 0.01f, FLT_MAX);
        tModuleData.fNoiseTerm = fTime;
        
        float fPower = tModuleData.fNoiseForce;
        ImGui::Text("Power");
        ImGui::SameLine();
        ImGui::DragFloat("##fNoiseForce", &fPower, 1.f, 0.f, FLT_MAX);
        tModuleData.fNoiseForce = fPower;
    }
    ImGui::EndDisabled();

    // RENDER
    bModule = tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER];
    ImGui::Checkbox("##RenderModule", &bModule);
    tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER] = bModule;
    ImGui::SameLine();
    ImGui::BeginDisabled(!bModule);
    if (ImGui::CollapsingHeader("Rendering"))
    {
        bool bStretch = tModuleData.VelocityScale;
        ImGui::Text("Stretch      ");
        ImGui::SameLine();
        ImGui::Checkbox("##VelocityScale", &bStretch);
        tModuleData.VelocityScale = bStretch;

        if (bStretch)
        {
            Vec3 vScale = tModuleData.MaxVelocityScale;
            ImGui::Text("Max Scale    ");
            ImGui::SameLine();
            ImGui::DragFloat3("##MaxVelocityScale", vScale, 1.f, 0.f, FLT_MAX);
            tModuleData.MaxVelocityScale = vScale;

            float fSpeed = tModuleData.fMaxSpeed;
            ImGui::Text("Max Speed    ");
            ImGui::SameLine();
            ImGui::DragFloat("##fMaxSpeed", &fSpeed, 1.f, 0.f, FLT_MAX);
            tModuleData.fMaxSpeed = fSpeed;
        }

        const char* arrRotType[3] =
        {
            "Not Use",
            "Z Rotation",
            "Velocity Alignment"
        };

        int iRotType = tModuleData.RotationType;
        ImGui::Text("Rotation Type");
        ImGui::SameLine();
        ImGui::Combo("##RotationType", &iRotType, arrRotType, 3);
        tModuleData.RotationType = iRotType;

        if (1 == iRotType)
        {
            float fAngle = tModuleData.MinRotAngle;
            ImGui::Text("Rot Angle Min");
            ImGui::SameLine();
            ImGui::DragFloat("##MinRotAngle", &fAngle, 1.f, -360.f, 360.f);
            tModuleData.MinRotAngle = fAngle;

            fAngle = tModuleData.MaxRotAngle;
            ImGui::Text("Rot Angle Max");
            ImGui::SameLine();
            ImGui::DragFloat("##MaxRotAngle", &fAngle, 1.f, -360.f, 360.f);
            tModuleData.MaxRotAngle = fAngle;
        }
    }
    ImGui::EndDisabled();

    // ANIMATION
    bModule = tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::ANIMATION];
    ImGui::Checkbox("##AnimationModule", &bModule);
    tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::ANIMATION] = bModule;
    ImGui::SameLine();
    ImGui::BeginDisabled(!bModule);
    if (ImGui::CollapsingHeader("Animation"))
    {
        int iTile = tModuleData.TileX;
        ImGui::Text("Tile X   ");
        ImGui::SameLine();
        ImGui::InputScalar("##TileX", ImGuiDataType_S32, &iTile);

        if (0 >= iTile)
        {
            iTile = 1;
        }
        tModuleData.TileX = iTile;

        iTile = tModuleData.TileY;
        ImGui::Text("Tile Y   ");
        ImGui::SameLine();
        ImGui::InputScalar("##TileY", ImGuiDataType_S32, &iTile);

        if (0 >= iTile)
        {
            iTile = 1;
        }
        tModuleData.TileY = iTile;

        const char* arrAnimMode[2] =
        {
            "Play Once",
            "Frame Time"
        };

        int iPlayMode = tModuleData.PlayMode;
        ImGui::Text("Play Mode");
        ImGui::SameLine();
        ImGui::Combo("##PlayMode", &iPlayMode, arrAnimMode, 2);
        tModuleData.PlayMode = iPlayMode;

        if (1 == iPlayMode)
        {
            float fDuration = tModuleData.fDuration;
            ImGui::Text("Duration ");
            ImGui::SameLine();
            ImGui::DragFloat("##fDuration", &fDuration, 1.f, 0.01f, FLT_MAX);
            tModuleData.fDuration = fDuration;
        }
    }
    ImGui::EndDisabled();

    // GRAVITY
    bModule = tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::GRAVITY];
    ImGui::Checkbox("##GravityModule", &bModule);
    tModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::GRAVITY] = bModule;
    ImGui::SameLine();
    ImGui::BeginDisabled(!bModule);
    if (ImGui::CollapsingHeader("Gravity"))
    {
        Vec3 vDir = tModuleData.vGravityDir;
        ImGui::Text("Gravity Dir");
        ImGui::SameLine();
        ImGui::DragFloat3("##vGravityDir", vDir, 0.1f);
        tModuleData.vGravityDir = vDir;

        float fScale = tModuleData.GravityScale;
        ImGui::Text("Duration   ");
        ImGui::SameLine();
        ImGui::DragFloat("##fDuration", &fScale, 0.1f, 0.f, FLT_MAX);
        tModuleData.GravityScale = fScale;
    }
    ImGui::EndDisabled();
    
    // ORBIT
    //if (ImGui::CollapsingHeader("Orbit"))
    //{
    //    float fNoiseTerm;
    //    float fNoiseForce;
    //}

    GetTarget()->ParticleSystem()->SetModuleData(tModuleData);
}

void ParticleSystemUI::Save()
{
    // open a file name
    OPENFILENAME ofn = {};

    wstring strTileFolderPath = CPathMgr::GetInst()->GetContentPath();
    strTileFolderPath += L"particle\\";

    wchar_t szFilePath[256] = {};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFilePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 256;
    ofn.lpstrFilter = L"ALL\0*.*";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = strTileFolderPath.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (false == GetSaveFileName(&ofn))
        return;

    wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
    size_t start = strFilePath.length();
    wstring Buff = szFilePath;
    size_t end = Buff.length();
    wstring FilePath = Buff.substr(start, end);

    GetTarget()->ParticleSystem()->Save(FilePath);
}

void ParticleSystemUI::SelectParticleTexture(DWORD_PTR _strKey)
{
    string strKey = (char*)_strKey;

    Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(wstring(strKey.begin(), strKey.end()));
    GetTarget()->ParticleSystem()->GetMaterial()->SetTexParam(TEX_0, pTexture);
}
#ifndef _PARTICLE_UPDATE
#define _PARTICLE_UPDATE

#include "value.fx"
#include "struct.fx"
#include "func.fx"

RWStructuredBuffer<tParticle>       ParticleBuffer : register(u0);
RWStructuredBuffer<int4>            ParticleSpawnCount : register(u1);
StructuredBuffer<tParticleModule>   ParticleModuleData : register(t20);

Texture2D NoiseTexture : register(t21);

#define ObjectPos           g_vec4_0

#define NoiseTexResolution  g_vec2_0

#define SpawnCount          ParticleSpawnCount[0].x
#define ModuleData          ParticleModuleData[0]
#define ParticleMaxCount    ParticleModuleData[0].iMaxParticleCount

#define SpawnModule         ParticleModuleData[0].Spawn
#define ColorChangeModulde  ParticleModuleData[0].ColorChange
#define ScaleChangeModulde  ParticleModuleData[0].ScaleChange

[numthreads(128, 1, 1)]
void CS_ParticleUpdate(int3 _ID : SV_DispatchThreadID)
{
    if (ParticleMaxCount <= _ID.x)
        return;
    
    tParticle particle = ParticleBuffer[_ID.x];
    
    if (SpawnModule)
    {
        if (0 == particle.Active)
        {
            while (0 < SpawnCount)
            {
                int orgvalue = SpawnCount;
                int outvalue = 0;
                InterlockedCompareExchange(SpawnCount, orgvalue, SpawnCount - 1, outvalue);
            
                if (orgvalue == outvalue)
                {
                    float3 vOut1 = (float3) 0.f;
                    float3 vOut2 = (float3) 0.f;
                    float3 vOut3 = (float3) 0.f;
                    
                    float fNormalizedThreadID = (float) _ID.x / (float) ParticleMaxCount;
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizedThreadID, vOut1);
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizedThreadID + 0.1f, vOut2);
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizedThreadID + 0.2f, vOut3);
                   
                    // Box
                    if (0 == ModuleData.SpawnShapeType)
                    {
                        particle.vLocalPos.xyz = float3(vOut1.r * ModuleData.vBoxShapeScale.x - ModuleData.vBoxShapeScale.x / 2.f,
                                                        vOut2.r * ModuleData.vBoxShapeScale.y - ModuleData.vBoxShapeScale.y / 2.f,
                                                        vOut3.r * ModuleData.vBoxShapeScale.z - ModuleData.vBoxShapeScale.z / 2.f);
                    }
                    // Sphere
                    else if (1 == ModuleData.SpawnShapeType)
                    {
                        float2 vDir = float2(vOut1.r * 2.f - 1.f, vOut2.r * 2.f - 1.f);
                        vDir = normalize(vDir);
                        
                        float fRadius = vOut3.r * ModuleData.fSphereShapeRadius * 1.2f - ModuleData.fSphereShapeRadius * 0.1f;
                        
                        particle.vLocalPos.xyz = float3(vDir.x * fRadius, vDir.y * fRadius, 0.f);
                    }
                    
                    // Particle Mass
                    particle.Mass = 1.f;
                    
                    particle.vWorldPos.xyz = ObjectPos.xyz + particle.vLocalPos.xyz;
                    
                    // Add Velocity Module
                    if (ModuleData.AddVelocity)
                    {
                        float fSpeed = lerp(ModuleData.MinSpeed, ModuleData.MaxSpeed, vOut1.r);
                        switch (ModuleData.AddVelocityType)
                        {
                            case 0: // Spread From Center
                            {
                                particle.vVelocity.xyz = normalize(particle.vLocalPos.xyz) * fSpeed;
                                break;
                            }
                            case 1: // Coverge to Center
                            {
                                particle.vVelocity.xyz = normalize(particle.vLocalPos.xyz) * fSpeed * -1.f;
                                break;
                            }
                            case 2: // Fixed Direction
                            {
                                float fRadian = radians(ModuleData.OffsetAngle);
                                float fAngle = lerp(0.f, fRadian, vOut3.r) - fRadian / 2.f;
                                float4 vDir = (float4) 0.f;
                                vDir.xyz = normalize(ModuleData.vVelocityDir.xyz);
                                float fOriginAngle = acos(vDir.x) * (vDir.y >= 0.f ? 1.f : -1.f);
                                vDir.x = cos(fOriginAngle + fAngle);
                                vDir.y = sin(fOriginAngle + fAngle);
                                particle.vVelocity.xyz = normalize(vDir.xyz) * fSpeed;
                                break;
                            }
                        }
                    }
                    
                    // Render Normal Rotation
                    if (1 == ModuleData.Render && 1 == ModuleData.RotationType)
                    {
                        particle.RotFactor = radians(lerp(ModuleData.MinRotAngle, ModuleData.MaxRotAngle, vOut1.r));
                    }
                    
                    // Animation Module
                    if (ModuleData.Animation)
                    {
                        if (0 != ModuleData.PlayMode)
                        {
                            int Idx = ModuleData.TileX * ModuleData.TileY;
                            particle.AnimFrame = (int) (vOut2.r * 1000) % Idx;  
                        }
                    }
                   
                    // Scale
                    particle.vWorldScale.xyz = lerp(ModuleData.vSpawnScaleMin, ModuleData.vSpawnScaleMax, vOut3.r).xyz;
                    
                    // Lifetime
                    particle.LifeTime = lerp(ModuleData.MinLifeTime, ModuleData.MaxLifeTime, vOut2.r);
                    
                    particle.vColor = lerp(ModuleData.vSpawnColorMin, ModuleData.vSpawnColorMax, vOut1.r);
                    particle.Active = 1;
                    break;
                }
            }
        }
    }

    // 파티클 동작 부분
    if (particle.Active)
    {
        // Life Check
        particle.PrevAge = particle.Age;
        particle.Age += g_DT;
        particle.NormalizedAge = saturate(particle.Age / particle.LifeTime);
        
        particle.vForce.xyz = (float3) 0.f;
        
        if (particle.LifeTime <= particle.Age)
        {
            particle.Active = 0;
            particle.Age = 0.f;
        }
        
        // Noise Force Module
        if (ModuleData.NoiseForce)
        {
            if (0.f == particle.PrevAge || 
                trunc(particle.Age / ModuleData.fNoiseTerm) != trunc(particle.PrevAge / ModuleData.fNoiseTerm))
            {
                float3 vOut1 = (float3) 0.f;
                float3 vOut2 = (float3) 0.f;
                float3 vOut3 = (float3) 0.f;
                    
                float fNormalizedThreadID = (float) _ID.x / (float) ParticleMaxCount;
                GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizedThreadID, vOut1);
                GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizedThreadID + 0.1f, vOut2);
                GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizedThreadID + 0.2f, vOut3);
                
                float3 vForce = float3(vOut1.x, vOut2.x, vOut3.x) * 2.f - 1.f;
                particle.vRandomForce.xyz = normalize(vForce) * ModuleData.fNoiseForce;
            }
            
            particle.vForce.xyz += particle.vRandomForce.xyz;
        }
        
        // Orbit Module
        if (ModuleData.Orbit)
        {
            float3 vVelocity = normalize(particle.vLocalPos.xyz);
            float3 vAxis = float3(0.f, 0.f, 1.f);
            float3 vCross = cross(vVelocity, vAxis);
            float fRadian = radians(ModuleData.OrbitDegree * g_DT);
            
            float fLength = length(particle.vLocalPos.xyz);

            float3 vAddVelocity = fLength * tan(fRadian) * vCross * -1.f;
            
            if(ModuleData.AddVelocity)
            {
                float fSpeed = length(particle.vVelocity.xyz);
                particle.vVelocity.xyz = normalize(particle.vVelocity.xyz + vAddVelocity) * fSpeed;
            }
            else
            {      
                particle.vVelocity.xyz = vAddVelocity / g_DT;
            }

            if (0 == ModuleData.Space)
            {
                float3x3 matOrbit =
                {
                    cos(fRadian), sin(fRadian), 0,
                    -1.f * sin(fRadian), cos(fRadian), 0,
                    0, 0, 1,
                };
                
                particle.vLocalPos.xyz = mul(vVelocity, matOrbit) * fLength;
            }
        }
        
        // Gravity Module
        if (ModuleData.Gravity)
        {
            float3 vForce = normalize(ModuleData.vGravityDir) * ModuleData.GravityScale * particle.Mass;
            particle.vForce.xyz += vForce;
        }
        
        float3 vAccel = particle.vForce.xyz / particle.Mass;
        particle.vVelocity.xyz += vAccel * g_DT;
        
        // Drag Module
        if (ModuleData.Drag)
        {
            float fSpeed = length(particle.vVelocity);
            float fDrag = lerp(ModuleData.StartDrag, ModuleData.EndDrag, particle.NormalizedAge);
            
            if(0.f > fDrag)
            {
                fDrag = 0.00000001f;
            }
            
            //if (fDrag < fSpeed)
            //{          
                
            //}
            particle.vVelocity.xyz = normalize(particle.vVelocity).xyz * fDrag;
        }
        
        // 속도에 따른 파티클 이동
        if (0 == ModuleData.Space)
        {
            particle.vWorldPos += particle.vVelocity * g_DT;
        }
        else if (1 == ModuleData.Space)
        {
            particle.vLocalPos += particle.vVelocity * g_DT;
            particle.vWorldPos.xyz = particle.vLocalPos.xyz + ObjectPos.xyz;
        }
        
        // Scale Change Module
        if (ModuleData.ScaleChange)
        {
            particle.ScaleFactor = lerp(ModuleData.StartScale, ModuleData.EndScale, particle.NormalizedAge);
        }
        else
        {
            particle.ScaleFactor = 1.f;
        }
        
        // 색상 변화 모듈
        if (ModuleData.ColorChange)
        {
            particle.vColor = lerp(ModuleData.vStartColor, ModuleData.vEndColor, particle.NormalizedAge);
        }
        
        // 일반 회전 
        if (1 == ModuleData.Render && 1 == ModuleData.RotationType)
        {
            particle.RotData += g_DT * particle.RotFactor;
        }
        
        // Animation Module
        if (ModuleData.Animation)
        {
            int MaxFrame = ModuleData.TileX * ModuleData.TileY;
            int CurFrame = particle.AnimFrame;
            
            if (0 == ModuleData.PlayMode)
            {
                CurFrame = (int)floor(particle.NormalizedAge * MaxFrame);
            }
            else if (1 == ModuleData.PlayMode)
            {
                if (trunc(particle.Age / ModuleData.fDuration) != trunc(particle.PrevAge / ModuleData.fDuration))
                {
                    CurFrame++;
                    
                    if (MaxFrame <= CurFrame)
                    {
                        CurFrame = 0;
                    }
                }
            }
            
            particle.AnimFrame = CurFrame;
        }
    }
    
    ParticleBuffer[_ID.x] = particle;
}


#endif
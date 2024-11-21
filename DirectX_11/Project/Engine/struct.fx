#ifndef _STRUCT
#define _STRUCT

struct tLightColor
{
    float4 vDiffuse; //빛의 색상
    float4 vAmbient; // 주변광(환경광)
};

// LightInfo
struct tLightInfo
{
    tLightColor Color;
    
    float4  vWorldPos;
    float4  vWorldDir;
    
    int     LightType;
    float   Radius;
    float   Angle;
    int     Padding;
};

// Particle
struct tParticle
{
    float4  vLocalPos;
    float4  vWorldPos;
    float4  vWorldScale;
    float4  vColor;
    float4  vVelocity;
    float4  vForce;
    float4  vRandomForce;

    float   Age;
    float   PrevAge;
    float   NormalizedAge;
    float   LifeTime;
    float   Mass;
    
    float   ScaleFactor;
    float   RotFactor;
    float   RotData;
    int     AnimFrame;
    
    int     Active;
    int2    padd;
};

struct tParticleModule
{
	// Spawn Module
    float4  vSpawnColorMin;
    float4  vSpawnColorMax;
    float4  vSpawnScaleMin;
    float4  vSpawnScaleMax;
    float3  vBoxShapeScale;
    float   fSphereShapeRadius;
    int     SpawnShapeType;
    int     SpawnRate;
    int     Space;              // 0 : World, 1 : Local
    float   MinLifeTime;
    float   MaxLifeTime;
    int3    spawnpad;

	// Color Change Module
    float4  vStartColor; // 초기 색상
    float4  vEndColor; // 최종 색상

	// Scale Change Module 
    float   StartScale; // 초기 크기
    float   EndScale; // 최종 크기

    int     iMaxParticleCount;
    int     ipad;
    
    // Add Velocity Module
    float4  vVelocityDir;
    int     AddVelocityType; // 0 : From Center, 1 : Fixed Direction
    float   OffsetAngle;
    float   MinSpeed;
    float   MaxSpeed;

	// Drag Module
    float   StartDrag;
    float   EndDrag;

    // Noise Force Module
    float   fNoiseTerm;
    float   fNoiseForce;
    
    // Render Module
    int     RotationType; // 0 : Not Use, 1 : Z Rotation, 2 : Velocity Alignment
    int     VelocityScale;
    float   fMaxSpeed;
    float   MinRotAngle;
    float   MaxRotAngle;
    float3  RotPad;

    float4  MaxVelocityScale;
    
    // Animation 모듈
    int     TileX;
    int     TileY;
    int     PlayMode; // 0 : Play Once, 1 : Frame Time, 2 : MultiSelect
    float   fDuration;
    
    // Gravity 모듈
    float3  vGravityDir;
    float   GravityScale;
    
    // Orbit 모듈
    float OrbitDegree;
    float3 orbitpadd;
   
	// Module Check
    int     Spawn;
    int     ColorChange;
    int     ScaleChange;
    int     AddVelocity;
    
    int     Drag;
    int     NoiseForce;
    int     Render;
    int     Animation;
    
    int     Gravity;
    int     Orbit;
    int2    chekcpadd;
};
#endif
#pragma once

struct tVertex
{
	Vec3 vPos;
	Vec4 vColor;
	Vec2 vUV;
};

typedef tVertex Vtx;

// Event 
struct tEvent
{
	EVENT_TYPE Type;
	DWORD_PTR wParam;
	DWORD_PTR lParam;
};

struct tDebugShapeInfo
{
	SHAPE_TYPE	eShape;
	Matrix		matWorld;
	Vec3		vWorldPos;
	Vec3		vWorldScale;
	Vec3		vWorldRotation;
	Vec4		vColor;
	float		fMaxTime;
	float		fCurTime;
};

struct tTile
{
	Vec2 vLeftTop;
	Vec2 vSlice;
};

struct tAnim2DFrm
{
	Vec2	LeftTopUV;
	Vec2	SliceUV;
	Vec2	Offset;
	float	fDuration;
	wstring Notify;
};


// ============
// Const Buffer
// ============
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;

	Matrix matWV;
	Matrix matWVP;
};
extern tTransform g_transform;

struct tMtrlConst
{
	int arrInt[4];
	float arrFloat[4];
	Vec2 arrV2[4];
	Vec4 arrV4[4];
	Matrix arrMat[4];

	int arrTex[(UINT)TEX_PARAM::TEX_END];
};

struct tGlobal
{
	Vec2	Resolution;
	float   tDT;
	float   tAccTime;
	UINT    Light2DCount;
	UINT    Light3DCount;
	int     globalPadding[2];
};

extern tGlobal GlobalData;


// =================
// Structured Buffer
// =================

struct tLightColor
{
	Vec4 vDiffuse;	//���� ����
	Vec4 vAmbient; // �ֺ���(ȯ�汤)
};

// LightInfo
struct tLightInfo
{
	tLightColor Color;
	Vec4		vWorldPos;
	Vec4		vWorldDir;

	UINT		LightType;
	float		Radius;
	float		Angle;
	int			Padding;
};

// Particle
struct tParticle
{
	Vec4 vLocalPos;
	Vec4 vWorldPos;
	Vec4 vWorldScale;
	Vec4 vColor;
	Vec4 vVelocity;
	Vec4 vForce;
	Vec4 vRandomForce;

	float Age;
	float PrevAge;
	float NormalizedAge;
	float LifeTime;
	float Mass;
	float ScaleFactor;
	float RotFactor;
	float RotData;
	int AnimFrame;

	int Active;
	int padd[2];
};

struct tRWParticleBuffer
{
	int SpawnCount;		// �̹� �����ӿ� ���� ��ų ����
	int padding[3];
};

struct tParticleModule
{
	// Spawn Module
	Vec4 vSpawnColorMin;
	Vec4 vSpawnColorMax;
	Vec4 vSpawnScaleMin;
	Vec4 vSpawnScaleMax;
	Vec3 vBoxShapeScale;
	float fSphereShapeRadius;
	int SpawnShapeType;			// 0 : box , 1 : sphere
	int SpawnRate;				// �ʴ� ���� ����
	int Space;					// 0 : World , 1 : Simulation
	float MinLifeTime;
	float MaxLifeTime;
	float fpad[3];

	// Color Change Module
	Vec4 vStartColor;		// �ʱ� ����
	Vec4 vEndColor;			// ���� ����

	// Scale Change Module 
	float StartScale;		// �ʱ� ����
	float EndScale;			// ���� ����

	// ���� �ִ� ũ��
	int MaxParticleCount;
	int ipad;

	// Add Velocity Module
	Vec4 vVelocityDir;
	int AddVelocityType;	// 0 : Spread From Center, 1 : Converge to Center, 2 : Fixed Direction
	float OffsetAngle;
	float MinSpeed;
	float MaxSpeed;

	// Drag Module - �ӵ� ����
	float StartDrag;
	float EndDrag;

	// Noise Force Module
	float fNoiseTerm;
	float fNoiseForce;

	// Render ���
	int RotationType;	// 0 : Not Use, 1 : Z Rotation, 2 : Velocity Alignment
	int VelocityScale;
	float fMaxSpeed;
	float MinRotAngle;
	float MaxRotAngle;
	float RotPad[3];

	Vec4 MaxVelocityScale;

	// Animation ���
	int TileX;
	int TileY;
	int PlayMode;	// 0 : Play Once, 1 : Frame Time
	float fDuration;

	// Gravity 
	Vec3	vGravityDir;
	float   GravityScale;

	// Orbit 
	float	OrbitDegree;
	float   orbitpadd[3];

	// Module Check
	int	ModuleCheck[(UINT)PARTICLE_MODULE::END];
};

struct tRoomCollisionResult
{
	int     iResult;  // 0 : �浹 ����, 1 : ��������, 2 : ��
	float   fDist;
	Vec2    vDir;
	UINT    uDist;
	float	arrPadding[3];
};
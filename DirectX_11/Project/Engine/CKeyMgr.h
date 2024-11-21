#pragma once

enum class KEY
{
	UP,		
	DOWN,
	LEFT,
	RIGHT,

	SPACE,
	ENTER,
	ESC,
	LALT,
	LCTRL,
	LSHIFT,
	TAB,

	LBTN,
	RBTN,
	
	Q,
	W,
	E,
	R,

	T,
	Y,
	U,
	I,
	O,
	P,

	A,
	S,
	D,
	F,

	Z,
	X,
	C,
	V,
	B,

	_0,
	_1,
	_2,
	_3,
	_4,
	_5,
	_6,
	_7,
	_8,
	_9,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10, 
	F11,
	F12,
	
	END,
};

enum class KEY_STATE
{
	TAP,
	PRESSED,
	RELEASED,
	NONE,
};


struct tKeyInfo
{
	KEY			key;
	KEY_STATE	state;
	bool		bPrev;
};




class CKeyMgr 
	: public CSingleton<CKeyMgr>
{
	SINGLE(CKeyMgr);
private:
	vector<tKeyInfo>	m_vecKey;
	Vec2				m_vMousePos;
	Vec2				m_vPreMousePos;
	Vec2				m_vMouseDir;

	Vec3				m_vMouseWorldPos;
	Vec3				m_vMouseNormal;
	
	float				m_fMouseWheelMovememt;

public:
	void init();
	void tick();

public:
	KEY_STATE GetKeyState(KEY _key) const { return m_vecKey[(UINT)_key].state; }
	Vec2 GetMousePos() const { return m_vMousePos; }
	Vec3 GetMouseWorldPos() const { return m_vMouseWorldPos; }
	Vec3 GetMouseNormal() const { return m_vMouseNormal; }

	Vec2 GetMouseDir() const { return m_vMouseDir; }

	void SetMouseWheelMovement(float _fMove) { m_fMouseWheelMovememt = _fMove; }
	float GetmouseWheelMovement() const { return m_fMouseWheelMovememt; }

private:
	void CalcMousePos();
};


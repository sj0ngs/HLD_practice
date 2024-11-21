#pragma once

template<typename T>
class Ptr
{
private:
	T* m_Res;

public:
	T* Get() const { return m_Res; }

public:
	void operator = (T* _Res)
	{
		if (nullptr != m_Res)
			m_Res->Release();

		m_Res = _Res;

		if (nullptr != m_Res)
			m_Res->AddRef();
	}

	void operator = (const Ptr<T>& _Res)
	{
		if (nullptr != m_Res)
			m_Res->Release();

		m_Res = _Res.m_Res;

		if (nullptr != m_Res)
			m_Res->AddRef();
	}

	T* operator -> () const
	{
		return m_Res;
	}

	bool operator == (T* _Other)
	{
		return _Other == m_Res;
	}

	bool operator != (T* _Other)
	{
		return _Other != m_Res;
	}

	bool operator == (const Ptr<T>& _Other)
	{
		return _Other.m_Res == m_Res;
	}

	bool operator != (const Ptr<T>& _Other)
	{
		return _Other.m_Res != m_Res;
	}

public:
	Ptr()	:
		m_Res(nullptr)
	{

	}

	Ptr(T* Res) :
		m_Res(Res)
	{
		if (nullptr != Res)
			m_Res->AddRef();
	}

	Ptr(const Ptr<T>& _Res)	:
		m_Res(_Res.m_Res)
	{
		if (nullptr != m_Res)
			m_Res->AddRef();
	}

	~Ptr()
	{
		if (nullptr != m_Res)
			m_Res->Release();
	}
};

template<typename T>
bool operator == (void* _Res, const Ptr<T>& _Ptr)
{
	return _Res == _Ptr.Get();
}

template<typename T>
bool operator != (void* _Res, const Ptr<T>& _Ptr)
{
	return _Res != _Ptr.Get();
}
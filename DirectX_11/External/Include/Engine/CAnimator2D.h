#pragma once
#include "CComponent.h"

class CAnim2D;

class CAnimator2D :
    public CComponent
{
private:
    map<wstring, CAnim2D*>  m_mapAnim;
    CAnim2D*                m_pCurAnim;
    bool                    m_bRepeat;
    bool                    m_bStop;

public:
    const map<wstring, CAnim2D*>& GetAnims() const { return m_mapAnim; }

public:
    virtual void finaltick() override;
    void UpdateData();

    CLONE(CAnimator2D);

public:
    virtual void SaveToLevelFile(FILE* _pFile);
    virtual void LoadFromLevelFile(FILE* _pFile);

public:
    CAnimator2D();
    CAnimator2D(const CAnimator2D& _Origin);
    ~CAnimator2D();

public:
    void CreateAnim(const wstring& _strAnimName, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, Vec2 _vBackSize, int _FrameCount, int _FPS);
    void LoadAnim(const wstring& _strFilePath);

    CAnim2D* FindAnim(const wstring& _strAnimName);
    void DeleteAnim(const wstring& _strAnimName);

    void Play(const wstring& _strAnimName, bool _bRepeat);
    void Stop();
    void Clear();
    CAnim2D* GetCurAnim() { return m_pCurAnim; }
};


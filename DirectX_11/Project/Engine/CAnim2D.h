#pragma once
#include "CEntity.h"

#include "ptr.h"
#include "CTexture.h"

class CAnimator2D;

class CAnim2D :
    public CEntity
{
private:
    CAnimator2D*        m_pOwner;
    vector<tAnim2DFrm>  m_vecFrm;
    Vec2                m_vBackSize;
    Ptr<CTexture>       m_AtlasTex;

    int                 m_iCurFrm;
    float               m_fAccTime;

    bool                m_bFinish;

    wstring             m_strFilePath;
    
public:
    CAnimator2D* GetAnimator() const { return m_pOwner; }
    vector<tAnim2DFrm>& GetFrames() { return m_vecFrm; }

    const tAnim2DFrm& GetCurFrame() const { return m_vecFrm[m_iCurFrm]; }
    int GetCurFrameIdx() const { return m_iCurFrm; }

    Vec2 GetBackSize() const { return m_vBackSize; }
    void SetBackSize(Vec2 _vBackSize) { m_vBackSize = _vBackSize; }

    Ptr<CTexture> GetAtlasTex() const { return m_AtlasTex; }

    bool IsFinish() const { return m_bFinish; }

    const wstring& GetFilePath() const { return m_strFilePath; }

public:
    void finaltick();
    void Create(const wstring& _strAnimName, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, Vec2 _vBackSize, int _FrameCount, int _FPS);
    void Create(const wstring& _strAnimName, Ptr<CTexture> _AtlasTex, vector<tAnim2DFrm>& _vecFrm, Vec2 _vBackSize);
    void Change(vector<tAnim2DFrm>& _vecFrm, Vec2 _vBackSize);

    CLONE(CAnim2D);

public:
    CAnim2D();
    ~CAnim2D();

    friend class CAnimator2D;
public:
    void Reset() { m_iCurFrm = 0; m_bFinish = false; m_fAccTime = 0.f; }
    void Save(const wstring& _strRelativePath);
    void Load(const wstring& _strRelativePath);
    const wstring& GetCurFrameNotify() const { return m_vecFrm[m_iCurFrm].Notify; }
    void SetCurFrameByNotify(const wstring& _strNotify);
};


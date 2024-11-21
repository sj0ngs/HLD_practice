#include "pch.h"
#include "CAnimator2D.h"

#include "CAnim2D.h"
#include "CMeshRender.h"

CAnimator2D::CAnimator2D()	:
	CComponent(COMPONENT_TYPE::ANIMATOR2D),
	m_pCurAnim(nullptr),
	m_bRepeat(true),
	m_bStop(false)
{
}

CAnimator2D::CAnimator2D(const CAnimator2D& _Origin)	:
	CComponent(_Origin),
	m_pCurAnim(nullptr),
	m_bRepeat(_Origin.m_bRepeat),
	m_bStop(_Origin.m_bStop)
{
	for (const auto& pair : _Origin.m_mapAnim)
	{
		CAnim2D* pAnim = pair.second->Clone();
		pAnim->Reset();
		pAnim->m_pOwner = this;
		m_mapAnim.insert(make_pair(pair.first, pAnim));
	}

	if (nullptr != _Origin.m_pCurAnim)
	{
		Play(_Origin.m_pCurAnim->GetName(), m_bRepeat);
	}
}

CAnimator2D::~CAnimator2D()
{
	Safe_Del_Map(m_mapAnim);
}

void CAnimator2D::finaltick()
{
	if (nullptr == m_pCurAnim)
		return;

	if (m_bRepeat && m_pCurAnim->IsFinish())
	{
		m_pCurAnim->Reset();
	}

	if(false == m_bStop)
		m_pCurAnim->finaltick();
}

void CAnimator2D::UpdateData()
{	
	Ptr<CMaterial> pMtrl = MeshRender()->GetMaterial();
	
	if (!m_pCurAnim)
		return;

	const tAnim2DFrm& frm = m_pCurAnim->GetCurFrame();

	Vec2 vBackSize = m_pCurAnim->GetBackSize();

	int iAnimUse = 1;

	pMtrl->SetScalarParam(INT_0, &iAnimUse);

	pMtrl->SetScalarParam(VEC2_0, &frm.LeftTopUV);
	pMtrl->SetScalarParam(VEC2_1, &frm.SliceUV);
	pMtrl->SetScalarParam(VEC2_2, &frm.Offset);
	pMtrl->SetScalarParam(VEC2_3, &vBackSize);

	pMtrl->SetTexParam(TEX_0, m_pCurAnim->GetAtlasTex());
}

void CAnimator2D::CreateAnim(const wstring& _strAnimName, Ptr<CTexture> _AtlasTex,
						Vec2 _vLeftTop, Vec2 _vSlice, Vec2 _vBackSize, int _FrameCount, int _FPS)
{
	CAnim2D* pAnim = new CAnim2D;
	pAnim->Create(_strAnimName, _AtlasTex, _vLeftTop, _vSlice, _vBackSize, _FrameCount, _FPS);

	pAnim->m_pOwner = this;
	m_mapAnim.insert(make_pair(_strAnimName, pAnim));
}

void CAnimator2D::LoadAnim(const wstring& _strFilePath)
{
	if (FindAnim(_strFilePath))
	{
		MessageBoxA(nullptr, "이미 존재하는 애니메이션 입니다", "애니메이션 중복 로드", MB_OK);
		return;
	}

	CAnim2D* pAnim = new CAnim2D;

	pAnim->Load(_strFilePath);

	pAnim->m_pOwner = this;
	m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));

	if (nullptr == m_pCurAnim)
	{
		m_pCurAnim = pAnim;
	}
}

CAnim2D* CAnimator2D::FindAnim(const wstring& _strAnimName)
{
	if (_strAnimName.empty())
		return nullptr;

	map<wstring, CAnim2D*>::iterator iter = m_mapAnim.find(_strAnimName);

	if (iter == m_mapAnim.end())
	{
		MessageBox(nullptr, L"해당 애니메이션 존재하지 않음", _strAnimName.data(), MB_OK);
		return nullptr;
	}

	return iter->second;
}

void CAnimator2D::DeleteAnim(const wstring& _strAnimName)
{
	map<wstring, CAnim2D*>::iterator iter = m_mapAnim.find(_strAnimName);

	if (iter == m_mapAnim.end())
	{
		MessageBox(nullptr, L"해당 애니메이션 존재하지 않음", _strAnimName.data(), MB_OK);
		return;
	}
	if (iter->second == m_pCurAnim)
	{
		m_pCurAnim = nullptr;
	}
	delete iter->second;

	m_mapAnim.erase(iter);
}

void CAnimator2D::Play(const wstring& _strAnimName, bool _bRepeat)
{
	CAnim2D* pAnim = FindAnim(_strAnimName);
	assert(pAnim);

	m_bStop = false;
	m_bRepeat = _bRepeat;
	if (m_pCurAnim == pAnim)
		return;

	m_pCurAnim = pAnim;
	m_pCurAnim->Reset();
}

void CAnimator2D::Stop()
{
	m_bStop = true;
	if (m_pCurAnim)
		m_pCurAnim->Reset();
}

void CAnimator2D::Clear()
{
	Ptr<CMaterial> pMtrl = MeshRender()->GetMaterial();

	int iAnimUse = 0;
	pMtrl->SetScalarParam(INT_0, &iAnimUse);
	if(m_pCurAnim)
		pMtrl->SetTexParam(TEX_0, m_pCurAnim->GetAtlasTex());
}

void CAnimator2D::SaveToLevelFile(FILE* _pFile)
{
	fwrite(&m_bRepeat, sizeof(bool), 1, _pFile);

	size_t AnimCount = m_mapAnim.size();
	fwrite(&AnimCount, sizeof(size_t), 1, _pFile);

	for (const auto& pair : m_mapAnim)
	{
		SaveWString(pair.second->GetFilePath(), _pFile);
	}

	wstring strCurAnimName;
	if (nullptr != m_pCurAnim)
	{
		strCurAnimName = m_pCurAnim->GetName();
	}
	SaveWString(strCurAnimName, _pFile);
}

void CAnimator2D::LoadFromLevelFile(FILE* _pFile)
{
	fread(&m_bRepeat, sizeof(bool), 1, _pFile);

	size_t AnimCount = m_mapAnim.size();
	fread(&AnimCount, sizeof(size_t), 1, _pFile);
	
	for (size_t i = 0; i < AnimCount; i++)
	{
		wstring strFilePath;
		LoadWString(strFilePath, _pFile);
		CAnim2D* pNewAnim = new CAnim2D;
		pNewAnim->Load(strFilePath);
		m_mapAnim.insert(make_pair(pNewAnim->GetName(), pNewAnim));
	}

	wstring strCurAnimName;
	LoadWString(strCurAnimName, _pFile);

	m_pCurAnim = FindAnim(strCurAnimName);
}

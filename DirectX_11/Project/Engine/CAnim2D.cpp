#include "pch.h"
#include "CAnim2D.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CPathMgr.h"
#include "CTimeMgr.h"
#include "CResMgr.h"

CAnim2D::CAnim2D()	:
	m_pOwner(nullptr),
	m_iCurFrm(0),
	m_fAccTime(0.f),
	m_bFinish(false)
{
}

CAnim2D::~CAnim2D()
{
}

void CAnim2D::finaltick()	
{
	if (m_bFinish)
		return;

	m_fAccTime += DT;

	if (m_fAccTime > m_vecFrm[m_iCurFrm].fDuration)
	{
		m_fAccTime = m_fAccTime - m_vecFrm[m_iCurFrm].fDuration;
		
		if (m_vecFrm.size() <= ++m_iCurFrm)
		{
			m_iCurFrm = (int)m_vecFrm.size() - 1;
			m_bFinish = true;
		}
	}
}

void CAnim2D::Create(const wstring& _strAnimName, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, Vec2 _vBackSize, int _FrameCount, int _FPS)
{
	assert(_AtlasTex.Get());

	SetName(_strAnimName);

	m_AtlasTex = _AtlasTex;

	Vec2 vResolution = Vec2((float)m_AtlasTex->Width(), (float)m_AtlasTex->Height());

	m_vBackSize = _vBackSize / vResolution;

	for (int i = 0; i < _FrameCount; i++)
	{
		tAnim2DFrm frm = {};

		frm.fDuration = 1.f / (float)_FPS;
		frm.LeftTopUV = Vec2(_vLeftTop.x + i * _vSlice.x, _vLeftTop.y) / vResolution;
		frm.SliceUV = _vSlice / vResolution;

		m_vecFrm.push_back(frm);
	}
}

void CAnim2D::Create(const wstring& _strAnimName, Ptr<CTexture> _AtlasTex, vector<tAnim2DFrm>& _vecFrm, Vec2 _vBackSize)
{
	assert(_AtlasTex.Get());

	m_AtlasTex = _AtlasTex;
	SetName(_strAnimName);
	m_vecFrm = _vecFrm;
	m_vBackSize = _vBackSize;
}

void CAnim2D::Change(vector<tAnim2DFrm>& _vecFrm, Vec2 _vBackSize)
{
	m_vecFrm = _vecFrm;
	m_vBackSize = _vBackSize;
}

void CAnim2D::Save(const wstring& _strRelativePath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	// 파일 입출력
	FILE* pFile = nullptr;
	errno_t iErrNo = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Animation Save 실패, Error Number = %d", iErrNo);
		MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
		return;
	}

	fwprintf_s(pFile, L"\n");
	// 애니메이션 이름 저장
	fwprintf_s(pFile, L"[ANIMATION_NAME]\n");
	fwprintf_s(pFile, GetName().c_str());
	fwprintf_s(pFile, L"\n\n");

	// 키값 저장
	fwprintf_s(pFile, L"[ATLAS_KEY]\n");
	fwprintf_s(pFile, m_AtlasTex->GetKey().c_str());
	fwprintf_s(pFile, L"\n\n");

	// 상대 경로 저장
	fwprintf_s(pFile, L"[ATLAS_PATH]\n");
	fwprintf_s(pFile, m_AtlasTex->GetRelativePath().c_str());
	fwprintf_s(pFile, L"\n\n");

	// BackSize 정보 저장
	fwprintf_s(pFile, L"[BACK_SIZE]\n");
	fwprintf_s(pFile, L"%f %f\n", m_vBackSize.x, m_vBackSize.y);
	fwprintf_s(pFile, L"\n");

	// 프레임 정보 저장
	fwprintf_s(pFile, L"[FRAME_COUNT]\n");
	wchar_t szNum[50] = {};
	size_t iFrmCount = m_vecFrm.size();
	_itow_s((int)iFrmCount, szNum, 50, 10);
	fwprintf_s(pFile, szNum);
	fwprintf_s(pFile, L"\n\n");

	for (size_t i = 0; i < iFrmCount; i++)
	{
		fwprintf_s(pFile, L"[%zd_FRAME]\n", i);

		// Nofity
		if (!m_vecFrm[i].Notify.empty())
		{
			fwprintf_s(pFile, L"[NOTIFY]\n");
			fwprintf_s(pFile, m_vecFrm[i].Notify.c_str());
		}

		// LEFT_TOP
		fwprintf_s(pFile, L"[LEFT_TOP]\n");
		fwprintf_s(pFile, L"%f %f\n", m_vecFrm[i].LeftTopUV.x, m_vecFrm[i].LeftTopUV.y);

		// SIZE
		fwprintf_s(pFile, L"[SIZE]\n");
		fwprintf_s(pFile, L"%f %f\n", m_vecFrm[i].SliceUV.x, m_vecFrm[i].SliceUV.y);

		// OFFSET
		fwprintf_s(pFile, L"[OFFSET]\n");
		fwprintf_s(pFile, L"%f %1f\n", m_vecFrm[i].Offset.x, m_vecFrm[i].Offset.y);

		// DURATION
		fwprintf_s(pFile, L"[DURATION]\n");
		fwprintf_s(pFile, L"%.5f\n", m_vecFrm[i].fDuration);

		fwprintf_s(pFile, L"\n");
	}

	fclose(pFile);
}

void CAnim2D::Load(const wstring& _strRelativePath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	m_strFilePath = _strRelativePath;

	// 파일 입출력
	FILE* pFile = nullptr;
	errno_t iErrNo = _wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Animation Load 실패, Error Number = %d", iErrNo);
		MessageBox(nullptr, szStr, L"파일 불러오기 실패", MB_OK);
		return;
	}

	wstring strAtlasKey;
	wstring strAtlasPath;
	size_t iFrmCount = 0;

	while (true)
	{
		wchar_t szBuffer[256] = {};
		fwscanf_s(pFile, L"%s", szBuffer, 256);

		if (!wcscmp(szBuffer, L"[ANIMATION_NAME]"))
		{
			fwscanf_s(pFile, L"%s", szBuffer, 256);
			SetName(szBuffer);
		}
		else if (!wcscmp(szBuffer, L"[ATLAS_KEY]"))
		{
			fwscanf_s(pFile, L"%s", szBuffer, 256);
			strAtlasKey = szBuffer;
		}
		else if (!wcscmp(szBuffer, L"[ATLAS_PATH]"))
		{
			fwscanf_s(pFile, L"%s", szBuffer, 256);
			strAtlasPath = szBuffer;
		}
		else if (!wcscmp(szBuffer, L"[BACK_SIZE]"))
		{
			fwscanf_s(pFile, L"%f %f\n", &m_vBackSize.x, &m_vBackSize.y);
		}
		else if (!wcscmp(szBuffer, L"[FRAME_COUNT]"))
		{
			fwscanf_s(pFile, L"%zd", &iFrmCount);

			for (size_t i = 0; i < iFrmCount; i++)
			{
				tAnim2DFrm frm = {};
				while (true)
				{
					wchar_t szBuffer[256] = {};
					fwscanf_s(pFile, L"%s", szBuffer, 256);

					if (!wcscmp(szBuffer, L"[LEFT_TOP]"))
					{
						fwscanf_s(pFile, L"%f %f", &frm.LeftTopUV.x, &frm.LeftTopUV.y);
					}
					else if (!wcscmp(szBuffer, L"[SIZE]"))
					{
						fwscanf_s(pFile, L"%f %f", &frm.SliceUV.x, &frm.SliceUV.y);
					}
					else if (!wcscmp(szBuffer, L"[OFFSET]"))
					{
						fwscanf_s(pFile, L"%f %f", &frm.Offset.x, &frm.Offset.y);
					}
					else if (!wcscmp(szBuffer, L"[DURATION]"))
					{
						fwscanf_s(pFile, L"%f", &frm.fDuration);
						break;
					}
					else if (!wcscmp(szBuffer, L"[NOTIFY]"))
					{
						fwscanf_s(pFile, L"%s", szBuffer, 256);
						frm.Notify = szBuffer;
						//break;
					}
				}
				m_vecFrm.push_back(frm);
			}
			break;
		}
	}

	// 불러온 정보로 아틀라스 텍스쳐 생성해주기
	m_AtlasTex = CResMgr::GetInst()->LoadRes<CTexture>(strAtlasKey, strAtlasPath);

	fclose(pFile);
}

void CAnim2D::SetCurFrameByNotify(const wstring& _strNotify)
{
	Reset();

	int idx = 0;
	for (const auto& frm : m_vecFrm)
	{
		if(frm.Notify == _strNotify)
			break;
		idx++;
	}

	m_iCurFrm = idx;
}

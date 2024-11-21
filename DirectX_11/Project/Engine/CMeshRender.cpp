#include "pch.h"
#include "CMeshRender.h"

#include "CResMgr.h"

#include "CTransform.h"
#include "CAnimator2D.h"

CMeshRender::CMeshRender()	:
	CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::finaltick()
{
}

void CMeshRender::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial())
		return;

	// Transform ¿¡  UpdateData ¿äÃ»
	Transform()->UpdateData();

	if (nullptr != Animator2D())
	{
		Animator2D()->UpdateData();
	}

	GetMaterial()->UpdateData();

	GetMesh()->render();

	if (nullptr != Animator2D())
	{
		Animator2D()->Clear();
	}
}

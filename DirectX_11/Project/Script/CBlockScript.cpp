#include "pch.h"
#include "CBlockScript.h"

#include "CProjectileScript.h"

CBlockScript::CBlockScript()	:
	CScript(SCRIPT_TYPE::BLOCKSCRIPT),
	m_fPush(0.f)
{
}

void CBlockScript::begin()
{
}

void CBlockScript::tick()
{
}

void CBlockScript::BeginOverlap(CCollider2D* _Other)
{
	BlockObj(_Other);
}

void CBlockScript::OnOverlap(CCollider2D* _Other)
{
	BlockObj(_Other);
}

void CBlockScript::EndOverlap(CCollider2D* _Other)
{
}

CBlockScript::~CBlockScript()
{
}

void CBlockScript::BlockObj(CCollider2D* _Other)
{
	if (_Other->GetOwner()->GetScript<CProjectileScript>())
		return;

	Vec2 vColPos = Vec2(Transform()->GetRelativePos().x, Transform()->GetRelativePos().y) + Collider2D()->GetOffsetPos();
	Vec2 vOtherPos = Vec2(_Other->Transform()->GetRelativePos().x, _Other->Transform()->GetRelativePos().y) + _Other->GetOffsetPos();

	Vec2 vColScale = Vec2(Transform()->GetRelativeScale().x, Transform()->GetRelativeScale().y);
	if (Collider2D()->IsAbsolute())
		vColScale = Collider2D()->GetOffsetScale();
	else
		vColScale = vColScale * Collider2D()->GetOffsetScale();

	Vec2 vOtherScale = Vec2(_Other->Transform()->GetRelativeScale().x, _Other->Transform()->GetRelativeScale().y);
	if (_Other->Collider2D()->IsAbsolute())
		vOtherScale = _Other->Collider2D()->GetOffsetScale();
	else
		vOtherScale = vColScale * _Other->GetOffsetScale();

	Vec3 vPush;
	switch (_Other->GetCollider2DType())
	{
	case COLLIDER2D_TYPE::RECT:
	{
		Vec2 vDist = vColPos - vOtherPos;
		Vec2 vBox = (vColScale + vOtherScale) / 2.f;

		float fXOverlap = vBox.x - fabs(vDist.x);
		float fYOverlap = vBox.y - fabs(vDist.y);

		// 좌우로 들어옴
		if (fXOverlap <= fYOverlap)
		{
			// 오른쪽에서
			if (vDist.x < 0.f)
				vPush.x = fXOverlap + m_fPush;
			// 왼쪽에서 들어옴
			else
				vPush.x = -(fXOverlap + m_fPush);
		}
		// 상하로 들어옴
		else
		{
			// 아래에서
			if (vDist.y > 0.f)
				vPush.y = -(fYOverlap + m_fPush);
			// 위에서 들어옴
			else
				vPush.y = (fYOverlap + m_fPush);
		}
	}
	break;
	case COLLIDER2D_TYPE::CIRCLE:
	{
	}
	break;
	}

	_Other->Transform()->SetRelativePos(_Other->Transform()->GetRelativePos() + vPush);
}

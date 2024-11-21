#include "pch.h"
#include "CEffectAnimScript.h"

#include <Engine\CAnim2D.h>

CEffectAnimScript::CEffectAnimScript()	:
	CScript(SCRIPT_TYPE::EFFECTANIMSCRIPT),
	m_bRepeat(false)
{
}

CEffectAnimScript::~CEffectAnimScript()
{
}

void CEffectAnimScript::begin()
{
	wstring strAnimName = Animator2D()->GetCurAnim()->GetName();
	Animator2D()->Play(strAnimName, m_bRepeat);
}

void CEffectAnimScript::tick()
{
	if (!m_bRepeat && Animator2D()->GetCurAnim()->IsFinish())
	{
		Destory();
	}
}


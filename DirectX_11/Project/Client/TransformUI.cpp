#include "pch.h"
#include "TransformUI.h"

#include <Engine\CTransform.h>

TransformUI::TransformUI()	:
	ComponentUI("##Transform", COMPONENT_TYPE::TRANSFORM)
{
	SetName("Transform");
}

TransformUI::~TransformUI()
{
}

int TransformUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	CTransform* pTransform = GetTarget()->Transform();

	Vec3 vPos = pTransform->GetRelativePos();
	Vec3 vScale = pTransform->GetRelativeScale();
	Vec3 vRot = pTransform->GetRelativeRot();
	vRot *= (180.f / XM_PI);

	ImGui::Text("Absolute   ");
	ImGui::SameLine();
	
	bool bAbsolute = pTransform->IsAbsolute();
	ImGui::Checkbox("##Absolute", & bAbsolute);
	pTransform->SetAbsolute(bAbsolute);

	ImGui::Text("Position   ");
	ImGui::SameLine();
	ImGui::DragFloat3("##Relative Position", vPos);

	ImGui::Text("Scale      ");
	ImGui::SameLine();
	ImGui::DragFloat3("##Relative Scale", vScale);

	ImGui::Text("Rotation   ");
	ImGui::SameLine();
	ImGui::DragFloat3("##Relative Rotation", vRot);

	pTransform->SetRelativePos(vPos);
	pTransform->SetRelativeScale(vScale);

	vRot *= (XM_PI / 180.f);
	pTransform->SetRelativeRot(vRot);

	return TRUE;
}
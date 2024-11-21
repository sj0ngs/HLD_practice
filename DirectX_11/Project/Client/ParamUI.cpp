#include "pch.h"
#include "ParamUI.h"

#include <Engine\CResMgr.h>
#include "ImGuiMgr.h"
#include "ListUI.h"

#include "TreeUI.h"

UINT ParamUI::g_NextID = 0;

int ParamUI::Param_Int(const string& _strDesc, int* _pData, bool _bDrag)
{
	ImGui::Text(_strDesc.c_str());
	ImGui::SameLine(100.f);

	string strParam = "##Param_Int";
	strParam = GetNextName(strParam);

	if (_bDrag)
	{
		if (ImGui::DragInt(strParam.c_str(), _pData))
		{
			return 1;
		}
	}
	else
	{
		if (ImGui::InputInt(strParam.c_str(), _pData))
		{
			return 1;
		}
	}

	return 0;
}

int ParamUI::Param_Float(const string& _strDesc, float* _pData, bool _bDrag)
{
	ImGui::Text(_strDesc.c_str());
	ImGui::SameLine(100.f);

	string strParam = "##Param_Float";
	strParam = GetNextName(strParam);

	if (_bDrag)
	{
		if (ImGui::DragFloat(strParam.c_str(), _pData))
		{
			return 1;
		}
	}
	else
	{
		if (ImGui::InputFloat(strParam.c_str(), _pData))
		{
			return 1;
		}
	}

	return 0;
}

int ParamUI::Param_Vec2(const string& _strDesc, Vec2* _pData, bool _bDrag)
{
	ImGui::Text(_strDesc.c_str());
	ImGui::SameLine(100.f);
	string strParam = "##Param_Vec2";
	strParam = GetNextName(strParam);

	float* arrfloat[2] = { &_pData->x, &_pData->y };
	if (_bDrag)
	{
		if (ImGui::DragFloat2(strParam.c_str(), *arrfloat))
		{
			return 1;
		}
	}
	else
	{
		if (ImGui::InputFloat2(strParam.c_str(), *arrfloat))
		{
			return 1;
		}
	}

	return 0;
}

int ParamUI::Param_Vec4(const string& _strDesc, Vec4* _pData, bool _bDrag)
{
	ImGui::Text(_strDesc.c_str());
	ImGui::SameLine(100.f);

	string strParam = "##Param_Vec4";
	strParam = GetNextName(strParam);

	if (_bDrag)
	{
		if (ImGui::DragFloat4(strParam.c_str(), *_pData))
		{
			return 1;
		}
	}
	else
	{
		if (ImGui::InputFloat4(strParam.c_str(), *_pData))
		{
			return 1;
		}
	}

	return 0;
}

int ParamUI::Param_Mat(const string& _strDesc, Matrix* _pData, bool _bDrag)
{
	int iResult = 0;

	ImGui::Text(_strDesc.c_str());

	string strParam = "##Param_Matrix";
	string strID = GetNextName(strParam);

	float* arrRow1[4] = { &_pData->_11, &_pData->_12, &_pData->_13, &_pData->_14 };
	float* arrRow2[4] = { &_pData->_21, &_pData->_22, &_pData->_23, &_pData->_24 };
	float* arrRow3[4] = { &_pData->_31, &_pData->_32, &_pData->_33, &_pData->_34 };
	float* arrRow4[4] = { &_pData->_41, &_pData->_42, &_pData->_43, &_pData->_44 };


	if (_bDrag)
	{	
		if (ImGui::DragFloat4(strID.c_str(), *arrRow1))
		{
			iResult = 1;
		}
		strID = GetNextName(strParam);
		if (ImGui::DragFloat4(strID.c_str(), *arrRow2))
		{
			iResult = 1;
		}
		strID = GetNextName(strParam);
		if (ImGui::DragFloat4(strID.c_str(), *arrRow3))
		{
			iResult = 1;
		}
		strID = GetNextName(strParam);
		if (ImGui::DragFloat4(strID.c_str(), *arrRow4))
		{
			iResult = 1;
		}
	}
	else
	{
		if (ImGui::InputFloat4(strID.c_str(), *arrRow1))
		{
			iResult = 1;
		}
		strID = GetNextName(strParam);
		if (ImGui::InputFloat4(strID.c_str(), *arrRow2))
		{
			iResult = 1;
		}
		strID = GetNextName(strParam);
		if (ImGui::InputFloat4(strID.c_str(), *arrRow3))
		{
			iResult = 1;
		}
		strID = GetNextName(strParam);
		if (ImGui::InputFloat4(strID.c_str(), *arrRow4))
		{
			iResult = 1;
		}
	}

	return iResult;
}

int ParamUI::Param_Tex(const string& _strDesc, Ptr<CTexture>& _Tex, UI* _pUI, UI_DELEGATE_1 _Func)
{
	ImGui::Text(_strDesc.c_str());

	string strParam = "##Param_Image";
	string strID = GetNextName(strParam);

	if (nullptr == _Tex.Get())
	{
		ImGui::Image(nullptr, ImVec2(150.f, 150.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f),
			ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f));
	}
	else
	{
		float fAspect = (float)_Tex->Width() / (float)_Tex->Height();
		ImGui::Image(_Tex->GetSRV().Get(), ImVec2(150, 150.f / fAspect), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f),
			ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f));
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
		if (pPayLoad)
		{
			TreeNode* pNode = (TreeNode*)pPayLoad->Data;
			CRes* pRes = (CRes*)pNode->GetData();
			if (RES_TYPE::TEXTURE == pRes->GetType())
			{
				_Tex = (CTexture*)pRes;
			}
		}
		ImGui::EndDragDropTarget();

		return 0;
	}

	ImGui::SameLine();
	string strBtnName = GetNextName("##TexSelectBtn");
	if (ImGui::Button(strBtnName.c_str(), ImVec2(19, 19)))
	{
		const map<wstring, Ptr<CRes>>& mapTex = CResMgr::GetInst()->GetResources(RES_TYPE::TEXTURE);

		ListUI* pList = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
		pList->Reset("Texture List", ImVec2(300.f, 500.f));

		for (const auto& pair : mapTex)
		{
			pList->AddItem(string(pair.first.begin(), pair.first.end()));
		}
		pList->AddDynamic_Select(_pUI, _Func);

		return 1;
	}

	return 0;
}

string ParamUI::GetNextName(const string& _strName)
{
	string strName = _strName;
	strName += "##";
	strName += std::to_string(g_NextID++);

	return strName;
}

#include "pch.h"
#include "OutlinerUI.h"

#include <Engine\CEventMgr.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>
#include <Engine\CPrefab.h>
#include <Engine\CResMgr.h>

#include "InspectorUI.h"
#include "TreeUI.h"

OutlinerUI::OutlinerUI()    :
    UI("##Outliner"),
    m_Tree(nullptr),
    m_dwReserveData(0)
{
    SetName("Outliner");

    m_Tree = new TreeUI("Outliner");
    m_Tree->SetActive(true);
    m_Tree->ShowRoot(false);
    m_Tree->UseDragDrop("GameObject");

    m_Tree->AddDynamic_Select(this, (UI_DELEGATE_1)&OutlinerUI::SetTargetToInspector);
    m_Tree->AddDynamic_DragDrop(this, (UI_DELEGATE_2)&OutlinerUI::DragDrop);
    m_Tree->AddDynamic_Edit(this, (UI_DELEGATE_2)&OutlinerUI::Edit);
    m_Tree->AddDynamic_Rename(this, (UI_DELEGATE_2)&OutlinerUI::RenameObject);

    AddChildUI(m_Tree);
}

OutlinerUI::~OutlinerUI()
{
}

void OutlinerUI::tick()
{
    if (CEventMgr::GetInst()->IsLevelChanged())
    {
        ResetOutliner();

        if (0 != m_dwReserveData)
        {
            m_Tree->SetSelectedNode(m_dwReserveData);
        }
    }
}

int OutlinerUI::render_update()
{
    return 0;
}

void OutlinerUI::ResetOutliner()
{
    m_Tree->Clear();
    m_Tree->AddItem("Root", 0);

    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

    for (UINT i = 0; i < (UINT)MAX_LAYER; i++)
    {
        CLayer* pLayer = pCurLevel->GetLayer(i);

        const vector<CGameObject*>& vecParentObj = pLayer->GetParentObject();

        for (size_t i = 0; i < vecParentObj.size(); i++)
        {
            AddGameObject(vecParentObj[i], nullptr);
        }
    }
}

void OutlinerUI::SetTargetToInspector(DWORD_PTR _SelectedNode)
{
    TreeNode* pSelectedNode = (TreeNode*)_SelectedNode;
    CGameObject* pSelectedObject = (CGameObject*)pSelectedNode->GetData();

    InspectorUI* pInspector = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");
    pInspector->SetTargetObject(pSelectedObject);
}

void OutlinerUI::AddGameObject(CGameObject* _pObj, TreeNode* _pParentNode)
{
    TreeNode* pNode = m_Tree->AddItem(string(_pObj->GetName().begin(), _pObj->GetName().end()), 
                                    (DWORD_PTR)_pObj, _pParentNode);

    const vector<CGameObject*>& vecChild = _pObj->GetChild();
    for (size_t i = 0; i < vecChild.size(); i++)
    {
        AddGameObject(vecChild[i], pNode);
    }
}

void OutlinerUI::DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode)
{
    TreeNode* pDragNode = (TreeNode*)_DragNode;
    TreeNode* pDropNode = (TreeNode*)_DropNode;

    CGameObject* pDragObj = (CGameObject*)pDragNode->GetData();
    CGameObject* pDropObj = nullptr;

    if (nullptr != pDropNode)
    {
        pDropObj = (CGameObject*)pDropNode->GetData();
    }

    if (nullptr != pDropObj)
    {
        if (pDropObj->IsAncestor(pDragObj) || pDropObj == pDragObj->GetParent())
            return;
    }
    
    AddChildObject(pDropObj, pDragObj);
}

void OutlinerUI::Edit(DWORD_PTR _pEditNode, DWORD_PTR _eEditType)
{
    TreeNode* pEditNode = (TreeNode*)_pEditNode;
    CGameObject* pEditObj = (CGameObject*)pEditNode->GetData();

    if (nullptr == pEditObj)
        return;

    EDIT_TREE_NODE eType = (EDIT_TREE_NODE)_eEditType;

    switch (eType)
    {
    case EDIT_TREE_NODE::TREE_DELETE:
    {
        DestoryObject(pEditObj);
    }
        break;
    case EDIT_TREE_NODE::TREE_COPY:
    {
        wstring strName = pEditObj->GetName();
        strName += L"_COPY";
        CGameObject* pCloneObj = pEditObj->Clone();
        pCloneObj->SetName(strName);
        SpawnGameObject(pCloneObj, pEditObj->Transform()->GetRelativePos(), pEditObj->GetLayerIndex());
        SetReserveData((DWORD_PTR)pCloneObj);
    }
        break;
    case EDIT_TREE_NODE::MAKE_PREF:
    {
        Ptr<CPrefab> pref = new CPrefab();
        CGameObject* clone = pEditObj->Clone();
        DestoryObject(clone);
        pref->RegisterProtoObject(clone);
        pref->Save(L"prefab");
    }
        break;
    }
}

void OutlinerUI::RenameObject(DWORD_PTR _pRenameNode, DWORD_PTR _strRename)
{
    TreeNode* pRenameNode = (TreeNode*)_pRenameNode;
    CGameObject* pRenameObj = (CGameObject*)pRenameNode->GetData();

    if (nullptr == pRenameObj)
        return;

    char* szBuff = (char*)_strRename;
    string strBuff = szBuff;
    wstring strName = wstring(strBuff.begin(), strBuff.end());

    pRenameObj->SetName(strName);

    ResetOutliner();
}

CGameObject* OutlinerUI::GetSelectedObject() const
{
    TreeNode* pSelectedNode = m_Tree->GetSelectedNode();

    if (nullptr == pSelectedNode)
        return nullptr;
    
    return (CGameObject*)pSelectedNode->GetData();
}

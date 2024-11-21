#pragma once
#include "UI.h"

class TreeUI;
class TreeNode;

class OutlinerUI :
    public UI
{
private:
    TreeUI* m_Tree;

    DWORD_PTR m_dwReserveData;

public:
    virtual void tick() override;
    virtual int render_update() override;

public:
    void ResetOutliner();
    void SetTargetToInspector(DWORD_PTR _SelectedNode);     // TreeNode Address
    CGameObject* GetSelectedObject() const;

    void SetReserveData(DWORD_PTR _Data) { m_dwReserveData = _Data; }

private:
    void AddGameObject(CGameObject* _pObj, TreeNode* _pParentNode);
    void DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode);
    void Edit(DWORD_PTR _pEditNode, DWORD_PTR _eEditType);
    void RenameObject(DWORD_PTR _pRenameNode, DWORD_PTR _strRename);

public:
    OutlinerUI();
    ~OutlinerUI();
};


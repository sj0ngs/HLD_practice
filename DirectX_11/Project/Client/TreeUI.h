#pragma once
#include "UI.h"

enum class EDIT_TREE_NODE
{
    TREE_DELETE,
    TREE_COPY,
    MAKE_PREF
};


class TreeUI;
class TreeNode
{
private:
    TreeUI*             m_Owner;
    TreeNode*           m_ParentNode;
    vector<TreeNode*>   m_vecChildNode;

    string              m_strName;
    UINT                m_ID;
    DWORD_PTR           m_Data;

    bool                m_bCategoryNode;
    bool                m_bHighlight;
    bool                m_bEditName;

public:
    void SetCategoryNode(bool _bSet) { m_bCategoryNode = _bSet; }

    const string& GetName() const { return m_strName; }
    DWORD_PTR GetData() const { return m_Data; }

private:
    void render_update();

public:
    TreeNode();
    ~TreeNode();

    friend class TreeUI;
};

class TreeUI :
    public UI
{
private:
    TreeNode*       m_RootNode;
    UINT            m_iNextId;
    bool            m_bShowRoot;

    TreeNode*       m_SelectedNode;
    TreeNode*       m_LbtnDownNode;
    TreeNode*       m_DragNode;
    TreeNode*       m_DropNode;

    DWORD_PTR       m_dwPrevSelected;

    UI*             m_SelectInst;
    UI_DELEGATE_1   m_SelectFunc;

    UI*             m_DragDropInst;
    UI_DELEGATE_2   m_DragDropFunc;

    UI*             m_EditInst;
    UI_DELEGATE_2   m_EditFunc;

    UI*             m_RenameInst;
    UI_DELEGATE_2   m_RenameFunc;

    UI*             m_DoubleClickInst;
    UI_DELEGATE_1   m_DoubleClickFunc;

    string          m_strPayLoadID;
    bool            m_bUseDragDrop;

    bool            m_bEditable;

public:
    void ShowRoot(bool _bShow) { m_bShowRoot = _bShow; };
    TreeNode* AddItem(const string& _strNodeName, DWORD_PTR _Data, TreeNode* _pParent = nullptr);
    TreeNode* GetSelectedNode() const { return m_SelectedNode; }

    bool SetSelectedNode(DWORD_PTR _Data);

    void AddDynamic_Select(UI* _pUI, UI_DELEGATE_1 _MemFunc) { m_SelectInst = _pUI; m_SelectFunc = _MemFunc; }
    void AddDynamic_DragDrop(UI* _pUI, UI_DELEGATE_2 _MemFunc) { m_DragDropInst = _pUI; m_DragDropFunc = _MemFunc; }
    void AddDynamic_Edit(UI* _pUI, UI_DELEGATE_2 _MemFunc) { m_EditInst = _pUI; m_EditFunc = _MemFunc; }
    void AddDynamic_Rename(UI* _pUI, UI_DELEGATE_2 _MemFunc) { m_RenameInst = _pUI; m_RenameFunc = _MemFunc; }
    void AddDynamic_DoubleClick(UI* _pUI, UI_DELEGATE_1 _MemFunc) { m_DoubleClickInst = _pUI; m_DoubleClickFunc = _MemFunc; }

    void UseDragDrop(const string& _strPayLoadID) { m_bUseDragDrop = true; m_strPayLoadID = _strPayLoadID; }

private:
    void SetSelectedNode(TreeNode* _Node);
    void SetDragNode(TreeNode* _Node);
    void SetDropNode(TreeNode* _Node);

public:
    virtual void tick() override;
    virtual int render_update() override;

public:
    void Clear();

public:
    TreeUI(const string& _strName);
    ~TreeUI();

    friend class TreeNode;
};


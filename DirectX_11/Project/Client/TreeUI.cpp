#include "pch.h"
#include "TreeUI.h"

#include <Engine\CKeyMgr.h>

// =========
// Tree Node
// =========

TreeNode::TreeNode() :
    m_Owner(nullptr),
    m_ParentNode(nullptr),
    m_ID(0),
    m_Data(0),
    m_bCategoryNode(false),
    m_bHighlight(false),
    m_bEditName(false)
{
}

TreeNode::~TreeNode()
{
    for (size_t i = 0; i < m_vecChildNode.size(); i++)
    {
        if (nullptr != m_vecChildNode[i])
        {
            delete m_vecChildNode[i];
            m_vecChildNode[i] = nullptr;
        }
    }
    m_vecChildNode.clear();
}

void TreeNode::render_update()
{
    string strFullName = m_strName;
    strFullName += "##";
    strFullName += std::to_string(m_ID);

    UINT flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | 
                ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap;

    if (m_vecChildNode.empty())
    {
        flag |= ImGuiTreeNodeFlags_Leaf;
    }

    if (m_bHighlight || m_bCategoryNode)
    {
        flag |= ImGuiTreeNodeFlags_Selected;
    }

    bool bOpen = ImGui::TreeNodeEx((void*)(UINT_PTR)m_ID, flag, "");

    // Selected Check
    if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        m_Owner->m_LbtnDownNode = this;
    }
    else if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        m_Owner->SetSelectedNode(this);
    }
    else if (0 != m_Data && m_Owner->m_dwPrevSelected == m_Data)
    {
        m_Owner->m_dwPrevSelected = 0;
        m_Owner->SetSelectedNode(this);
    }

    // Drag & Drop
    if (m_Owner->m_bUseDragDrop)
    {
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload(m_Owner->m_strPayLoadID.c_str(), this, sizeof(TreeNode));
            ImGui::Text(m_strName.c_str());
            m_Owner->SetDragNode(this);

            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload(m_Owner->m_strPayLoadID.c_str());
            if (pPayLoad)
            {
                m_Owner->SetDropNode(this);
            }
            ImGui::EndDragDropTarget();
        }

        if (m_Owner->m_DragNode == this && ImGui::IsItemHovered() &&
            ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_Owner->SetDragNode(nullptr);
        }
    }

    // Node Editing
    if (m_Owner->m_EditInst && m_Owner->m_EditFunc)
    {
        ImGui::OpenPopupOnItemClick("##TreeNodeEdit", ImGuiPopupFlags_MouseButtonRight);
        if (ImGui::BeginPopup("##TreeNodeEdit"))
        {
            m_Owner->SetSelectedNode(this);

            if (ImGui::MenuItem("Delete##TreeNode"))
            {
                ((m_Owner->m_EditInst)->*(m_Owner->m_EditFunc))((DWORD_PTR)this, (DWORD_PTR)EDIT_TREE_NODE::TREE_DELETE);
            }

            if (ImGui::MenuItem("Copy##TreeNode"))
            {
                ((m_Owner->m_EditInst)->*(m_Owner->m_EditFunc))((DWORD_PTR)this, (DWORD_PTR)EDIT_TREE_NODE::TREE_COPY);
            }

            if (ImGui::MenuItem("Make Prefab##TreeNode"))
            {
                ((m_Owner->m_EditInst)->*(m_Owner->m_EditFunc))((DWORD_PTR)this, (DWORD_PTR)EDIT_TREE_NODE::MAKE_PREF);
            }

            if (ImGui::MenuItem("Rename##TreeNode"))
            {
                m_bEditName = true;
            }

            ImGui::EndPopup();
        }
    }

    // Node Renaming
    ImGui::SameLine();
    if (m_bEditName)
    {
        char szBuff[50] = {};
        memcpy(szBuff, m_strName.c_str(), m_strName.size());

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        bool bEndInput = ImGui::InputText("##RenameTreeNode", szBuff, 50, ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::PopStyleVar();

        if(false == ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) || bEndInput)
        {
            m_bEditName = false;

            if (nullptr != m_Owner->m_RenameInst && nullptr != m_Owner->m_RenameFunc)
            {
                ((m_Owner->m_RenameInst)->*(m_Owner->m_RenameFunc))((DWORD_PTR)this, (DWORD_PTR)szBuff);
            }
        }
    }
    else
    {
        ImGui::Text(m_strName.c_str());
    }

    // Node Child Rendering
    if (bOpen)
    {
        for (size_t i = 0; i < m_vecChildNode.size(); i++)
        {
            m_vecChildNode[i]->render_update();
        }

        ImGui::TreePop();
    }
}



// =======
// Tree UI
// =======

TreeUI::TreeUI(const string& _strName)    :
    UI("##Tree"),
    m_RootNode(nullptr),
    m_iNextId(0),
    m_bShowRoot(true),
    m_SelectedNode(nullptr),
    m_LbtnDownNode(nullptr),
    m_DragNode(nullptr),
    m_DropNode(nullptr),
    m_dwPrevSelected(0),
    m_SelectInst(nullptr),
    m_SelectFunc(nullptr),
    m_DragDropInst(nullptr),
    m_DragDropFunc(nullptr),
    m_EditInst(nullptr),
    m_EditFunc(nullptr),
    m_RenameInst(nullptr),
    m_RenameFunc(nullptr),
    m_DoubleClickInst(nullptr),
    m_DoubleClickFunc(nullptr),
    m_bUseDragDrop(false),
    m_bEditable(false)
{
    SetName(_strName);
}

TreeUI::~TreeUI()
{
    if(nullptr != m_RootNode)
        delete m_RootNode;
}

void TreeUI::tick()
{
    if (nullptr != m_EditInst && nullptr != m_EditFunc)
    {
        if (m_SelectedNode)
        {
            if (KEY_TAP(KEY::F2))
            {
                m_SelectedNode->m_bEditName = true;
            }
        }
    }
}

int TreeUI::render_update()
{
    tick();

    if (nullptr != m_RootNode)
    {
        if (m_bShowRoot)
        {
            m_RootNode->render_update();
        }
        else
        {
            for (size_t i = 0; i < m_RootNode->m_vecChildNode.size(); i++)
            {
                m_RootNode->m_vecChildNode[i]->render_update();
            }
        }
    }

    if (m_bUseDragDrop)
    {
        if ((m_DragNode && m_DropNode) ||
            (m_DragNode && ImGui::IsMouseReleased(ImGuiMouseButton_Left)))
        {
            if (m_DragDropInst && m_DragDropFunc)
            {
                (m_DragDropInst->*m_DragDropFunc)((DWORD_PTR)m_DragNode, (DWORD_PTR)m_DropNode);
            }

            m_DragNode = nullptr;
            m_DropNode = nullptr;
        }
    }

    return TRUE;
}


void TreeUI::SetSelectedNode(TreeNode* _Node)
{
    assert(this == _Node->m_Owner);

    if (nullptr != m_LbtnDownNode && m_LbtnDownNode != _Node)
        return;

    if (m_SelectedNode)
    {
        m_SelectedNode->m_bHighlight = false;
    }

    m_SelectedNode = _Node;
    m_LbtnDownNode = nullptr;

    if (m_SelectedNode)
    {
        m_SelectedNode->m_bHighlight = true;

        if (m_SelectInst && m_SelectFunc)
        {
            (m_SelectInst->*m_SelectFunc)((DWORD_PTR)m_SelectedNode);
        }
    }
}

void TreeUI::Clear()
{
    if (nullptr != m_SelectedNode)
    {
        m_dwPrevSelected = m_SelectedNode->GetData();
    }

    if (nullptr != m_RootNode)
    {
        delete m_RootNode;
        m_RootNode = nullptr;
    }
}

TreeNode* TreeUI::AddItem(const string& _strNodeName, DWORD_PTR _Data, TreeNode* _pParent)
{
    TreeNode* pNewNode = new TreeNode();

    pNewNode->m_Owner = this; 
    if (0 == _strNodeName.size())
        pNewNode->m_strName = "NO_NAME";
    else
        pNewNode->m_strName = _strNodeName;
    pNewNode->m_Data = _Data;
    pNewNode->m_ID = m_iNextId++;

    if (nullptr == m_RootNode)
    {
        assert(!_pParent);

        m_RootNode = pNewNode;
    }
    else
    {
        if (_pParent)
        {
            assert(this == _pParent->m_Owner);

            _pParent->m_vecChildNode.push_back(pNewNode);
            pNewNode->m_ParentNode = _pParent;
        }
        else
        {
            m_RootNode->m_vecChildNode.push_back(pNewNode);
            pNewNode->m_ParentNode = m_RootNode;
        }
    }
    return pNewNode;
}

bool TreeUI::SetSelectedNode(DWORD_PTR _Data)
{
    static list<TreeNode*> queue;  
    queue.clear();
    queue.push_back(m_RootNode);

    while (!queue.empty())
    {
        TreeNode* pCurNode = queue.front();
        queue.pop_front();

        for (size_t i = 0; i < pCurNode->m_vecChildNode.size(); i++)
        {
            queue.push_back(pCurNode->m_vecChildNode[i]);
        }

        if (_Data == pCurNode->GetData())
        {
            SetSelectedNode(pCurNode);
            return true;
        }
    }

    return false;
}

void TreeUI::SetDragNode(TreeNode* _Node)
{
    m_DragNode = _Node;
    m_DropNode = nullptr;
}

void TreeUI::SetDropNode(TreeNode* _Node)
{
    m_DropNode = _Node;
}

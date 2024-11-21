#pragma once
#include "UI.h"

class TreeUI;

class ContentUI :
    public UI
{
private:
    TreeUI*         m_Tree;

    vector<wstring> m_vecResPath;

public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update() override;

public:
    void ResetContent();
    void SetTargetToInspector(DWORD_PTR _SelectedNode);     // TreeNode Address
    void Reload();

private:
    void FindFileName(const wstring& _FolderPath);
    RES_TYPE GetResTypeByExt(const wstring& _strRelativePath);

public:
    ContentUI();
    ~ContentUI();
};


#include "pch.h"
#include "ComputeShaderUI.h"

#include <Engine\CComputeShader.h>

ComputeShaderUI::ComputeShaderUI()  :
    ResUI("##ComputeShader", RES_TYPE::COMPUTE_SHADER)
{
    SetName("ComputeShader");
}

ComputeShaderUI::~ComputeShaderUI()
{
}

int ComputeShaderUI::render_update()
{
    ResUI::render_update();

    Ptr<CComputeShader> pCS = (CComputeShader*)GetTargetRes().Get();

    ImGui::Text("Compute Shader");
    ImGui::SameLine(120.f);

    string strName = string(pCS->GetKey().begin(), pCS->GetKey().end());
    ImGui::InputText("##ComputeShader", (char*)strName.c_str(), strName.size(), ImGuiInputTextFlags_ReadOnly);

    ImGui::NewLine();


    ImGui::Text("Thread Group");
    ImGui::SameLine(120.f);
    Vec3 vThreadGroup = pCS->GetThreadGroup();
    int iThread[3] =
    {
        (int)vThreadGroup.x,
        (int)vThreadGroup.y,
        (int)vThreadGroup.z
    };
    ImGui::InputInt3("##ThreadGroup", iThread, ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("Thread Per Group");
    ImGui::SameLine(120.f);
    Vec3 vThreadPerGroup = pCS->GetThreadPerGroup();
    int iThreadPer[3] =
    {
        (int)vThreadPerGroup.x,
        (int)vThreadPerGroup.y,
        (int)vThreadPerGroup.z
    };
    ImGui::InputInt3("##ThreadPerGroup", iThreadPer, ImGuiInputTextFlags_ReadOnly);

    return 0;
}


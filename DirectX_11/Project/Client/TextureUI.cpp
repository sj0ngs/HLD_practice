#include "pch.h"
#include "TextureUI.h"

#include <Engine\CTexture.h>

TextureUI::TextureUI()  :
    ResUI("Texture", RES_TYPE::TEXTURE)
{
    SetName("Texture");
}

TextureUI::~TextureUI()
{
}

int TextureUI::render_update()
{
    ResUI::render_update();

    Ptr<CTexture> pTex = (CTexture*)GetTargetRes().Get();

    ImGui::Text("Texture");
    ImGui::SameLine();
    string strName = string(pTex->GetKey().begin(), pTex->GetKey().end());
    ImGui::InputText("##Texture", (char*)strName.c_str(), strName.size(), ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("Size   ");
    ImGui::SameLine();
    string strSize = std::to_string(pTex->Width());
    strSize += " / ";
    strSize += std::to_string(pTex->Height());
    ImGui::Text((char*)strSize.c_str());

    const D3D11_TEXTURE2D_DESC& tDesc = pTex->GetDesc();

    ImGui::Text("Type   ");
    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.258f, 0.286f, 0.34f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.258f, 0.286f, 0.34f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.258f, 0.286f, 0.34f, 1.f));
    if (tDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET)
    {
        ImGui::SameLine();
        ImGui::Button("Render Target");
    }
    if (tDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL)
    {
        ImGui::SameLine();
        ImGui::Button("Depth Stencil");
    }
    if (tDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE)
    {
        ImGui::SameLine();
        ImGui::Button("Shader Resource");
    }
    if (tDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
    {
        ImGui::SameLine();
        ImGui::Button("Unordered Access");
    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();

    if (tDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE)
    {
        float fAspect = (float)pTex->Width() / (float)pTex->Height();
        ImGui::Image(pTex->GetSRV().Get(), ImVec2(300.f, 300.f / fAspect), 
                    ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), 
                    ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f));
    }

    return TRUE;
}

#include "pch.h"
#include "AtlasTextureUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CTimeMgr.h>

#include "AnimEditorUI.h"

AtlasTextureUI::AtlasTextureUI()  :
    UI("##Texture"),
    m_pAnimEditor(nullptr),
    m_iTexWidth(0),
    m_iTexHeight(0),
    m_vScrolling{},
    m_vFirst{},
    m_vSecond{},
    m_bGrid(true),
    m_bAddingRect(false),
    m_fScale(1.f),
    m_bUpdate(false)
{
    m_pAnimEditor = (AnimEditorUI*)ImGuiMgr::GetInst()->FindUI("##AnimEditor");
    AddWindowFlags(ImGuiWindowFlags_NoDocking);
}

AtlasTextureUI::~AtlasTextureUI()
{
}

void AtlasTextureUI::finaltick()
{
    if (!m_pAnimEditor->IsActive())
    {
        SetActive(false);
        return;
    }

    m_vecFrm = m_pAnimEditor->GetVecFrm();

    UI::finaltick();
}

int AtlasTextureUI::render_update()
{
    ImGui::Checkbox("Enable grid", &m_bGrid);
    float fStep = 0.1f;
    ImGui::Text("Zoom in/out");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(70.f);
    if (ImGui::InputScalar("##Scale", ImGuiDataType_Float, &m_fScale, &fStep, NULL, "%f"))
    {
        if (0.f >= m_fScale)
        {
            m_fScale = 0.1f;
        }
    }
    ImGui::Text("Mouse Left: drag to add frame,\nMouse Right: drag to scroll");
    DrawImage();

    if (m_bUpdate)
    {
        m_pAnimEditor->SetVecFrm(m_vecFrm);
        m_bUpdate = false;
    }

    return TRUE;
}

void AtlasTextureUI::DrawImage()
{
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();     
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();  
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw border and background color
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    // This will catch our interactions
    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered
    const bool is_active = ImGui::IsItemActive();   // Held
    const ImVec2 origin(canvas_p0.x + m_vScrolling.x, canvas_p0.y + m_vScrolling.y); // Lock scrolled origin
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

    // Add first and second point
    if (is_hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        if (0.f <= mouse_pos_in_canvas.x / m_fScale && 0.f <= mouse_pos_in_canvas.y / m_fScale
            && m_pTex->Width() > mouse_pos_in_canvas.x / m_fScale && m_pTex->Height() > mouse_pos_in_canvas.y / m_fScale)
        {
            Vec4 vRect = FindImageRect(Vec2(mouse_pos_in_canvas.x / m_fScale, mouse_pos_in_canvas.y / m_fScale));
            if (vRect.x + 1 != vRect.z && vRect.y + 1 != vRect.w)
            {
                tAnim2DFrm tFrm = {};
                tFrm.LeftTopUV = Vec2(vRect.x / m_iTexWidth, vRect.y / m_iTexHeight);
                tFrm.SliceUV = Vec2((vRect.z - vRect.x) / m_iTexWidth, (vRect.w - vRect.y) / m_iTexHeight);

                if (0 == m_vecFrm.size())
                    tFrm.fDuration = 0.1f;
                else
                    tFrm.fDuration = m_vecFrm.back().fDuration;

                m_vecFrm.push_back(tFrm);
                m_bUpdate = true;
            }
        }
    }
    if (is_hovered && !m_bAddingRect && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (0.f <= mouse_pos_in_canvas.x / m_fScale && 0.f <= mouse_pos_in_canvas.y / m_fScale
            && m_pTex->Width() > mouse_pos_in_canvas.x / m_fScale && m_pTex->Height() > mouse_pos_in_canvas.y / m_fScale)
        {
            m_vFirst = ImVec2(mouse_pos_in_canvas.x / m_fScale, mouse_pos_in_canvas.y / m_fScale);
            m_vSecond = ImVec2(mouse_pos_in_canvas.x / m_fScale, mouse_pos_in_canvas.y / m_fScale);
            m_bAddingRect = true;
        }
    }
    if (m_bAddingRect)
    {
        if (0.f <= mouse_pos_in_canvas.x / m_fScale && 0.f <= mouse_pos_in_canvas.y / m_fScale
            && m_pTex->Width() > mouse_pos_in_canvas.x / m_fScale && m_pTex->Height() > mouse_pos_in_canvas.y / m_fScale)
        {
            m_vSecond = ImVec2(mouse_pos_in_canvas.x / m_fScale, mouse_pos_in_canvas.y / m_fScale);
        }

        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            float fDist = sqrtf(powf(m_vSecond.x - m_vFirst.x, 2.f) + powf(m_vSecond.y - m_vFirst.y, 2.f));
            if (50.f * m_fScale <= fDist)
            {
                tAnim2DFrm tFrm = {};
                tFrm.LeftTopUV = Vec2(m_vFirst.x / m_iTexWidth, m_vFirst.y / m_iTexHeight);
                tFrm.SliceUV = Vec2((m_vSecond.x - m_vFirst.x) / m_iTexWidth, (m_vSecond.y - m_vFirst.y) / m_iTexHeight);

                if (0 == m_vecFrm.size())
                    tFrm.fDuration = 0.1f;
                else
                    tFrm.fDuration = m_vecFrm.back().fDuration;

                m_vecFrm.push_back(tFrm);
                m_bUpdate = true;
            }
            m_bAddingRect = false;
        }
    }

    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        m_vScrolling.x += io.MouseDelta.x;
        m_vScrolling.y += io.MouseDelta.y;
    }
    
    // Draw grid + all lines in the canvas
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);

    draw_list->AddImage((void*)(m_pTex->GetSRV().Get()), ImVec2(canvas_p0.x + m_vScrolling.x, canvas_p0.y + m_vScrolling.y),
        ImVec2((float)m_iTexWidth * m_fScale + m_vScrolling.x + canvas_p0.x, (float)m_iTexHeight * m_fScale + m_vScrolling.y + canvas_p0.y),
        ImVec2(0.f, 0.f), ImVec2(1.f, 1.f));

    if (m_bGrid)
    {
        const float GRID_STEP = 64.0f * m_fScale;
        for (float x = fmodf(m_vScrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
        for (float y = fmodf(m_vScrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
    }

    ImU32 RectCol = IM_COL32(255, 255, 0, 255);
    if (m_bAddingRect)
    {
        draw_list->AddRect(ImVec2(origin.x + m_vFirst.x * m_fScale, origin.y + m_vFirst.y * m_fScale),
                            ImVec2(origin.x + m_vSecond.x * m_fScale, origin.y + m_vSecond.y * m_fScale),
                            RectCol, 2.0f);
    }
    for (int i = 0; i < m_vecFrm.size(); i++)
    {
        draw_list->AddRect(ImVec2(origin.x + m_vecFrm[i].LeftTopUV.x * m_iTexWidth * m_fScale, 
                                  origin.y + m_vecFrm[i].LeftTopUV.y * m_iTexHeight * m_fScale),
                           ImVec2(origin.x + (m_vecFrm[i].LeftTopUV.x + m_vecFrm[i].SliceUV.x) * m_iTexWidth * m_fScale,
                                  origin.y + (m_vecFrm[i].LeftTopUV.y + m_vecFrm[i].SliceUV.y) * m_iTexHeight * m_fScale), RectCol, 2.0f);

        // 인덱스 번호 출력
        string strIdx = "[" + std::to_string(i) + "]";

        draw_list->AddRectFilled(ImVec2(origin.x + m_vecFrm[i].LeftTopUV.x * m_iTexWidth * m_fScale + 2.f,
            origin.y + m_vecFrm[i].LeftTopUV.y * m_iTexHeight * m_fScale + 2.f),
            ImVec2(origin.x + m_vecFrm[i].LeftTopUV.x * m_iTexWidth * m_fScale + 25 + strIdx.length(),
                origin.y + m_vecFrm[i].LeftTopUV.y * m_iTexHeight * m_fScale + 25), 
                IM_COL32(0.f, 0.f, 0.f, 200.f));

        draw_list->AddText(ImVec2(origin.x + m_vecFrm[i].LeftTopUV.x * m_iTexWidth * m_fScale + 5.f,
            origin.y + m_vecFrm[i].LeftTopUV.y * m_iTexHeight * m_fScale + 5.f),
            IM_COL32(255.f, 255.f, 255.f, 255.f), strIdx.c_str());
    }

    draw_list->PopClipRect();
}

Vec4 AtlasTextureUI::FindImageRect(Vec2 _vStart)
{
    const ScratchImage& Image = m_pTex->GetImage();
    int iSize = m_iTexWidth * m_iTexHeight;

    int Idx = m_iTexWidth * (int)_vStart.y + (int)_vStart.x;
    tPixel* pPixel = (tPixel*)Image.GetPixels();
    tPixel Pixel = pPixel[Idx];

    set<int> sPixel;
    queue<int> qPixel;
    qPixel.push(Idx);
    sPixel.insert(Idx);

    int iMinX = (int)_vStart.x, iMinY = (int)_vStart.y, iMaxX = (int)_vStart.x, iMaxY = (int)_vStart.y;
    while (!qPixel.empty())
    {
        Idx = qPixel.front();
        qPixel.pop();

        int iTemp = Idx % m_iTexWidth;
        if (iMinX > iTemp)
            iMinX = iTemp;

        if (iMaxX < iTemp)
            iMaxX = iTemp;

        iTemp = Idx / m_iTexWidth;
        if (iMinY > iTemp)
            iMinY = iTemp;

        if (iMaxY < iTemp)
            iMaxY = iTemp;

        // Left
        iTemp = Idx - 1;
        Pixel = pPixel[iTemp];
        if (0 <= iTemp && 0 < pPixel[iTemp].a && sPixel.find(iTemp) == sPixel.end())
        {
            qPixel.push(iTemp);
            sPixel.insert(iTemp);
        }
        // Left Up
        iTemp = Idx - m_iTexWidth - 1;
        if (0 <= iTemp && 0 < pPixel[iTemp].a && sPixel.find(iTemp) == sPixel.end())
        {
            qPixel.push(iTemp);
            sPixel.insert(iTemp);
        }
        // Up
        iTemp = Idx - m_iTexWidth;
        if (0 <= iTemp && 0 < pPixel[iTemp].a && sPixel.find(iTemp) == sPixel.end())
        {
            qPixel.push(iTemp);
            sPixel.insert(iTemp);
        }
        // Right Up
        iTemp = Idx - m_iTexWidth + 1;
        if (0 <= iTemp && 0 < pPixel[iTemp].a && sPixel.find(iTemp) == sPixel.end())
        {
            qPixel.push(iTemp);
            sPixel.insert(iTemp);
        }
        // Right
        iTemp = Idx + 1;
        if (iSize > iTemp && 0 < pPixel[iTemp].a && sPixel.find(iTemp) == sPixel.end())
        {
            qPixel.push(iTemp);
            sPixel.insert(iTemp);
        }
        // Right Down
        iTemp = Idx + m_iTexWidth + 1;
        if (iSize > iTemp && 0 < pPixel[iTemp].a && sPixel.find(iTemp) == sPixel.end())
        {
            qPixel.push(iTemp);
            sPixel.insert(iTemp);
        }
        // Down
        iTemp = Idx + m_iTexWidth;
        if (iSize > iTemp && 0 < pPixel[iTemp].a && sPixel.find(iTemp) == sPixel.end())
        {
            qPixel.push(iTemp);
            sPixel.insert(iTemp);
        }
        // Left Down
        iTemp = Idx + m_iTexWidth - 1;
        if (iSize > iTemp && 0 < pPixel[iTemp].a && sPixel.find(iTemp) == sPixel.end())
        {
            qPixel.push(iTemp);
            sPixel.insert(iTemp);
        }
    }
    return Vec4((float)iMinX, (float)iMinY, (float)iMaxX + 1.f, (float)iMaxY + 1.f);
}

void AtlasTextureUI::SetAtlasTex(Ptr<CTexture> _pTex)
{
    m_pTex = _pTex;
    m_iTexWidth = m_pTex->Width();
    m_iTexHeight = m_pTex->Height();
}

void AtlasTextureUI::Clear()
{
    m_pTex = nullptr;
    m_vecFrm.clear();
    m_iTexWidth = 0;
    m_iTexHeight = 0;
    m_vScrolling = Vec2{};
    m_vFirst = ImVec2{};
    m_vSecond = ImVec2{};
    m_bAddingRect = false;
    m_fScale = 1.f;
    m_bUpdate = false;
}

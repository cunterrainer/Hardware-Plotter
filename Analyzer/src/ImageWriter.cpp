#include "Thread.h"
#include "ImageWriter.h"

bool ImageWriter::SaveImage()
{
    RenderWindow::SetThemePopup();
    m_Open = true;
    m_ItemSpacing = ImGui::GetStyle().ItemSpacing;
    ImGui::SetNextWindowFocus();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("Image writer", nullptr, ImGuiWindowFlags_NoCollapse);
    CalcProperties();
    ImGui::SetWindowSize(m_WindowSize);
    ImGui::Image((void*)(intptr_t)m_Image->GetGpuImage(), m_ImageSize);

    ImGui::SameLine();
    const ImVec2 currsorPos = ImGui::GetCursorPos();

    ImGui::Text("%s", m_DisplayPath.c_str());
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("%s", m_Path.c_str());

    ImGui::SameLine(m_WindowSize.x - PathButtonSize.x - m_ItemSpacing.x);
    if (ImGui::Button("Path", PathButtonSize))
        Thread::Dispatch(&ImageWriter::SaveFileDialog, this);

    ImGui::SetCursorPos({ currsorPos.x, currsorPos.y + ImGui::GetItemRectSize().y + m_ItemSpacing.y });
    m_BtnWidth = (SettingsSectionWidth - 3 * m_ItemSpacing.x) * 0.5f;
    if (ImGui::Button("Save", { m_BtnWidth, 0 }))
        Thread::Dispatch(&ImageWriter::SaveImageToFile, this);

    ImGui::SameLine();
    const bool close = ImGui::Button("Cancel", { m_BtnWidth, 0 }) || m_Image->Width() <= 0 || m_Image->Height() <= 0;
    NextLine();
    Upscalar();
    ImGui::End();
    ImGui::PopStyleVar();
    RenderWindow::SetThemeWindow();
    return close;
}
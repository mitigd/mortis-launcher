#include "pch.h"
#include "UI/Theme.h"

namespace UI
{

    void ThemeManager::ApplyTheme()
    {
        std::time_t now = std::time(0);
        std::tm *ltm = std::localtime(&now);

        int month = 1 + ltm->tm_mon; // Jan = 1
        int day = ltm->tm_mday;

        // --- SEASONAL SELECTOR ---

        // Valentine's (Feb 7 - Feb 15)
        if (month == 2 && day >= 7 && day <= 15)
        {
            SetValentineTheme();
            return;
        }

        // St. Patrick's (Mar 14 - Mar 20)
        if (month == 3 && day >= 14 && day <= 20)
        {
            SetShamrockTheme();
            return;
        }

        // Halloween (Oct 20 - Nov 2)
        if ((month == 10 && day >= 20) || (month == 11 && day <= 2))
        {
            SetHalloweenTheme();
            return;
        }

        // Christmas (Dec 15 - Dec 30)
        if (month == 12 && day >= 15 && day <= 30)
        {
            SetChristmasTheme();
            return;
        }
    }

    void SetupCommonStyles(ImGuiStyle &style)
    {
        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;
        style.TabBorderSize = 0.0f;
    }

    // ---------------------------------------------------------
    // VALENTINE'S (FEB)
    // Dark Cherry Red & Soft Pink
    // ---------------------------------------------------------
    void ThemeManager::SetValentineTheme()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        SetupCommonStyles(style);
        ImVec4 *colors = style.Colors;

        ImVec4 dark_cherry = ImVec4(0.15f, 0.05f, 0.07f, 1.00f);
        ImVec4 soft_pink = ImVec4(0.85f, 0.40f, 0.50f, 0.80f);
        ImVec4 crimson = ImVec4(0.60f, 0.10f, 0.20f, 0.90f);

        colors[ImGuiCol_WindowBg] = dark_cherry;
        colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.03f, 0.04f, 0.70f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.08f, 0.10f, 0.96f);
        colors[ImGuiCol_Border] = crimson;

        colors[ImGuiCol_Header] = crimson;
        colors[ImGuiCol_HeaderHovered] = soft_pink;
        colors[ImGuiCol_HeaderActive] = ImVec4(0.95f, 0.50f, 0.60f, 1.00f);

        colors[ImGuiCol_Button] = crimson;
        colors[ImGuiCol_ButtonHovered] = soft_pink;
        colors[ImGuiCol_ButtonActive] = ImVec4(0.95f, 0.50f, 0.60f, 1.00f);

        colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.70f, 0.80f, 1.00f); // Pale Pink
        colors[ImGuiCol_SliderGrab] = colors[ImGuiCol_CheckMark];
        colors[ImGuiCol_Text] = ImVec4(1.00f, 0.90f, 0.92f, 1.00f);
    }

    // ---------------------------------------------------------
    // SHAMROCK (MARCH)
    // Deep Green & Gold
    // ---------------------------------------------------------
    void ThemeManager::SetShamrockTheme()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        SetupCommonStyles(style);
        ImVec4 *colors = style.Colors;

        ImVec4 deep_green = ImVec4(0.02f, 0.12f, 0.05f, 1.00f);
        ImVec4 emerald = ImVec4(0.00f, 0.50f, 0.20f, 0.80f);
        ImVec4 gold = ImVec4(1.00f, 0.80f, 0.20f, 1.00f);

        colors[ImGuiCol_WindowBg] = deep_green;
        colors[ImGuiCol_ChildBg] = ImVec4(0.01f, 0.08f, 0.03f, 0.70f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.02f, 0.15f, 0.06f, 0.96f);
        colors[ImGuiCol_Border] = emerald;

        colors[ImGuiCol_Header] = emerald;
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.60f, 0.25f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.70f, 0.30f, 1.00f);

        colors[ImGuiCol_Button] = emerald;
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.60f, 0.25f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.70f, 0.30f, 1.00f);

        colors[ImGuiCol_CheckMark] = gold;
        colors[ImGuiCol_SliderGrab] = gold;
        colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.90f, 0.50f, 1.00f);
        colors[ImGuiCol_Text] = ImVec4(0.90f, 1.00f, 0.92f, 1.00f);
    }

    // ---------------------------------------------------------
    // HALLOWEEN (OCT/NOV)
    // ---------------------------------------------------------
    void ThemeManager::SetHalloweenTheme()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        SetupCommonStyles(style);
        ImVec4 *colors = style.Colors;

        ImVec4 deep_purple = ImVec4(0.08f, 0.03f, 0.10f, 1.00f);
        ImVec4 pumpkin = ImVec4(0.90f, 0.45f, 0.00f, 0.80f);
        ImVec4 bone = ImVec4(0.90f, 0.85f, 0.75f, 1.00f);

        colors[ImGuiCol_WindowBg] = deep_purple;
        colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.04f, 0.12f, 0.70f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.05f, 0.15f, 0.96f);
        colors[ImGuiCol_Border] = pumpkin;

        colors[ImGuiCol_Header] = pumpkin;
        colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.55f, 0.10f, 0.90f);
        colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.60f, 0.15f, 1.00f);

        colors[ImGuiCol_Button] = ImVec4(0.30f, 0.10f, 0.40f, 0.80f); // Darker Purple buttons
        colors[ImGuiCol_ButtonHovered] = pumpkin;                     // Glow orange on hover
        colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.55f, 0.10f, 1.00f);

        colors[ImGuiCol_CheckMark] = pumpkin;
        colors[ImGuiCol_SliderGrab] = pumpkin;
        colors[ImGuiCol_Text] = bone;
    }

    // ---------------------------------------------------------
    // CHRISTMAS (DEC)
    // ---------------------------------------------------------
    void ThemeManager::SetChristmasTheme()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        SetupCommonStyles(style);
        ImVec4 *colors = style.Colors;

        ImVec4 pine_green = ImVec4(0.04f, 0.12f, 0.06f, 1.00f);
        ImVec4 berry_red = ImVec4(0.80f, 0.10f, 0.15f, 0.80f);
        ImVec4 snow = ImVec4(0.95f, 0.98f, 1.00f, 1.00f);
        ImVec4 gold = ImVec4(1.00f, 0.85f, 0.20f, 1.00f);

        colors[ImGuiCol_WindowBg] = pine_green;
        colors[ImGuiCol_ChildBg] = ImVec4(0.06f, 0.16f, 0.08f, 0.70f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.14f, 0.07f, 0.96f);

        colors[ImGuiCol_Header] = berry_red;
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.90f, 0.15f, 0.20f, 0.90f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.95f, 0.20f, 0.25f, 1.00f);

        colors[ImGuiCol_Button] = berry_red;
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.90f, 0.15f, 0.20f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.95f, 0.20f, 0.25f, 1.00f);

        colors[ImGuiCol_CheckMark] = gold;
        colors[ImGuiCol_SliderGrab] = gold;
        colors[ImGuiCol_Text] = snow;
    }

}
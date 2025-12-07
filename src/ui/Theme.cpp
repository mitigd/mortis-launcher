#include "pch.h"
#include "UI/Theme.h"

namespace UI
{
    void ThemeManager::ApplyTheme(AppTheme themeSelection) {
        
        if (themeSelection != AppTheme::AutoSeasonal) {
            switch (themeSelection) {
                case AppTheme::Valentine:   SetValentineTheme(); return;
                case AppTheme::Shamrock:    SetShamrockTheme(); return;
                case AppTheme::Halloween:   SetHalloweenTheme(); return;
                case AppTheme::Christmas:   SetChristmasTheme(); return;
                case AppTheme::DefaultDark: 
                default:                    SetDefaultTheme(); return;
            }
        }

        std::time_t now = std::time(0);
        std::tm* ltm = std::localtime(&now);
        int month = 1 + ltm->tm_mon;
        int day = ltm->tm_mday;

        if (month == 2 && day >= 7 && day <= 15) { SetValentineTheme(); return; }
        if (month == 3 && day >= 14 && day <= 20) { SetShamrockTheme(); return; }
        if ((month == 10 && day >= 20) || (month == 11 && day <= 2)) { SetHalloweenTheme(); return; }
        if (month == 12 && day >= 15 && day <= 30) { SetChristmasTheme(); return; }

        SetDefaultTheme();
    }

    void ThemeManager::SetupCommonStyles(ImGuiStyle &style)
    {
        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;
        style.TabBorderSize = 0.0f;
    }

    // ---------------------------------------------------------
    // DEFAULT
    // ---------------------------------------------------------
    void ThemeManager::SetDefaultTheme()
    {
        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        SetupCommonStyles(style);

        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1f, 0.1f, 0.12f, 0.70f); 
        style.Colors[ImGuiCol_WindowBg].w = 0.94f; 
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.35f, 0.40f, 0.45f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
        style.Colors[ImGuiCol_SeparatorActive]  = ImVec4(0.40f, 0.45f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_NavHighlight]  = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
    }

    // ---------------------------------------------------------
    // VALENTINE'S (FEB)
    // ---------------------------------------------------------
    void ThemeManager::SetValentineTheme()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        SetupCommonStyles(style);
        ImVec4 *colors = style.Colors;

        ImVec4 dark_cherry = ImVec4(0.15f, 0.05f, 0.07f, 1.00f);
        ImVec4 darker_red  = ImVec4(0.10f, 0.03f, 0.04f, 1.00f);
        ImVec4 soft_pink   = ImVec4(0.85f, 0.40f, 0.50f, 0.80f);
        ImVec4 crimson     = ImVec4(0.60f, 0.10f, 0.20f, 0.90f);

        colors[ImGuiCol_WindowBg] = dark_cherry;
        colors[ImGuiCol_ChildBg]  = ImVec4(0.10f, 0.03f, 0.04f, 0.70f);
        colors[ImGuiCol_PopupBg]  = ImVec4(0.20f, 0.08f, 0.10f, 0.98f);
        colors[ImGuiCol_Border]   = crimson;

        colors[ImGuiCol_Tab]           = crimson;
        colors[ImGuiCol_TabHovered]    = soft_pink;
        colors[ImGuiCol_TabActive]     = ImVec4(0.95f, 0.50f, 0.60f, 1.00f);
        colors[ImGuiCol_TabUnfocused]  = dark_cherry;
        colors[ImGuiCol_TabUnfocusedActive] = crimson;

        colors[ImGuiCol_SeparatorHovered] = soft_pink;
        colors[ImGuiCol_SeparatorActive]  = crimson;

        colors[ImGuiCol_TitleBg]       = crimson;
        colors[ImGuiCol_TitleBgActive] = crimson;
        colors[ImGuiCol_FrameBg]       = darker_red;
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.08f, 0.10f, 1.00f);
        colors[ImGuiCol_FrameBgActive]  = ImVec4(0.30f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_Header]        = crimson;
        colors[ImGuiCol_Button]        = crimson;
        colors[ImGuiCol_ButtonHovered] = soft_pink;
        colors[ImGuiCol_ButtonActive]  = ImVec4(0.95f, 0.50f, 0.60f, 1.00f);
        colors[ImGuiCol_CheckMark]     = ImVec4(1.00f, 0.70f, 0.80f, 1.00f);
        colors[ImGuiCol_SliderGrab]    = colors[ImGuiCol_CheckMark];
        colors[ImGuiCol_Text]          = ImVec4(1.00f, 0.90f, 0.92f, 1.00f);

        colors[ImGuiCol_HeaderHovered] = soft_pink;
        colors[ImGuiCol_HeaderActive]  = ImVec4(0.95f, 0.50f, 0.60f, 1.00f);
        colors[ImGuiCol_NavHighlight]  = crimson;
    }

    // ---------------------------------------------------------
    // SHAMROCK (MARCH)
    // ---------------------------------------------------------
    void ThemeManager::SetShamrockTheme()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        SetupCommonStyles(style);
        ImVec4 *colors = style.Colors;

        ImVec4 deep_green = ImVec4(0.02f, 0.12f, 0.05f, 1.00f);
        ImVec4 dark_green = ImVec4(0.01f, 0.08f, 0.02f, 1.00f); 
        ImVec4 emerald    = ImVec4(0.00f, 0.50f, 0.20f, 0.80f);
        ImVec4 bright_em  = ImVec4(0.00f, 0.60f, 0.25f, 1.00f);
        ImVec4 gold       = ImVec4(1.00f, 0.80f, 0.20f, 1.00f);

        colors[ImGuiCol_WindowBg] = deep_green;
        colors[ImGuiCol_ChildBg]  = ImVec4(0.01f, 0.08f, 0.03f, 0.70f);
        colors[ImGuiCol_PopupBg]  = ImVec4(0.02f, 0.15f, 0.06f, 0.98f);
        colors[ImGuiCol_Border]   = emerald;

        colors[ImGuiCol_Tab]           = emerald;
        colors[ImGuiCol_TabHovered]    = bright_em;
        colors[ImGuiCol_TabActive]     = ImVec4(0.00f, 0.70f, 0.30f, 1.00f);
        colors[ImGuiCol_TabUnfocused]  = deep_green;
        colors[ImGuiCol_TabUnfocusedActive] = emerald;

        colors[ImGuiCol_SeparatorHovered] = bright_em;
        colors[ImGuiCol_SeparatorActive]  = emerald;

        colors[ImGuiCol_TitleBg]       = emerald;
        colors[ImGuiCol_TitleBgActive] = bright_em;
        colors[ImGuiCol_FrameBg]       = dark_green;
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.05f, 0.20f, 0.10f, 1.00f); 
        colors[ImGuiCol_FrameBgActive]  = ImVec4(0.05f, 0.25f, 0.12f, 1.00f);
        colors[ImGuiCol_Header]        = emerald;
        colors[ImGuiCol_Button]        = emerald;
        colors[ImGuiCol_ButtonHovered] = bright_em;
        colors[ImGuiCol_ButtonActive]  = ImVec4(0.00f, 0.70f, 0.30f, 1.00f);
        colors[ImGuiCol_CheckMark]     = gold;
        colors[ImGuiCol_SliderGrab]    = gold;
        colors[ImGuiCol_Text]          = ImVec4(0.90f, 1.00f, 0.92f, 1.00f);

        colors[ImGuiCol_HeaderHovered] = bright_em;
        colors[ImGuiCol_HeaderActive]  = ImVec4(0.00f, 0.70f, 0.30f, 1.00f);
        colors[ImGuiCol_NavHighlight]  = emerald;
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
        ImVec4 dark_purple = ImVec4(0.05f, 0.02f, 0.07f, 1.00f); 
        ImVec4 pumpkin     = ImVec4(0.90f, 0.45f, 0.00f, 0.80f);
        ImVec4 bone        = ImVec4(0.90f, 0.85f, 0.75f, 1.00f);

        colors[ImGuiCol_WindowBg] = deep_purple;
        colors[ImGuiCol_ChildBg]  = ImVec4(0.10f, 0.04f, 0.12f, 0.70f);
        colors[ImGuiCol_PopupBg]  = ImVec4(0.12f, 0.05f, 0.15f, 0.98f);
        colors[ImGuiCol_Border]   = pumpkin;

        colors[ImGuiCol_Tab]           = ImVec4(0.30f, 0.10f, 0.40f, 0.80f);
        colors[ImGuiCol_TabHovered]    = pumpkin;
        colors[ImGuiCol_TabActive]     = ImVec4(1.00f, 0.55f, 0.10f, 1.00f);
        colors[ImGuiCol_TabUnfocused]  = deep_purple;
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.30f, 0.10f, 0.40f, 1.00f);

        colors[ImGuiCol_SeparatorHovered] = pumpkin;
        colors[ImGuiCol_SeparatorActive]  = ImVec4(1.00f, 0.55f, 0.10f, 1.00f);

        colors[ImGuiCol_TitleBg]       = ImVec4(0.30f, 0.10f, 0.40f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.40f, 0.15f, 0.50f, 1.00f);
        colors[ImGuiCol_FrameBg]       = dark_purple;
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.10f, 0.25f, 1.00f);
        colors[ImGuiCol_FrameBgActive]  = ImVec4(0.30f, 0.15f, 0.35f, 1.00f);
        colors[ImGuiCol_Header]        = pumpkin;
        colors[ImGuiCol_Button]        = ImVec4(0.30f, 0.10f, 0.40f, 0.80f); 
        colors[ImGuiCol_ButtonHovered] = pumpkin;                     
        colors[ImGuiCol_ButtonActive]  = ImVec4(1.00f, 0.55f, 0.10f, 1.00f);
        colors[ImGuiCol_CheckMark]     = pumpkin;
        colors[ImGuiCol_SliderGrab]    = pumpkin;
        colors[ImGuiCol_Text]          = bone;

        colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.55f, 0.10f, 0.90f);
        colors[ImGuiCol_HeaderActive]  = ImVec4(1.00f, 0.60f, 0.15f, 1.00f);
        colors[ImGuiCol_NavHighlight]  = pumpkin;
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
        ImVec4 dark_pine  = ImVec4(0.02f, 0.08f, 0.04f, 1.00f); 
        ImVec4 berry_red  = ImVec4(0.80f, 0.10f, 0.15f, 0.80f);
        ImVec4 snow       = ImVec4(0.95f, 0.98f, 1.00f, 1.00f);
        ImVec4 gold       = ImVec4(1.00f, 0.85f, 0.20f, 1.00f);
        ImVec4 emerald    = ImVec4(0.00f, 0.50f, 0.20f, 0.80f);

        colors[ImGuiCol_WindowBg] = pine_green;
        colors[ImGuiCol_ChildBg]  = ImVec4(0.06f, 0.16f, 0.08f, 0.70f);
        colors[ImGuiCol_PopupBg]  = ImVec4(0.05f, 0.14f, 0.07f, 0.98f);
        colors[ImGuiCol_Border]   = gold; 

        colors[ImGuiCol_Tab]           = berry_red;
        colors[ImGuiCol_TabHovered]    = ImVec4(0.90f, 0.15f, 0.20f, 0.90f);
        colors[ImGuiCol_TabActive]     = ImVec4(0.95f, 0.20f, 0.25f, 1.00f);
        colors[ImGuiCol_TabUnfocused]  = pine_green;
        colors[ImGuiCol_TabUnfocusedActive] = berry_red;

        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.90f, 0.15f, 0.20f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = emerald;
        colors[ImGuiCol_SeparatorActive]  = gold;

        colors[ImGuiCol_TitleBg]       = berry_red;
        colors[ImGuiCol_TitleBgActive] = berry_red;
        colors[ImGuiCol_FrameBg]       = dark_pine;
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.25f, 0.15f, 1.00f);
        colors[ImGuiCol_FrameBgActive]  = ImVec4(0.15f, 0.30f, 0.20f, 1.00f);
        colors[ImGuiCol_Header]        = berry_red;
        colors[ImGuiCol_Button]        = berry_red;
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.90f, 0.15f, 0.20f, 1.00f);
        colors[ImGuiCol_ButtonActive]  = ImVec4(0.95f, 0.20f, 0.25f, 1.00f);
        colors[ImGuiCol_CheckMark]     = gold;
        colors[ImGuiCol_SliderGrab]    = gold;
        colors[ImGuiCol_Text]          = snow;

        colors[ImGuiCol_HeaderActive]  = ImVec4(0.95f, 0.20f, 0.25f, 1.00f);
        colors[ImGuiCol_NavHighlight]  = gold;
    }
}
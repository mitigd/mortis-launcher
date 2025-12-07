#ifndef THEME_H
#define THEME_H

#include "imgui.h"
#include <ctime>

namespace UI {

    enum class AppTheme {
        AutoSeasonal = 0,
        DefaultDark, 
        Valentine,
        Shamrock,
        Halloween,
        Christmas
    };

    class ThemeManager {
    public:
        static void ApplyTheme(AppTheme themeSelection = AppTheme::AutoSeasonal);

    private:
        static void SetupCommonStyles(ImGuiStyle& style);
        
        static void SetDefaultTheme();
        static void SetValentineTheme();
        static void SetShamrockTheme();
        static void SetHalloweenTheme();
        static void SetChristmasTheme();
    };
} // namespace UI

#endif // THEME_H
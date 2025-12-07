#pragma once
#include "imgui.h"
#include <ctime>

namespace UI {

    class ThemeManager {
    public:
        static void ApplyTheme();

    private:
        static void SetHalloweenTheme();
        static void SetChristmasTheme();
        // New Ones
        static void SetValentineTheme();
        static void SetShamrockTheme();
        static void SetSpringTheme();
        static void SetSynthwaveTheme();
    };

}
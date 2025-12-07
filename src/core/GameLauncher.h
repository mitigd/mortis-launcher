#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include "imgui.h"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <future>

namespace Core
{

    namespace fs = std::filesystem;

    enum class GamePlatform
    {
        DOS = 0,
        Windows = 1
    };
    enum class GameStatus
    {
        Unplayable = 0,
        Playable = 1
    };
    enum class MachineType
    {
        PC = 0,
        Tandy = 1
    };

    struct GameEntry
    {
        std::string name = "New Game";
        std::string description = "";

        GamePlatform platform = GamePlatform::DOS;
        GameStatus status = GameStatus::Unplayable;

        std::string exePath;
        std::string setupPath;

        int ramKB = 16384;
        int mips = 60;
        MachineType machine = MachineType::PC;

        bool audioFlags[6] = {false, false, false, true, false, false};

        int videoHwIdx = 5;
        int width = 800, height = 600, depth = 32;
        std::string rootPathOverride;

        std::string isoPath;
    };

    struct FileBrowserEntry
    {
        std::string name;
        fs::path fullPath;
        bool isDirectory;
    };

    class GameLauncher
    {
    public:
        GameLauncher();
        ~GameLauncher();

        void Initialize();
        void RenderUI();

        struct FileBrowserEntry
        {
            std::filesystem::path fullPath;
            std::string name;
            bool isDirectory;
        };

    private:
        void LoadDatabase();
        void SaveDatabase();
        void SortLibrary();

        void LaunchGame(const GameEntry &game, bool runSetup);
        void CreateDreammFile(const GameEntry &game);
        std::string BuildCommand(const GameEntry &game, bool runSetup);

        std::string m_autoScrollTarget;

        int m_autoScrollFrames = 0; 

        // -- UI Helpers --
        void RenderGameList();
        void RenderGameDashboard();
        void RenderEditWindow();

        // -- File Browser --
        void OpenFileBrowser(const std::string &title, std::string *targetStringRef, const std::vector<std::string> &allowedExtensions = {});
        void RenderFileBrowser();

        bool m_showParent = true;
        bool m_pendingOpenChild = false;
        bool m_showChild = false;

        // -- State --
        std::string m_dreammExePath;
        std::vector<GameEntry> m_games;
        int m_selectedGameIdx = -1;

        // UI State
        bool m_showEditWindow = false;
        bool m_editWindowNeedsFocus = false;
        bool m_triggerEditWindow = false;
        bool m_scrollToSelection = false;

        // Filtering State
        char m_filterName[256] = "";
        int m_filterPlatform = 0;
        int m_filterStatus = 0;

        // File Browser State
        bool m_showFileBrowser = false;
        bool m_pendingBrowserOpen = false;

        std::future<std::vector<FileBrowserEntry>> m_loadingFuture;

        std::string m_fileBrowserTitle;
        std::string *m_fileBrowserTarget = nullptr;

        std::vector<std::string> m_fileBrowserExtensions;

        fs::path m_browserCurrentPath;
        fs::path m_lastGlobalPath; 

        std::vector<FileBrowserEntry> m_browserEntries;

        // Constants
        const char *m_audioNames[6] = {"speaker", "cms", "adlib", "sb16", "mt32", "gmidi"};
        const char *m_videoHwOptions[6] = {"hercules", "cga", "ega", "mcga", "vga", "svga"};
    };

}

#endif // GAMELAUNCHER_H
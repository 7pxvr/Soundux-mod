#include <backward.hpp>
#include <core/enums/enums.hpp>
#include <core/global/globals.hpp>
#include <cstdlib>
#include <fancy.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <ui/impl/webview/webview.hpp>

#if defined(__linux__)
#include <helper/audio/linux/backend.hpp>
#endif

#if defined(__linux__) && !defined(SOUNDUX_WEBVIEW_QT)
namespace
{
    bool hasEnvValue(const char *name)
    {
        const auto *value = std::getenv(name); // NOLINT
        return value && std::string(value).length() > 0;
    }

    bool isWaylandSession()
    {
        const auto *sessionType = std::getenv("XDG_SESSION_TYPE"); // NOLINT
        return hasEnvValue("WAYLAND_DISPLAY") || (sessionType && std::string(sessionType) == "wayland");
    }

    bool isNvidiaGpuPresent()
    {
        static constexpr auto nvidiaVendorId = "0x10de";

        std::error_code ec;
        for (const auto &entry : std::filesystem::directory_iterator("/sys/class/drm", ec))
        {
            auto vendorPath = entry.path() / "device" / "vendor";
            if (!std::filesystem::is_regular_file(vendorPath, ec))
            {
                continue;
            }

            std::ifstream vendorFile(vendorPath);
            std::string vendor;
            vendorFile >> vendor;
            std::transform(vendor.begin(), vendor.end(), vendor.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            if (vendor == nvidiaVendorId)
            {
                return true;
            }
        }

        return std::filesystem::exists("/proc/driver/nvidia/version", ec);
    }

    void configureWaylandWebKitWorkarounds()
    {
        if (!isWaylandSession())
        {
            return;
        }

        if (hasEnvValue("SOUNDUX_DISABLE_WEBKIT_DMABUF"))
        {
            setenv("WEBKIT_DISABLE_DMABUF_RENDERER", "1", 0);
            Fancy::fancy.logTime().message()
                << "Wayland WebKit workaround: disabling DMA-BUF renderer by request" << std::endl;
            return;
        }

        if (isNvidiaGpuPresent() && !hasEnvValue("__NV_DISABLE_EXPLICIT_SYNC") &&
            !hasEnvValue("WEBKIT_DISABLE_DMABUF_RENDERER") && !hasEnvValue("WEBKIT_DISABLE_COMPOSITING_MODE"))
        {
            setenv("__NV_DISABLE_EXPLICIT_SYNC", "1", 0);
            Fancy::fancy.logTime().message()
                << "Wayland NVIDIA workaround: disabling explicit sync for WebKit" << std::endl;
            return;
        }

        if (hasEnvValue("__NV_DISABLE_EXPLICIT_SYNC") || hasEnvValue("WEBKIT_DISABLE_DMABUF_RENDERER") ||
            hasEnvValue("WEBKIT_DISABLE_COMPOSITING_MODE"))
        {
            Fancy::fancy.logTime().message() << "Wayland WebKit workaround: using existing environment" << std::endl;
        }
    }
} // namespace
#endif

#if defined(_WIN32)
#include "../assets/icon.h"
#include <Windows.h>
#include <helper/misc/misc.hpp>
#include <shellapi.h>

int __stdcall WinMain([[maybe_unused]] HINSTANCE hInstrance, [[maybe_unused]] HINSTANCE prevInstance,
                      [[maybe_unused]] LPSTR winArgs, [[maybe_unused]] int argc)
#else
int main(int argc, char **arguments)
#endif
{
    using namespace Soundux::Globals; // NOLINT
    using namespace Soundux::Objects; // NOLINT
    using namespace Soundux::Enums;   // NOLINT

#if defined(_WIN32)
    auto **arguments = CommandLineToArgvW(GetCommandLineW(), &argc);

    std::vector<std::string> args;
    for (int i = 0; argc > i; i++)
    {
        args.emplace_back(Soundux::Helpers::narrow(arguments[i]));
    }
#else
    std::vector<std::string> args(reinterpret_cast<char **>(arguments), reinterpret_cast<char **>(arguments) + argc);
#endif

#if defined(__linux__) && !defined(SOUNDUX_WEBVIEW_QT)
    configureWaylandWebKitWorkarounds();
#endif

#if defined(_WIN32)
    if (std::getenv("SOUNDUX_DEBUG")) // NOLINT
    {
        AllocConsole();
        freopen_s(reinterpret_cast<FILE **>(stdin), "CONIN$", "r", stdin);
        freopen_s(reinterpret_cast<FILE **>(stderr), "CONOUT$", "w", stderr);
        freopen_s(reinterpret_cast<FILE **>(stdout), "CONOUT$", "w", stdout);

        DWORD lMode = 0;
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleMode(hStdout, &lMode);
        SetConsoleMode(hStdout, lMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
    }
#endif
    if (std::getenv("SOUNDUX_DEBUG") != nullptr) // NOLINT
    {
        Fancy::fancy.logTime().success() << "Enabling debug features" << std::endl;
    }

    backward::SignalHandling crashHandler;
    gGuard = std::make_shared<guardpp::guard>("soundux-guard");

    if (std::find(args.begin(), args.end(), "--help") != args.end() ||
        std::find(args.begin(), args.end(), "-h") != args.end())
    {
        Fancy::fancy.message() << "Soundux usage" << std::endl;
        Fancy::fancy.message() << "  -h --help        description of launch arguments" << std::endl;
        Fancy::fancy.message() << "  --hidden         start application hidden to taskbar" << std::endl;
        Fancy::fancy.message() << "  --reset-mutex    fix 'Another instance is already running! error'" << std::endl;
        return 0;
    }

    if (std::find(args.begin(), args.end(), "--reset-mutex") != args.end())
    {
        gGuard->reset();
        gGuard.reset();
        gGuard = std::make_shared<guardpp::guard>("soundux-guard");
    }

    if (auto other_instance = gGuard->other_instance(); other_instance.has_value() && other_instance.value())
    {
        Fancy::fancy.logTime().failure() << "Another Instance is already running!" << std::endl;
        return 1;
    }
    else if (!other_instance.has_value()) // NOLINT
    {
        Fancy::fancy.logTime().warning() << "Failed to check for other instances: " << other_instance.error()
                                         << std::endl;
    }

    gConfig.load();
    gData.set(gConfig.data);
    gSettings = gConfig.settings;

#if defined(__linux__)
    gIcons = IconFetcher::createInstance();
    gAudioBackend = AudioBackend::createInstance(gSettings.audioBackend);
#elif defined(_WIN32)
    gWinSound = WinSound::createInstance();
#endif

    gAudio.setup();
    gYtdl.setup();

#if defined(__linux__)
    if (gAudioBackend && gSettings.audioBackend == BackendType::PulseAudio && gConfig.settings.useAsDefaultDevice)
    {
        gAudioBackend->useAsDefault();
    }
#endif

    gGui = std::make_unique<Soundux::Objects::WebView>();
    gGui->setup();

    if (std::find(args.begin(), args.end(), "--hidden") == args.end())
    {
        gGui->show();
    }
    else
    {
        Fancy::fancy.logTime().message() << "Starting window hidden" << std::endl;
    }

#if defined(_WIN32)
    HICON hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1)); // NOLINT
    SendMessage(GetActiveWindow(), WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
    SendMessage(GetActiveWindow(), WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
#endif

    gGui->mainLoop();

    gAudio.destroy();
#if defined(__linux__)
    if (gAudioBackend)
    {
        gAudioBackend->destroy();
    }
#endif
    gConfig.data.set(gData);
    gConfig.settings = gSettings;
    if (!gConfig.settings.rememberApplications)
    {
        gConfig.settings.outputs.clear();
        gConfig.settings.rememberedApplications.clear();
    }
    gConfig.save();

    return 0;
}

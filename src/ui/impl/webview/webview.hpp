#pragma once
#include <atomic>
#include <thread>
#include <tray.hpp>
#include <ui/ui.hpp>
#include <webview.hpp>

namespace Soundux
{
    namespace Objects
    {
        class WebView : public Window
        {
          private:
            std::shared_ptr<Tray::Tray> tray;
            std::shared_ptr<Webview::Window> webview;
#if defined(__linux__)
            std::thread audioAppRefreshThread;
            std::atomic<bool> stopAudioAppRefresh = false;
            std::string audioAppSignature;
#endif

            bool onClose();
            void exposeFunctions();
            void onResize(int, int);

            void setupTray();
            void fetchTranslations();
#if defined(__linux__)
            void refreshAudioApps(bool force = false);
            void startAudioAppRefresh();
            void stopAudioAppRefreshThread();
#endif

            void onAllSoundsFinished() override;
            Settings changeSettings(Settings newSettings) override;

          public:
            void show() override;
            void setup() override;
            void mainLoop() override;
            void onSoundFinished(const PlayingSound &sound) override;
            void onHotKeyReceived(const std::vector<int> &keys) override;

            void onAdminRequired() override;
            void onSettingsChanged() override;
            void onSwitchOnConnectDetected(bool state) override;
            void onError(const Enums::ErrorCode &error) override;
            void onSoundPlayed(const PlayingSound &sound) override;
            void onSoundProgressed(const PlayingSound &sound) override;
            void onDownloadProgressed(float progress, const std::string &eta) override;
        };
    } // namespace Objects
} // namespace Soundux

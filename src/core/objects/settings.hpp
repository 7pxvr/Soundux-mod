#pragma once
#include <core/enums/enums.hpp>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace Soundux
{
    namespace Objects
    {
        struct Settings
        {
#if defined(__linux__)
            Enums::BackendType audioBackend = Enums::BackendType::PipeWire;
#else
            Enums::BackendType audioBackend = Enums::BackendType::None;
#endif
            Enums::ViewMode viewMode = Enums::ViewMode::List;
            Enums::Theme theme = Enums::Theme::System;
            std::optional<std::string> language;

            std::vector<int> pushToTalkKeys;
            std::vector<int> stopHotkey;

            std::vector<std::string> outputs;
            std::vector<std::string> disabledApplications;
            std::map<std::string, std::string> rememberedApplications;
            std::uint32_t selectedTab = 0;

            int remoteVolume = 100;
            int localVolume = 50;
            bool syncVolumes = false;

            bool allowMultipleOutputs = true;
            bool autoRefreshAudioDevices = true;
            bool rememberApplications = true;
            bool useAsDefaultDevice = false;
            bool muteDuringPlayback = false;
            bool allowOverlapping = true;
            bool minimizeToTray = false;
            bool tabHotkeysOnly = false;
            bool deleteToTrash = true;
        };
    } // namespace Objects
} // namespace Soundux

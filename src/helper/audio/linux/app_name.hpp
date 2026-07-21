#pragma once
#if defined(__linux__)
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace Soundux::Objects::LinuxAudioAppName
{
    inline std::string lower(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    }

    inline std::string trim(const std::string &value)
    {
        const auto first = value.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
        {
            return "";
        }

        const auto last = value.find_last_not_of(" \t\r\n");
        return value.substr(first, (last - first) + 1);
    }

    inline std::string readFile(const std::filesystem::path &path)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream)
        {
            return "";
        }

        return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
    }

    inline std::vector<std::string> splitNullSeparated(const std::string &value)
    {
        std::vector<std::string> parts;
        std::string current;
        for (const auto c : value)
        {
            if (c == '\0')
            {
                if (!current.empty())
                {
                    parts.emplace_back(current);
                    current.clear();
                }
            }
            else
            {
                current += c;
            }
        }

        if (!current.empty())
        {
            parts.emplace_back(current);
        }

        return parts;
    }

    inline bool isRuntimeName(const std::string &value)
    {
        const auto name = lower(std::filesystem::path(value).stem().string());
        return name == "electron" || name.rfind("electron", 0) == 0 || name == "chrome" || name == "chromium" ||
               name == "exe";
    }

    inline std::string normalizeIdentifier(const std::string &value)
    {
        auto identifier = trim(value);
        if (identifier.empty())
        {
            return identifier;
        }

        const auto slash = identifier.find_last_of('/');
        if (slash != std::string::npos)
        {
            identifier = identifier.substr(slash + 1);
        }

        constexpr auto desktopSuffix = ".desktop";
        const auto lowerIdentifier = lower(identifier);
        if (lowerIdentifier.size() > std::char_traits<char>::length(desktopSuffix) &&
            lowerIdentifier.compare(lowerIdentifier.size() - std::char_traits<char>::length(desktopSuffix),
                                    std::char_traits<char>::length(desktopSuffix), desktopSuffix) == 0)
        {
            identifier.resize(identifier.size() - std::char_traits<char>::length(desktopSuffix));
        }

        return identifier;
    }

    inline std::string prettyIdentifier(std::string identifier)
    {
        if (identifier.empty())
        {
            return identifier;
        }

        if (auto dot = identifier.find_last_of('.'); dot != std::string::npos)
        {
            identifier = identifier.substr(dot + 1);
        }

        std::replace(identifier.begin(), identifier.end(), '_', ' ');
        std::replace(identifier.begin(), identifier.end(), '-', ' ');

        bool capitalize = true;
        for (auto &c : identifier)
        {
            if (std::isspace(static_cast<unsigned char>(c)))
            {
                capitalize = true;
                continue;
            }

            c = capitalize ? static_cast<char>(std::toupper(static_cast<unsigned char>(c)))
                           : static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            capitalize = false;
        }

        return identifier;
    }

    inline std::optional<std::string> desktopNameFor(const std::string &identifier)
    {
        if (identifier.empty())
        {
            return std::nullopt;
        }

        std::vector<std::filesystem::path> roots = {"/usr/share/applications",
                                                    "/var/lib/flatpak/exports/share/applications"};
        if (const auto *home = std::getenv("HOME"); home)
        {
            roots.emplace_back(std::filesystem::path(home) / ".local/share/applications");
        }

        const auto needle = lower(identifier);
        std::error_code ec;
        for (const auto &root : roots)
        {
            if (!std::filesystem::is_directory(root, ec))
            {
                continue;
            }

            for (const auto &entry : std::filesystem::directory_iterator(root, ec))
            {
                if (!entry.is_regular_file(ec) || entry.path().extension() != ".desktop")
                {
                    continue;
                }

                auto content = readFile(entry.path());
                auto searchable = lower(entry.path().filename().string() + "\n" + content);
                if (searchable.find(needle) == std::string::npos)
                {
                    continue;
                }

                std::istringstream lines(content);
                std::string line;
                while (std::getline(lines, line))
                {
                    if (line.rfind("Name=", 0) == 0)
                    {
                        auto name = trim(line.substr(5));
                        if (!name.empty())
                        {
                            return name;
                        }
                    }
                }
            }
        }

        return std::nullopt;
    }

    inline std::optional<std::string> resolveFromIdentifiers(const std::vector<std::string> &identifiers)
    {
        for (const auto &rawIdentifier : identifiers)
        {
            auto identifier = normalizeIdentifier(rawIdentifier);
            if (identifier.empty() || isRuntimeName(identifier))
            {
                continue;
            }

            if (auto desktopName = desktopNameFor(rawIdentifier))
            {
                return desktopName;
            }
            if (auto desktopName = desktopNameFor(identifier))
            {
                return desktopName;
            }

            return prettyIdentifier(identifier);
        }

        return std::nullopt;
    }

    inline std::optional<std::string> candidateFromArgument(const std::string &argument)
    {
        constexpr auto userDataPrefix = "--user-data-dir=";
        if (argument.rfind(userDataPrefix, 0) == 0)
        {
            auto name = std::filesystem::path(argument.substr(std::char_traits<char>::length(userDataPrefix)))
                            .filename()
                            .string();
            if (!name.empty() && !isRuntimeName(name))
            {
                return name;
            }
        }

        std::filesystem::path path(argument);
        for (const auto &part : path)
        {
            auto name = part.string();
            if (!name.empty() && name != "/" && !isRuntimeName(name) && name != "usr" && name != "lib" &&
                name != "bin" && name != "share" && name != "app.asar")
            {
                if (name == ".config")
                {
                    continue;
                }

                const auto lowerName = lower(name);
                if (lowerName.find("equibop") != std::string::npos || lowerName.find("vesktop") != std::string::npos ||
                    lowerName.find("discord") != std::string::npos)
                {
                    return name;
                }
            }
        }

        return std::nullopt;
    }

    inline std::vector<std::string> candidatesFor(std::uint32_t pid)
    {
        std::vector<std::string> candidates;
        if (pid == 0)
        {
            return candidates;
        }

        const auto procRoot = std::filesystem::path("/proc") / std::to_string(pid);

        std::error_code ec;
        if (auto exe = std::filesystem::read_symlink(procRoot / "exe", ec); !ec && !exe.empty())
        {
            auto executableName = exe.stem().string();
            if (!executableName.empty() && !isRuntimeName(executableName))
            {
                candidates.emplace_back(executableName);
            }
        }

        for (const auto &argument : splitNullSeparated(readFile(procRoot / "cmdline")))
        {
            if (auto candidate = candidateFromArgument(argument))
            {
                candidates.emplace_back(*candidate);
            }
        }

        return candidates;
    }

    inline std::optional<std::string> identity(std::uint32_t pid, const std::string &binary,
                                               const std::vector<std::string> &identifiers = {})
    {
        if (isRuntimeName(binary))
        {
            for (const auto &rawIdentifier : identifiers)
            {
                auto identifier = normalizeIdentifier(rawIdentifier);
                if (!identifier.empty() && !isRuntimeName(identifier))
                {
                    return lower(identifier);
                }
            }

            for (const auto &candidate : candidatesFor(pid))
            {
                return lower(candidate);
            }
        }

        return std::nullopt;
    }

    inline std::optional<std::string> resolve(std::uint32_t pid, const std::string &binary,
                                              const std::vector<std::string> &identifiers = {})
    {
        if (isRuntimeName(binary))
        {
            if (auto resolvedIdentifier = resolveFromIdentifiers(identifiers))
            {
                return resolvedIdentifier;
            }

            for (const auto &candidate : candidatesFor(pid))
            {
                if (auto desktopName = desktopNameFor(candidate))
                {
                    return desktopName;
                }
                return prettyIdentifier(candidate);
            }
        }

        return std::nullopt;
    }
} // namespace Soundux::Objects::LinuxAudioAppName
#endif

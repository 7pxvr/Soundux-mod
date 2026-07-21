#include "check.hpp"
#include <fancy.hpp>
#include <json.hpp>
#include <optional>
#include <semver.hpp>

httplib::Client VersionCheck::client("https://api.github.com");

std::optional<Soundux::Objects::VersionStatus> VersionCheck::getStatus()
{
    auto githubTags = client.Get("/repos/7pxvr/Soundux-mod/tags");

    if (githubTags && githubTags->status == 200)
    {
        auto parsed = nlohmann::json::parse(githubTags->body, nullptr, false);

        if (!parsed.is_discarded() && parsed.is_array() && !parsed.empty())
        {
            auto latestTag = parsed[0]["name"];
            if (!latestTag.is_null())
            {
                auto latestTagStr = latestTag.get<std::string>();

                try
                {
                    auto remote = semver::from_string(latestTagStr);
                    auto local = semver::from_string(SOUNDUX_VERSION);

                    return Soundux::Objects::VersionStatus{SOUNDUX_VERSION, latestTagStr, remote > local};
                }
                catch (const std::exception &e)
                {
                    return std::nullopt;
                }
            }
        }
    }

    return std::nullopt;
}

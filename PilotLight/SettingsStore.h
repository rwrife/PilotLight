#pragma once

#include <string>

class SettingsStore {
public:
    struct Settings {
        std::wstring endpoint;
        std::wstring apiKey;
        bool stubModeEnabled = false;
    };

    static const Settings& Get();
    static void SetApiKey(const std::wstring& apiKey);
    static void SetEndpoint(const std::wstring& endpoint);
    static void SetStubModeEnabled(bool enabled);
    static bool IsStubModeEnabled();
    static void Save();

private:
    static void EnsureLoaded();
    static std::wstring SettingsFilePath();

    static Settings s_settings;
    static bool s_loaded;
};

#include "SettingsStore.h"
#include "FileUtils.h"
#include <fstream>
#include <cwctype>
#include <locale>

namespace {
    std::wstring TrimWhitespace(const std::wstring& value)
    {
        size_t start = 0;
        size_t end = value.length();

        while (start < end && std::iswspace(value[start])) {
            ++start;
        }
        while (end > start && std::iswspace(value[end - 1])) {
            --end;
        }

        return value.substr(start, end - start);
    }

    bool StringEqualsIgnoreCase(const std::wstring& a, const std::wstring& b)
    {
        if (a.length() != b.length()) {
            return false;
        }
        std::wstring aLower = a;
        std::wstring bLower = b;
        for (wchar_t& ch : aLower) ch = std::towlower(ch);
        for (wchar_t& ch : bLower) ch = std::towlower(ch);
        return aLower == bLower;
    }
}

SettingsStore::Settings SettingsStore::s_settings;
bool SettingsStore::s_loaded = false;

const SettingsStore::Settings& SettingsStore::Get()
{
    EnsureLoaded();
    return s_settings;
}

void SettingsStore::SetApiKey(const std::wstring& apiKey)
{
    EnsureLoaded();
    s_settings.apiKey = apiKey;
}

void SettingsStore::SetStubModeEnabled(bool enabled)
{
    EnsureLoaded();
    s_settings.stubModeEnabled = enabled;
}

bool SettingsStore::IsStubModeEnabled()
{
    EnsureLoaded();
    return s_settings.stubModeEnabled;
}

void SettingsStore::Save()
{
    EnsureLoaded();
    std::wstring appData = FileUtils::GetAppDataPath();
    if (appData.empty()) {
        return;
    }

    FileUtils::EnsureDirectoryExists(appData);
    std::wstring path = appData + L"\\settings.ini";

    std::wofstream file(path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        return;
    }

    file.imbue(std::locale::classic());
    file << L"apiKey=" << s_settings.apiKey << L"\n";
    file << L"stubMode=" << (s_settings.stubModeEnabled ? 1 : 0) << L"\n";
}

void SettingsStore::EnsureLoaded()
{
    if (s_loaded) {
        return;
    }
    s_loaded = true;

    std::wstring appData = FileUtils::GetAppDataPath();
    if (appData.empty()) {
        return;
    }

    std::wstring path = appData + L"\\settings.ini";
    std::wifstream file(path);
    if (!file.is_open()) {
        return;
    }

    std::wstring line;
    while (std::getline(file, line)) {
        line = TrimWhitespace(line);
        if (line.empty() || line[0] == L'#' || line[0] == L';') {
            continue;
        }

        size_t separator = line.find(L'=');
        if (separator == std::wstring::npos) {
            continue;
        }

        std::wstring key = TrimWhitespace(line.substr(0, separator));
        std::wstring value = TrimWhitespace(line.substr(separator + 1));

        if (key == L"apiKey") {
            s_settings.apiKey = value;
        } else if (key == L"stubMode") {
            s_settings.stubModeEnabled = (value == L"1" || StringEqualsIgnoreCase(value, L"true"));
        }
    }
}

std::wstring SettingsStore::SettingsFilePath()
{
    std::wstring appData = FileUtils::GetAppDataPath();
    if (appData.empty()) {
        return L"";
    }
    return appData + L"\\settings.ini";
}

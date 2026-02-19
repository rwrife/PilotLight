#include "PluginHost.h"
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

namespace {
    constexpr DWORD kMaxPluginOutputChars = 8192;
}

PluginHost::PluginHost()
{
    LoadPlugins();
}

PluginHost::~PluginHost()
{
    UnloadPlugins();
}

size_t PluginHost::LoadedPluginCount() const
{
    return m_plugins.size();
}

std::wstring PluginHost::ApplyUserMessageTransforms(const std::wstring& message) const
{
    std::wstring current = message;

    for (const auto& plugin : m_plugins) {
        if (!plugin.transformUserPrompt) {
            continue;
        }

        wchar_t buffer[kMaxPluginOutputChars] = { 0 };
        const BOOL handled = plugin.transformUserPrompt(current.c_str(), buffer, kMaxPluginOutputChars);
        if (handled && buffer[0] != L'\0') {
            current = buffer;
        }
    }

    return current;
}


std::wstring PluginHost::ApplyAssistantResponseTransforms(const std::wstring& response) const
{
    std::wstring current = response;

    for (const auto& plugin : m_plugins) {
        if (!plugin.transformAssistantResponse) {
            continue;
        }

        wchar_t buffer[kMaxPluginOutputChars] = { 0 };
        const BOOL handled = plugin.transformAssistantResponse(current.c_str(), buffer, kMaxPluginOutputChars);
        if (handled && buffer[0] != L'\0') {
            current = buffer;
        }
    }

    return current;
}

void PluginHost::LoadPlugins()
{
    const std::wstring pluginPattern = GetExecutableDirectory() + L"\\plugins\\*.dll";

    WIN32_FIND_DATAW findData = {};
    HANDLE findHandle = FindFirstFileW(pluginPattern.c_str(), &findData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        const std::wstring dllPath = GetExecutableDirectory() + L"\\plugins\\" + findData.cFileName;
        HMODULE module = LoadLibraryW(dllPath.c_str());
        if (!module) {
            continue;
        }

        auto transformPromptFn = reinterpret_cast<TransformUserPromptFn>(
            GetProcAddress(module, "PilotLight_TransformUserPrompt"));
        auto transformAssistantFn = reinterpret_cast<TransformAssistantResponseFn>(
            GetProcAddress(module, "PilotLight_TransformAssistantResponse"));

        if (!transformPromptFn && !transformAssistantFn) {
            FreeLibrary(module);
            continue;
        }

        Plugin plugin = {};
        plugin.module = module;
        plugin.name = findData.cFileName;
        plugin.transformUserPrompt = transformPromptFn;
        plugin.transformAssistantResponse = transformAssistantFn;
        m_plugins.push_back(plugin);

    } while (FindNextFileW(findHandle, &findData));

    FindClose(findHandle);
}

void PluginHost::UnloadPlugins()
{
    for (const auto& plugin : m_plugins) {
        if (plugin.module) {
            FreeLibrary(plugin.module);
        }
    }
    m_plugins.clear();
}

std::wstring PluginHost::GetExecutableDirectory() const
{
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    PathRemoveFileSpecW(path);
    return path;
}

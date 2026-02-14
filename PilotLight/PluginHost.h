#pragma once

#include <windows.h>
#include <string>
#include <vector>

class PluginHost {
public:
    PluginHost();
    ~PluginHost();

    std::wstring ApplyUserMessageTransforms(const std::wstring& message) const;
    size_t LoadedPluginCount() const;

private:
    typedef BOOL(WINAPI* TransformUserPromptFn)(LPCWSTR input, LPWSTR output, DWORD outputChars);

    struct Plugin {
        HMODULE module;
        std::wstring name;
        TransformUserPromptFn transformUserPrompt;
    };

    std::vector<Plugin> m_plugins;

    void LoadPlugins();
    void UnloadPlugins();
    std::wstring GetExecutableDirectory() const;
};

#include <windows.h>
#include <string>

extern "C" __declspec(dllexport)
BOOL WINAPI PilotLight_TransformUserPrompt(LPCWSTR input, LPWSTR output, DWORD outputChars)
{
    if (!input || !output || outputChars == 0) {
        return FALSE;
    }

    const std::wstring prefix = L"[sample-plugin] ";
    const std::wstring transformed = prefix + input;

    if (transformed.size() + 1 > outputChars) {
        return FALSE;
    }

    wcscpy_s(output, outputChars, transformed.c_str());
    return TRUE;
}

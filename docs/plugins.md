# PilotLight plugin plumbing (minimal)

PilotLight now includes a tiny plugin loader that discovers DLLs from:

- `<PilotLight.exe directory>\\plugins\\*.dll`

The core binary only loads plugins that expose a known symbol:

- `PilotLight_TransformUserPrompt`

## Hook currently available

### User prompt transform hook

Export this symbol from your plugin DLL:

```cpp
extern "C" __declspec(dllexport)
BOOL WINAPI PilotLight_TransformUserPrompt(
    LPCWSTR input,
    LPWSTR output,
    DWORD outputChars
);
```

Behavior:

- Return `TRUE` and write a non-empty string to `output` to replace the prompt.
- Return `FALSE` (or write empty output) to leave the prompt unchanged.
- Multiple plugins are applied in discovery order (`*.dll` order from Win32 file enumeration).

## Sample plugin stub

See `plugins/SamplePromptPrefixPlugin.cpp` for a tiny sample.

Build example (Developer Command Prompt):

```bat
cl /LD /EHsc SamplePromptPrefixPlugin.cpp /link /OUT:SamplePromptPrefixPlugin.dll
```

Copy the resulting DLL into:

- `<PilotLight.exe directory>\\plugins\\`

## Footprint notes

- No external dependencies added.
- Plugins are optional; app runs normally when no plugin DLLs are present.
- Loader is intentionally narrow (single hook) to keep core complexity and size low.

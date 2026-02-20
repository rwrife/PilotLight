# PilotLight plugin plumbing (minimal)

PilotLight includes a tiny plugin loader that discovers DLLs from:

- `<PilotLight.exe directory>\\plugins\\*.dll`

The core binary only loads plugins that expose at least one known symbol:

- `PilotLight_TransformUserPrompt`
- `PilotLight_TransformAssistantResponse`

## Hooks currently available

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

- Return `TRUE` and write a non-empty string to `output` to replace the outgoing user prompt.
- Return `FALSE` (or write empty output) to leave the prompt unchanged.

### Assistant response transform hook

Export this symbol from your plugin DLL:

```cpp
extern "C" __declspec(dllexport)
BOOL WINAPI PilotLight_TransformAssistantResponse(
    LPCWSTR input,
    LPWSTR output,
    DWORD outputChars
);
```

Behavior:

- Return `TRUE` and write a non-empty string to `output` to replace the assistant response before it is stored/rendered.
- Return `FALSE` (or write empty output) to leave the response unchanged.

### Ordering

- Multiple plugins are applied in discovery order (`*.dll` order from Win32 file enumeration).
- For each plugin, user-prompt transform runs on send and assistant-response transform runs after completion.

## Sample plugin stub

See `plugins/SamplePromptPrefixPlugin.cpp` for a tiny sample implementing both hooks.

Build example (Developer Command Prompt):

```bat
cl /LD /EHsc SamplePromptPrefixPlugin.cpp /link /OUT:SamplePromptPrefixPlugin.dll
```

Copy the resulting DLL into:

- `<PilotLight.exe directory>\\plugins\\`

## Footprint notes

- No external dependencies added.
- Plugins are optional; app runs normally when no plugin DLLs are present.
- Loader is intentionally narrow (two text hooks) to keep core complexity and size low.

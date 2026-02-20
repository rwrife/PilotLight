# <img src="logo.png" width="32px"> PilotLight

PilotLight is an extremely lightweight, native Windows chat client built in C++ with MFC, designed to deliver modern usability with minimal memory, CPU, and disk footprint.

The project intentionally avoids heavy frameworks, web runtimes, and background services in favor of a fast-starting, low-overhead native application.

## Project Goals

PilotLight is built around a few core principles:
- Minimal resource usage
  - Single native executable
  - Low idle memory footprint
  - Near-zero CPU usage when idle
- Native Windows experience
  - Fast startup
  - No web engines or runtime dependencies
  - High-DPI aware
  - Proper system integration
- Modern but simple UI
  - Borderless window with rounded corners
  - Dark theme
  - Rich text chat history
  - Clean, distraction-free layout
- LLM-agnostic backend
 - OpenAI-compatible API design
  - Works with OpenAI, Azure OpenAI, or local proxies

  ## LLM Integration

PilotLight is designed to be compatible with the OpenAI Chat Completions API and similar endpoints.

- API keys are provided at runtime via environment variables
- No secrets are embedded in the binary
- Endpoint support can be extended to:
  - Azure OpenAI
  - Local gateways
  - Self-hosted proxies

## Plugins (minimal plumbing)

PilotLight includes optional, lightweight plugin discovery for narrow extension hooks.

- Discovery path: `<PilotLight.exe dir>\\plugins\\*.dll`
- Current hooks: user prompt transform (`PilotLight_TransformUserPrompt`) and assistant response transform (`PilotLight_TransformAssistantResponse`)

See `docs/plugins.md` and `plugins/SamplePromptPrefixPlugin.cpp` for details.

## Agentic safety foundation

A lightweight tool confirmation dialog + in-memory permission helper is included for future tool execution flows.

See `docs/tool-confirmation.md` for usage and decision model.
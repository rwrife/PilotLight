# Build

- Always run ```msbuild PilotLight.slnx /p:Configuration=Debug /p:Platform=x64 /t:Build``` from the root folder to build the application on x86.

# Project Goals

- Build a lightweight Windows-native LLM chat client that looks modern, stays thin, and feels responsive even on modest hardware.
- Focus on giving the app agentic tooling and expected chat-client UX: search-friendly chat history, file-enabled input, and rich output from the model (formatted text, code blocks, etc.).
- The long-term vision is a modular, extensible thin client that can download plugins (DLLs) to safely add features. For now, keep that architecture in mind while shipping a beautiful, functional UI.
- Do **not** bake any real credentials into the repo. Instead, expose a settings panel where users can supply API keys/credentials, and optionally add test-mode switches or sample-data flags so we can preview the interface without hitting real services.

# Immediate Focus

- Make the UI feel modern: think polished colors, clean layout, intuitive spacing, and transitions where appropriate.
- Include the standard chat expectations (message list, user/assistant distinctions, quick reply affordances) plus the following:
  - A collapsible left-hand chat history panel that can be toggled open/closed without losing context.
  - A chat input area that supports text, attachments, and a file-picker button so users can upload reference material.
  - Support for agent-driven actions and tool calls in the prompt/responses, with rich-text rendering (hyperlinks, bold/italics) and nicely formatted code blocks. Don’t rely on clipboard tricks—render code cleanly inside the window.
  - Placeholders or sample-data flags so we can preview the history/input panels without real LLM responses.

# Platform Notes

- Use pilotlight’s codebase as the source of truth for anything Windows-specific (Win32/MFC).
- When adding features, double-check that no credentials or private keys are exposed; everything sensitive should live in user-entered settings.
- Document any new plugin hooks or extension points here so future contributors know where to plug in DLLs or other add-ins.

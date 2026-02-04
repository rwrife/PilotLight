# Desktop Copilot Research Log

This page records what we learned from today’s survey of modern desktop AI assistants so the cron jobs can refer back to it and we can keep iterating on PilotLight.

## Microsoft Copilot (Windows + Microsoft 365)
- Deep OS integration: taskbar shortcut, dedicated Copilot key, and sidebar that operates across the desktop.  The Windows 11/365 surface aims to be a lightweight overlay that can summarize documents, manipulate Office content, and orchestrate workflows without a bulky UI.
- Model foundation: built on Microsoft Prometheus (fine-tuned GPT-4/5 variants), with a freemium tier and advanced capabilities (custom chatbots, Copilot in the Microsoft 365 ribbon).
- Lessons for PilotLight: provide a fast, always-available hotkey, keep Chat/Tool UI minimal, and expose contextual actions (document summary, quick commands) while maintaining the lightweight client feel.
- Source: [Microsoft Copilot (Wikipedia)](https://en.wikipedia.org/wiki/Microsoft_Copilot)

## ChatGPT Desktop (OpenAI)
- Offers persistent chat history, side panels for contexts and settings, drag-and-drop file uploads, voice input, and plugin/tool access (browse, code interpreter, custom plugins) inside a native app.
- Instant summon shortcuts (Ctrl/Cmd+K), background notifications, and the ability to run tool calls in a tightly controlled way.
- Lessons: keep PilotLight small but show the user when a tool runs, allow easy file uploads and quick replies, and support toggles that load stub history vs. live API.
- Source: OpenAI launch coverage and public notes about the desktop app (2024 release).

## Other modern desktop bots (Claude Desktop, Perplexity, etc.)
- Rich UI polish with collapsible context/history panels, chips for quick replies, and explicit “actions” (web-browsing, search, code execution) that keep the assistant’s access transparent.
- Emphasis on “agentic tooling”: tools are encapsulated, user-approved, and logged in the conversation so the assistant can safely access local/remote resources.
- Lesson: create a plugin/tooling surface where each capability is explicitly granted (read files, run scripts, fetch system info), and show which tool fulfilled which part of the response.

## Agentic Tooling & Plugin Ideas for PilotLight
- Document extension points and plugin hooks (rendering pipeline, attachment handling, tool call routing) so we can safely load DLLs or executables later.
- Build a small “tool palette” inside the UI: toggles for local file search, script execution, or system info, each requiring explicit user confirmation and showing the tool’s name in the chat history.
- Align with the AGENTS.md “no secrets in repo” rule by exposing those capabilities via the settings panel or environment flags (PILOTLIGHT_OPENAI_API_KEY, stub/sample-data switch) and never hardcoding credentials.
- Maintain a stub/sample-data mode so the UI can be previewed locally without hitting real services.

## References
- Microsoft Copilot: https://en.wikipedia.org/wiki/Microsoft_Copilot
- ChatGPT Desktop overview (OpenAI marketing + help center)
- Public coverage of Claude/Perplexity desktop apps (general features noted above)

Continuous improvement note: keep updating this log with new observations, then link it from issues so the cron/PR jobs know what we’re iterating towards.
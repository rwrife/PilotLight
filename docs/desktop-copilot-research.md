# Desktop Copilot Research Notes (PilotLight)

This document captures the practical takeaways from modern desktop copilots (Microsoft Copilot, ChatGPT desktop, and similar tools) and translates them into a lightweight Windows-native plan for PilotLight.

## Why this exists

- Keep feature discussions grounded in real desktop UX expectations.
- Track what we should ship now vs. defer.
- Give cron-driven issue/PR automation a stable reference when prioritizing work.

## Key patterns observed

### 1) Fast summon / quick access
- Global hotkey or obvious launch path is expected.
- App should open quickly and be ready to type immediately.

PilotLight stance:
- **Ship:** fast startup and immediate input focus.
- **Defer:** global hotkey until we can add it without startup/background overhead.

### 2) Clear conversation structure
- Two-pane layouts and conversation navigation are common.
- Scroll behavior and spacing strongly affect perceived quality.

PilotLight stance:
- **Ship:** lightweight two-column shell and consistent chat spacing.
- **In progress:** polish for conversation controls and history ergonomics.

### 3) Safe tool actions
- Agentic tools are useful only when users can clearly see and approve actions.
- Permission prompts and action transparency are critical.

PilotLight stance:
- **Ship:** explicit confirmation UX for tool execution paths.
- **Defer:** broad tool/plugin surface until permission model is stable.

### 4) Attachments as first-class workflow
- Drag/drop + pre-send review is standard.
- Users expect visible, removable attachment chips/list before send.

PilotLight stance:
- **In progress:** compact attachment list with remove controls.
- **Next:** drag/drop hints and upload progress feedback.

### 5) Reliable offline/demo mode
- Stubbed data paths are useful for demos and UI iteration without API cost.
- Clear indication of non-live mode avoids confusion.

PilotLight stance:
- **Ship:** stub/sample-data toggles and safe defaults.
- **Ship:** runtime-configurable endpoint/key; no secrets in repo.

## What we are explicitly not doing (for now)

To stay small and maintainable:
- No embedded web runtime.
- No always-on background agent process.
- No heavy plugin runtime with broad OS access by default.

## PilotLight decision matrix

### Ship now (high value, low footprint)
- Input ergonomics (clipboard, keyboard, context menus).
- Settings + endpoint/key configuration with per-user storage.
- Stub/sample-data flows for demos.
- Attachment preview/remove basics.
- Focused UX polish for conversation controls.

### Defer until core is stable
- Global hotkey daemon/background service behavior.
- Broad plugin marketplace/runtime.
- Heavy telemetry/analytics infrastructure.

## Implementation guardrails

- Prefer native Win32/MFC patterns over new framework dependencies.
- Keep binaries small and startup path lean.
- Add focused, incremental PRs (one issue-sized step at a time).
- Validate on GitHub-hosted Windows CI before merge.

## How to use this file in future issue work

When opening/refining issues or PRs:
1. Reference this document in the issue description.
2. State whether work is **Ship now** or **Defer**.
3. Explain binary/complexity impact in one sentence.
4. Confirm Windows CI strategy for verification.

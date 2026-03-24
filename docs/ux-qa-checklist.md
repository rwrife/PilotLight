# PilotLight UX QA Checklist (Avalonia)

Use this checklist after changes to chat UX, settings, session handling, and attachments.

## Chat input and send behavior

- [ ] Text input supports copy/paste/cut/select-all shortcuts.
- [ ] Sending a message appends user message and assistant response in order.
- [ ] Empty sends are blocked unless attachments exist.
- [ ] Status text updates for loading/success/failure.

## Session management

- [ ] New Chat creates a fresh session.
- [ ] Session list selection changes visible message history.
- [ ] Session title initializes from first message.
- [ ] Session timestamps update after sends.

## Attachments

- [ ] Attach button opens file picker.
- [ ] Selected files appear in pending attachments list.
- [ ] Attachments are included as context in the outgoing request path.

## Settings and auth

- [ ] Endpoint/model values save and reload.
- [ ] API key mode and OAuth token mode toggle correctly.
- [ ] Credential fields are masked.

## Response interaction

- [ ] Copy button copies message content to clipboard.
- [ ] Long responses wrap and remain readable.

## Build and regression

- [ ] `dotnet build` succeeds locally.
- [ ] GitHub Actions x64 and ARM64 build artifacts succeed.

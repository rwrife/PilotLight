# PilotLight UX QA Checklist

Use this quick checklist after changes to chat chrome, input controls, or attachment flow.

## Input field behavior

- [ ] `Ctrl+C`, `Ctrl+V`, `Ctrl+X`, `Ctrl+A`, and `Ctrl+Z` work in the chat input.
- [ ] `Shift+Insert` pastes and `Shift+Delete` cuts.
- [ ] Right-click in the input shows context actions (Undo/Cut/Copy/Paste/Delete/Select All).
- [ ] Sending with `Enter` still works as expected (without breaking multiline behavior).

## Attachments and send controls

- [ ] Attach button opens file picker and selected files appear in the pending list.
- [ ] Drag-and-drop onto the window adds attachments.
- [ ] Double-click or delete/backspace on selected attachment removes it.
- [ ] Send button, attach button, and copy-last-response button show normal/hover/pressed/focus states.

## Conversation and layout behavior

- [ ] Chat autoscroll works for new assistant/user messages.
- [ ] If the user has scrolled up, new messages do not yank scroll unexpectedly.
- [ ] Window resize keeps title bar, chat surface, input area, and settings controls aligned.
- [ ] Settings panel blocks background chat interactions while open.

## DPI and accessibility sanity check

Run at 100%, 150%, and 200% Windows scaling (or closest available):

- [ ] Text remains legible in title bar, chat history, input, and settings.
- [ ] Buttons remain clickable with consistent hit targets.
- [ ] Focus indicators are visible for keyboard navigation.

## Regression notes

- [ ] Build passes locally (when on Windows) and on GitHub Actions.
- [ ] Any intentional UX deviations are documented in PR notes.

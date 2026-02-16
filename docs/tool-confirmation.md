# Tool Execution Confirmation (foundation)

PilotLight now includes a minimal modal dialog and in-memory permission store for tool execution requests.

## User choices

When a tool requests execution, UI code can call:

- `CToolConfirmationDialog::Prompt(toolName, actionSummary, parent)`

The dialog supports these outcomes:

- **Allow once** (`ToolConfirmationDecision::AllowOnce`)
- **Always allow** (`ToolConfirmationDecision::AlwaysAllow`)
- **Never allow** (`ToolConfirmationDecision::NeverAllow`)
- **Cancel** (`ToolConfirmationDecision::Cancel`)

## Policy helper

`ToolPermissionStore` tracks per-tool policies in memory:

- `Ask` (default)
- `AlwaysAllow`
- `NeverAllow`

Persistence is intentionally out of scope for this small, incremental change.

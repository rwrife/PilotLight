# <img src="logo.png" width="32px"> PilotLight

PilotLight is a lightweight cross-platform desktop chat client built with **C# + Avalonia UI**.

## What changed

This repository has been migrated from a legacy Windows-only MFC client to a modern Avalonia-based app focused on low dependency count and clean UX.

## Features

- Modern dark-themed chat interface
- Chat history with multiple sessions and quick new-chat creation
- OpenAI-compatible chat endpoint support
- Authentication options:
  - OpenAI API Key (Bearer)
  - OpenAI OAuth-style Access Token (Bearer)
- File attachments included as contextual text previews where supported
- Copy assistant or user responses from the conversation
- Per-user persisted settings and chat history
- Windows x64 + ARM64 CI builds

## Tech stack

- .NET 8
- Avalonia UI 11
- Minimal custom MVVM infrastructure (no heavy frameworks)

## Getting started

### Prerequisites

- .NET SDK 8.0+

### Run locally

```bash
dotnet restore
dotnet run --project /home/runner/work/PilotLight/PilotLight/PilotLight.Avalonia/PilotLight.Avalonia.csproj
```

### Build

```bash
dotnet build /home/runner/work/PilotLight/PilotLight/PilotLight.slnx -c Release
```

### Publish for Windows

```bash
dotnet publish /home/runner/work/PilotLight/PilotLight/PilotLight.Avalonia/PilotLight.Avalonia.csproj -c Release -r win-x64 --self-contained false -o ./artifacts/win-x64
dotnet publish /home/runner/work/PilotLight/PilotLight/PilotLight.Avalonia/PilotLight.Avalonia.csproj -c Release -r win-arm64 --self-contained false -o ./artifacts/win-arm64
```

## Settings

Use the Settings panel in the app to configure:

- Endpoint (default: `https://api.openai.com/v1/chat/completions`)
- Model
- Authentication mode (API key or OAuth access token)
- Credential/token value

Settings and history are stored per-user in the application data folder under `PilotLight`.

⚠️ Never commit keys or tokens.

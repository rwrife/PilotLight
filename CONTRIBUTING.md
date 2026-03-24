# Contributing to PilotLight

## Development environment

- .NET SDK 8.0+
- Any IDE with C# and Avalonia support (Visual Studio, Rider, VS Code + C# Dev Kit)

## Build and run

```bash
dotnet restore
dotnet build /home/runner/work/PilotLight/PilotLight/PilotLight.slnx -c Debug
dotnet run --project /home/runner/work/PilotLight/PilotLight/PilotLight.Avalonia/PilotLight.Avalonia.csproj
```

## Pull requests

- Keep changes focused and minimal
- Update docs when behavior changes
- Ensure CI workflows pass for x64 and ARM64 builds
- Do not commit secrets (API keys/tokens)

## Architecture notes

- `PilotLight.Avalonia`: UI layer and view models
- `PilotLight.Core`: chat/domain models and service abstractions/implementations
- Reusable UI pieces live under `PilotLight.Avalonia/Controls`

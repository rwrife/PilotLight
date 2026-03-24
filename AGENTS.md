# Build

- Preferred local build command:
  - `dotnet build /home/runner/work/PilotLight/PilotLight/PilotLight.slnx -c Debug`

- Legacy instruction retained for compatibility with prior automation:
  - `msbuild PilotLight.slnx /p:Configuration=Debug /p:Platform=x64 /t:Build`

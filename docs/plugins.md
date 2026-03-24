# PilotLight extension model (Avalonia migration)

The legacy C++ DLL plugin mechanism has been removed during the Avalonia migration.

Current focus is a lightweight, stable core chat UX with minimal dependencies.

If extension points are reintroduced, they will use a managed .NET plugin contract with explicit versioning and sandboxing boundaries.

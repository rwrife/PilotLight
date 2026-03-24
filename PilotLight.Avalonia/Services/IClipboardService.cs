namespace PilotLight.Avalonia.Services;

public interface IClipboardService
{
    Task SetTextAsync(string text, CancellationToken cancellationToken);
}

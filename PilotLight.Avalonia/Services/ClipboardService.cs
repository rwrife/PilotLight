using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;

namespace PilotLight.Avalonia.Services;

public sealed class ClipboardService : IClipboardService
{
    public async Task SetTextAsync(string text, CancellationToken cancellationToken)
    {
        var desktop = Application.Current?.ApplicationLifetime as IClassicDesktopStyleApplicationLifetime;
        var clipboard = desktop?.MainWindow?.Clipboard;
        if (clipboard is null)
        {
            return;
        }

        cancellationToken.ThrowIfCancellationRequested();
        await clipboard.SetTextAsync(text);
    }
}

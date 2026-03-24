using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using PilotLight.Avalonia.Services;
using PilotLight.Avalonia.ViewModels;
using PilotLight.Core.Services;

namespace PilotLight.Avalonia;

public partial class MainWindow : Window
{
    private readonly MainWindowViewModel _viewModel;

    public MainWindow()
    {
        InitializeComponent();

        var basePath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "PilotLight");
        var settingsStore = new JsonSettingsStore(basePath);
        var historyStore = new JsonChatHistoryStore(basePath);
        var chatClient = new OpenAIChatClient(new HttpClient());
        var clipboard = new ClipboardService();

        _viewModel = new MainWindowViewModel(settingsStore, historyStore, chatClient, clipboard);
        DataContext = _viewModel;

        Opened += OnOpened;
        Closing += OnClosing;
    }

    private async void OnOpened(object? sender, EventArgs e)
    {
        await _viewModel.InitializeAsync(CancellationToken.None);
    }

    private void OnClosing(object? sender, WindowClosingEventArgs e)
    {
        _viewModel.CancelPendingOperations();
        _viewModel.PersistAsync(CancellationToken.None).GetAwaiter().GetResult();
    }

    private async void OnAttachClick(object? sender, RoutedEventArgs e)
    {
        if (StorageProvider is null)
        {
            return;
        }

        var files = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Attach files",
            AllowMultiple = true
        });

        if (files.Count == 0)
        {
            return;
        }

        var paths = files
            .Where(f => f.TryGetLocalPath() is not null)
            .Select(f => f.TryGetLocalPath()!)
            .ToArray();

        _viewModel.AddPendingAttachments(paths);
    }
}

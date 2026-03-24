using System.Collections.ObjectModel;
using System.Windows.Input;
using PilotLight.Avalonia.Services;
using PilotLight.Core.Models;
using PilotLight.Core.Services;
using PilotLight.Core.Utilities;

namespace PilotLight.Avalonia.ViewModels;

public sealed class MainWindowViewModel : ObservableObject
{
    private readonly ISettingsStore _settingsStore;
    private readonly IChatHistoryStore _chatHistoryStore;
    private readonly IOpenAIChatClient _chatClient;
    private readonly IClipboardService _clipboardService;

    private ChatSessionItemViewModel? _selectedSession;
    private string _draftMessage = string.Empty;
    private string _statusText = "Ready.";
    private string _endpoint = string.Empty;
    private string _model = string.Empty;
    private AuthMode _selectedAuthMode = AuthMode.ApiKey;
    private string _apiKey = string.Empty;
    private string _oauthAccessToken = string.Empty;
    private bool _isBusy;

    public MainWindowViewModel(
        ISettingsStore settingsStore,
        IChatHistoryStore chatHistoryStore,
        IOpenAIChatClient chatClient,
        IClipboardService clipboardService)
    {
        _settingsStore = settingsStore;
        _chatHistoryStore = chatHistoryStore;
        _chatClient = chatClient;
        _clipboardService = clipboardService;

        Sessions = [];
        CurrentMessages = [];
        PendingAttachments = [];
        AuthModes = Enum.GetValues<AuthMode>();

        NewChatCommand = new RelayCommand(CreateNewChat);
        SendMessageCommand = new AsyncRelayCommand(SendMessageAsync, () => CanSendMessage);
        SaveSettingsCommand = new AsyncRelayCommand(SaveSettingsAsync);
        CopyMessageCommand = new AsyncRelayCommand<ChatMessageItemViewModel>(CopyMessageAsync, m => m is not null);
    }

    public ObservableCollection<ChatSessionItemViewModel> Sessions { get; }
    public ObservableCollection<ChatMessageItemViewModel> CurrentMessages { get; }
    public ObservableCollection<Attachment> PendingAttachments { get; }

    public IReadOnlyList<AuthMode> AuthModes { get; }

    public ChatSessionItemViewModel? SelectedSession
    {
        get => _selectedSession;
        set
        {
            if (!SetProperty(ref _selectedSession, value))
            {
                return;
            }

            RefreshCurrentMessages();
        }
    }

    public string DraftMessage
    {
        get => _draftMessage;
        set
        {
            if (SetProperty(ref _draftMessage, value))
            {
                RaisePropertyChanged(nameof(CanSendMessage));
                NotifyCommands();
            }
        }
    }

    public string StatusText
    {
        get => _statusText;
        private set => SetProperty(ref _statusText, value);
    }

    public string Endpoint
    {
        get => _endpoint;
        set => SetProperty(ref _endpoint, value);
    }

    public string Model
    {
        get => _model;
        set => SetProperty(ref _model, value);
    }

    public AuthMode SelectedAuthMode
    {
        get => _selectedAuthMode;
        set
        {
            if (SetProperty(ref _selectedAuthMode, value))
            {
                RaisePropertyChanged(nameof(IsApiKeyMode));
                RaisePropertyChanged(nameof(IsOAuthMode));
            }
        }
    }

    public string ApiKey
    {
        get => _apiKey;
        set => SetProperty(ref _apiKey, value);
    }

    public string OAuthAccessToken
    {
        get => _oauthAccessToken;
        set => SetProperty(ref _oauthAccessToken, value);
    }

    public bool IsApiKeyMode => SelectedAuthMode == AuthMode.ApiKey;
    public bool IsOAuthMode => SelectedAuthMode == AuthMode.OAuthAccessToken;
    public bool HasPendingAttachments => PendingAttachments.Count > 0;

    public bool CanSendMessage => !_isBusy && SelectedSession is not null && (!string.IsNullOrWhiteSpace(DraftMessage) || PendingAttachments.Count > 0);

    public ICommand NewChatCommand { get; }
    public ICommand SendMessageCommand { get; }
    public ICommand SaveSettingsCommand { get; }
    public ICommand CopyMessageCommand { get; }

    public async Task InitializeAsync(CancellationToken cancellationToken)
    {
        var settings = await _settingsStore.LoadAsync(cancellationToken);
        Endpoint = settings.Endpoint;
        Model = settings.Model;
        SelectedAuthMode = settings.AuthMode;
        ApiKey = settings.ApiKey;
        OAuthAccessToken = settings.OAuthAccessToken;

        var sessions = await _chatHistoryStore.LoadAsync(cancellationToken);
        Sessions.Clear();

        foreach (var session in sessions.OrderByDescending(s => s.UpdatedAt))
        {
            Sessions.Add(new ChatSessionItemViewModel(session));
        }

        if (Sessions.Count == 0)
        {
            CreateNewChat();
        }
        else
        {
            SelectedSession = Sessions[0];
        }

        StatusText = "Loaded settings and chat history.";
    }

    public void AddPendingAttachments(IEnumerable<string> filePaths)
    {
        foreach (var filePath in filePaths.Where(File.Exists).Distinct(StringComparer.OrdinalIgnoreCase))
        {
            PendingAttachments.Add(new Attachment
            {
                FilePath = filePath,
                DisplayName = Path.GetFileName(filePath)
            });
        }

        RaisePropertyChanged(nameof(HasPendingAttachments));
        RaisePropertyChanged(nameof(CanSendMessage));
        NotifyCommands();
    }

    public async Task PersistAsync(CancellationToken cancellationToken)
    {
        await _settingsStore.SaveAsync(BuildSettings(), cancellationToken);
        await _chatHistoryStore.SaveAsync(Sessions.Select(s => s.Model).ToList(), cancellationToken);
    }

    private void CreateNewChat()
    {
        var session = new ChatSession
        {
            Title = "New Chat"
        };

        var vm = new ChatSessionItemViewModel(session);
        Sessions.Insert(0, vm);
        SelectedSession = vm;
        StatusText = "Created new chat session.";
    }

    private async Task SendMessageAsync()
    {
        if (SelectedSession is null)
        {
            return;
        }

        var userText = DraftMessage.Trim();
        var attachments = PendingAttachments.ToList();
        if (string.IsNullOrWhiteSpace(userText) && attachments.Count == 0)
        {
            return;
        }

        var userMessage = new ChatMessage
        {
            Role = ChatRole.User,
            Content = userText,
            Attachments = attachments
        };

        SelectedSession.Model.Messages.Add(userMessage);
        SelectedSession.Model.UpdatedAt = DateTimeOffset.UtcNow;

        if (SelectedSession.Model.Title == "New Chat" && !string.IsNullOrWhiteSpace(userText))
        {
            SelectedSession.Model.Title = userText.Length > 36 ? userText[..36] + "…" : userText;
            SelectedSession.RefreshFromModel();
        }

        DraftMessage = string.Empty;
        PendingAttachments.Clear();
        RaisePropertyChanged(nameof(HasPendingAttachments));

        RefreshCurrentMessages();

        _isBusy = true;
        RaisePropertyChanged(nameof(CanSendMessage));
        NotifyCommands();
        StatusText = "Sending request...";

        try
        {
            var messageBuffer = SelectedSession.Model.Messages.ToList();
            if (attachments.Count > 0)
            {
                var attachmentContext = await FileAttachmentFormatter.BuildAttachmentContextAsync(attachments, CancellationToken.None);
                if (!string.IsNullOrWhiteSpace(attachmentContext))
                {
                    messageBuffer.Add(new ChatMessage { Role = ChatRole.System, Content = attachmentContext });
                }
            }

            var response = await _chatClient.GetAssistantResponseAsync(BuildSettings(), messageBuffer, CancellationToken.None);
            SelectedSession.Model.Messages.Add(new ChatMessage
            {
                Role = ChatRole.Assistant,
                Content = response
            });
            SelectedSession.Model.UpdatedAt = DateTimeOffset.UtcNow;
            StatusText = "Response received.";
        }
        catch (Exception ex)
        {
            SelectedSession.Model.Messages.Add(new ChatMessage
            {
                Role = ChatRole.Assistant,
                Content = $"Request failed: {ex.Message}"
            });
            StatusText = "Request failed.";
        }
        finally
        {
            _isBusy = false;
            RaisePropertyChanged(nameof(CanSendMessage));
            NotifyCommands();
            SelectedSession.RefreshFromModel();
            RefreshCurrentMessages();
            await PersistAsync(CancellationToken.None);
        }
    }

    private async Task SaveSettingsAsync()
    {
        await _settingsStore.SaveAsync(BuildSettings(), CancellationToken.None);
        StatusText = "Settings saved.";
    }

    private async Task CopyMessageAsync(ChatMessageItemViewModel? message)
    {
        if (message is null)
        {
            return;
        }

        await _clipboardService.SetTextAsync(message.Content, CancellationToken.None);
        StatusText = "Copied message to clipboard.";
    }

    private AppSettings BuildSettings() => new()
    {
        Endpoint = Endpoint,
        Model = Model,
        AuthMode = SelectedAuthMode,
        ApiKey = ApiKey,
        OAuthAccessToken = OAuthAccessToken
    };

    private void RefreshCurrentMessages()
    {
        CurrentMessages.Clear();

        if (SelectedSession is null)
        {
            return;
        }

        foreach (var message in SelectedSession.Model.Messages)
        {
            CurrentMessages.Add(new ChatMessageItemViewModel { Model = message });
        }

        RaisePropertyChanged(nameof(CanSendMessage));
        NotifyCommands();
    }

    private void NotifyCommands()
    {
        (SendMessageCommand as AsyncRelayCommand)?.NotifyCanExecuteChanged();
        (CopyMessageCommand as AsyncRelayCommand<ChatMessageItemViewModel>)?.NotifyCanExecuteChanged();
    }
}

using PilotLight.Core.Models;

namespace PilotLight.Avalonia.ViewModels;

public sealed class ChatSessionItemViewModel : ObservableObject
{
    private string _title;

    public ChatSessionItemViewModel(ChatSession model)
    {
        Model = model;
        _title = model.Title;
    }

    public ChatSession Model { get; }

    public string Title
    {
        get => _title;
        set => SetProperty(ref _title, value);
    }

    public string UpdatedText => Model.UpdatedAt.LocalDateTime.ToString("g");

    public void RefreshFromModel()
    {
        Title = Model.Title;
        RaisePropertyChanged(nameof(UpdatedText));
    }
}

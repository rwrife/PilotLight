using PilotLight.Core.Models;

namespace PilotLight.Avalonia.ViewModels;

public sealed class ChatMessageItemViewModel
{
    public required ChatMessage Model { get; init; }

    public string RoleLabel => Model.Role switch
    {
        ChatRole.User => "You",
        ChatRole.Assistant => "Assistant",
        _ => "System"
    };

    public bool IsAssistant => Model.Role == ChatRole.Assistant;
    public bool IsUser => Model.Role == ChatRole.User;
    public string Content => Model.Content;
    public string Timestamp => Model.CreatedAt.LocalDateTime.ToString("g");
    public IReadOnlyList<Attachment> Attachments => Model.Attachments;
}

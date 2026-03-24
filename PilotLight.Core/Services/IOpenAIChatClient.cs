using PilotLight.Core.Models;

namespace PilotLight.Core.Services;

public interface IOpenAIChatClient
{
    Task<string> GetAssistantResponseAsync(AppSettings settings, IReadOnlyList<ChatMessage> messages, CancellationToken cancellationToken);
}

using PilotLight.Core.Models;

namespace PilotLight.Core.Services;

public interface IChatHistoryStore
{
    Task<IReadOnlyList<ChatSession>> LoadAsync(CancellationToken cancellationToken);
    Task SaveAsync(IReadOnlyList<ChatSession> sessions, CancellationToken cancellationToken);
}

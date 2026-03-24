using System.Text.Json;
using PilotLight.Core.Models;

namespace PilotLight.Core.Services;

public sealed class JsonChatHistoryStore : IChatHistoryStore
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase
    };

    private readonly string _historyFilePath;

    public JsonChatHistoryStore(string baseDirectory)
    {
        Directory.CreateDirectory(baseDirectory);
        _historyFilePath = Path.Combine(baseDirectory, "chats.json");
    }

    public async Task<IReadOnlyList<ChatSession>> LoadAsync(CancellationToken cancellationToken)
    {
        if (!File.Exists(_historyFilePath))
        {
            return [];
        }

        await using var stream = File.OpenRead(_historyFilePath);
        var sessions = await JsonSerializer.DeserializeAsync<List<ChatSession>>(stream, JsonOptions, cancellationToken).ConfigureAwait(false);
        return sessions ?? [];
    }

    public async Task SaveAsync(IReadOnlyList<ChatSession> sessions, CancellationToken cancellationToken)
    {
        await using var stream = File.Create(_historyFilePath);
        await JsonSerializer.SerializeAsync(stream, sessions, JsonOptions, cancellationToken).ConfigureAwait(false);
    }
}

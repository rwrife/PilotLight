using System.Net.Http.Headers;
using System.Text;
using System.Text.Json;
using PilotLight.Core.Models;

namespace PilotLight.Core.Services;

public sealed class OpenAIChatClient(HttpClient httpClient) : IOpenAIChatClient
{
    public async Task<string> GetAssistantResponseAsync(AppSettings settings, IReadOnlyList<ChatMessage> messages, CancellationToken cancellationToken)
    {
        var token = settings.ResolveBearerToken().Trim();
        if (string.IsNullOrWhiteSpace(token))
        {
            throw new InvalidOperationException("A valid API key or OAuth access token is required.");
        }

        if (!Uri.TryCreate(settings.Endpoint, UriKind.Absolute, out var endpointUri))
        {
            throw new InvalidOperationException("The OpenAI endpoint is invalid.");
        }

        using var request = new HttpRequestMessage(HttpMethod.Post, endpointUri);
        request.Headers.Authorization = new AuthenticationHeaderValue("Bearer", token);

        var payload = new
        {
            model = string.IsNullOrWhiteSpace(settings.Model) ? "gpt-4.1-mini" : settings.Model,
            messages = messages.Select(m => new
            {
                role = m.Role.ToString().ToLowerInvariant(),
                content = m.Content
            })
        };

        request.Content = new StringContent(JsonSerializer.Serialize(payload), Encoding.UTF8, "application/json");
        using var response = await httpClient.SendAsync(request, cancellationToken).ConfigureAwait(false);
        var json = await response.Content.ReadAsStringAsync(cancellationToken).ConfigureAwait(false);

        if (!response.IsSuccessStatusCode)
        {
            throw new InvalidOperationException($"OpenAI request failed ({(int)response.StatusCode}): {json}");
        }

        using var document = JsonDocument.Parse(json);
        var root = document.RootElement;
        if (!root.TryGetProperty("choices", out var choices) || choices.GetArrayLength() == 0)
        {
            throw new InvalidOperationException("OpenAI response did not include choices.");
        }

        var content = choices[0].GetProperty("message").GetProperty("content").GetString();
        if (string.IsNullOrWhiteSpace(content))
        {
            throw new InvalidOperationException("OpenAI response message was empty.");
        }

        return content;
    }
}

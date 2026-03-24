namespace PilotLight.Core.Models;

public sealed class AppSettings
{
    public string Endpoint { get; set; } = "https://api.openai.com/v1/chat/completions";
    public string Model { get; set; } = "gpt-4.1-mini";
    public AuthMode AuthMode { get; set; } = AuthMode.ApiKey;
    public string ApiKey { get; set; } = string.Empty;
    public string OAuthAccessToken { get; set; } = string.Empty;

    public string ResolveBearerToken() => AuthMode == AuthMode.ApiKey ? ApiKey : OAuthAccessToken;
}

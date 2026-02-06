#include "OpenAIClient.h"
#include "JsonBuilder.h"
#include "SettingsStore.h"
#include <windows.h>
#include <winhttp.h>
#include <sstream>

#pragma comment(lib, "winhttp.lib")

std::wstring OpenAIClient::Endpoint()
{
    return L"https://api.openai.com/v1/chat/completions";
}

std::wstring OpenAIClient::ApiKey()
{
    const auto& settings = SettingsStore::Get();
    if (!settings.apiKey.empty()) {
        return settings.apiKey;
    }

    wchar_t buffer[512] = {0};
    GetEnvironmentVariableW(L"PILOTLIGHT_OPENAI_API_KEY", buffer, 512);
    return buffer;
}

std::wstring OpenAIClient::SerializeMessages(const std::vector<ChatMessage>& messages)
{
    JsonBuilder json;
    json.BeginObject();
    json.AddString(L"model", L"gpt-4o-mini");
    json.BeginArray(L"messages");

    for (const auto& msg : messages) {
        json.BeginObject();
        json.AddString(L"role", msg.RoleToString().c_str());
        json.AddString(L"content", msg.content.c_str());
        json.EndObject();
    }

    json.EndArray();
    json.EndObject();

    return json.ToString();
}

std::wstring OpenAIClient::SendHttpRequest(const std::wstring& jsonBody)
{
    std::wstring apiKey = ApiKey();
    if (apiKey.empty()) {
        return L"Error: PILOTLIGHT_OPENAI_API_KEY environment variable not set.";
    }

    // Parse URL
    URL_COMPONENTS urlComp = {0};
    urlComp.dwStructSize = sizeof(urlComp);
    wchar_t hostName[256] = {0};
    wchar_t urlPath[256] = {0};
    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = 256;
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = 256;

    std::wstring endpoint = Endpoint();
    if (!WinHttpCrackUrl(endpoint.c_str(), 0, 0, &urlComp)) {
        return L"Error: Invalid endpoint URL.";
    }

    // Open session
    HINTERNET hSession = WinHttpOpen(L"PilotLight/1.0",
                                      WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                      WINHTTP_NO_PROXY_NAME,
                                      WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        return L"Error: Failed to open HTTP session.";
    }

    // Connect
    HINTERNET hConnect = WinHttpConnect(hSession, hostName, urlComp.nPort, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return L"Error: Failed to connect to server.";
    }

    // Open request
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", urlPath,
                                             nullptr, WINHTTP_NO_REFERER,
                                             WINHTTP_DEFAULT_ACCEPT_TYPES,
                                             WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return L"Error: Failed to open HTTP request.";
    }

    // Set headers
    std::wstring authHeader = L"Authorization: Bearer " + apiKey;
    std::wstring contentType = L"Content-Type: application/json";
    
    WinHttpAddRequestHeaders(hRequest, authHeader.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);
    WinHttpAddRequestHeaders(hRequest, contentType.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);

    // Convert JSON to UTF-8
    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, jsonBody.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8Body(utf8Length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, jsonBody.c_str(), -1, &utf8Body[0], utf8Length, nullptr, nullptr);

    // Send request
    BOOL bResult = WinHttpSendRequest(hRequest,
                                       WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                       (LPVOID)utf8Body.c_str(), utf8Body.length() - 1,
                                       utf8Body.length() - 1, 0);

    if (!bResult || !WinHttpReceiveResponse(hRequest, nullptr)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return L"Error: Failed to send request or receive response.";
    }

    // Read response
    std::string response;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    
    do {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
        if (dwSize == 0) break;

        std::vector<char> buffer(dwSize + 1, 0);
        if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) break;
        
        response.append(buffer.data(), dwDownloaded);
    } while (dwSize > 0);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    // Convert response to wide string
    int wideLength = MultiByteToWideChar(CP_UTF8, 0, response.c_str(), -1, nullptr, 0);
    std::wstring wideResponse(wideLength, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, response.c_str(), -1, &wideResponse[0], wideLength);

    return wideResponse;
}

std::wstring OpenAIClient::ParseResponse(const std::wstring& jsonResponse)
{
    // Basic JSON parsing - extract content from response
    size_t contentPos = jsonResponse.find(L"\"content\":\"");
    if (contentPos == std::wstring::npos) {
        return L"Error: Could not parse response.";
    }

    contentPos += 11;  // Length of "content":""
    size_t contentEnd = jsonResponse.find(L"\"", contentPos);
    if (contentEnd == std::wstring::npos) {
        return L"Error: Could not parse response.";
    }

    std::wstring content = jsonResponse.substr(contentPos, contentEnd - contentPos);

    // Unescape JSON strings
    std::wstring result;
    for (size_t i = 0; i < content.length(); ++i) {
        if (content[i] == L'\\' && i + 1 < content.length()) {
            wchar_t next = content[i + 1];
            if (next == L'n') { result += L'\n'; i++; }
            else if (next == L'r') { result += L'\r'; i++; }
            else if (next == L't') { result += L'\t'; i++; }
            else if (next == L'\\') { result += L'\\'; i++; }
            else if (next == L'"') { result += L'"'; i++; }
            else { result += content[i]; }
        } else {
            result += content[i];
        }
    }

    return result;
}

std::wstring OpenAIClient::Complete(const std::vector<ChatMessage>& messages)
{
    if (SettingsStore::IsStubModeEnabled()) {
        std::wstring stubResponse = L"(Stub) Running in sample mode, so no OpenAI request was issued. ";
        if (!messages.empty()) {
            stubResponse += L"You asked: \"";
            stubResponse += messages.back().content;
            stubResponse += L"\". ";
        }
        stubResponse += L"Configure PILOTLIGHT_OPENAI_API_KEY in Settings to talk to the API.\n";
        stubResponse += L"Stub responses are deterministic and fast for local testing.";
        return stubResponse;
    }

    std::wstring jsonBody = SerializeMessages(messages);
    std::wstring jsonResponse = SendHttpRequest(jsonBody);
    
    if (jsonResponse.find(L"Error:") == 0) {
        return jsonResponse;
    }
    
    return ParseResponse(jsonResponse);
}

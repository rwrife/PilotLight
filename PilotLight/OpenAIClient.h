#pragma once
#include <string>
#include <vector>
#include "ChatMessage.h"

class OpenAIClient {
public:
    std::wstring Complete(const std::vector<ChatMessage>& messages);

private:
    std::wstring Endpoint();
    std::wstring ApiKey();
    std::wstring SerializeMessages(const std::vector<ChatMessage>& messages);
    std::wstring SendHttpRequest(const std::wstring& jsonBody);
    std::wstring ParseResponse(const std::wstring& jsonResponse);
};

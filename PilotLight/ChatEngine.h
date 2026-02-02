#pragma once
#include <string>
#include "ChatMessage.h"
#include "ChatHistory.h"

class ChatEngine {
public:
    ChatEngine();
    ~ChatEngine();

    void AddUserMessage(const std::wstring& content, const std::vector<FileAttachment>& attachments);
    ChatMessage GetAssistantResponse();
    ChatHistory& GetHistory();
    void ClearHistory();

private:
    ChatHistory m_history;
    void InitializeSystemMessage();
};

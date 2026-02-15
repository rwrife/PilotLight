#pragma once
#include <string>
#include "ChatMessage.h"
#include "ChatHistory.h"
#include "PluginHost.h"

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
    PluginHost m_pluginHost;
    void InitializeSystemMessage();
};

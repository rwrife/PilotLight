#pragma once
#include "ChatMessage.h"
#include <vector>
#include <string>

// Chat history container with persistence
class ChatHistory {
public:
    ChatHistory();
    ~ChatHistory();

    void AddMessage(const ChatMessage& msg);
    const std::vector<ChatMessage>& GetMessages() const;
    void Clear();
    
    bool SaveToFile(const std::wstring& path);
    bool LoadFromFile(const std::wstring& path);

private:
    std::vector<ChatMessage> m_messages;
};
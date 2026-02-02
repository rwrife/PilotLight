#include "ChatHistory.h"
#include <fstream>
#include <sstream>
#include <windows.h>
#include <shlobj.h>

ChatHistory::ChatHistory()
{
}

ChatHistory::~ChatHistory()
{
}

void ChatHistory::AddMessage(const ChatMessage& msg)
{
    m_messages.push_back(msg);
}

const std::vector<ChatMessage>& ChatHistory::GetMessages() const
{
    return m_messages;
}

void ChatHistory::Clear()
{
    m_messages.clear();
}

bool ChatHistory::SaveToFile(const std::wstring& path)
{
    std::wofstream file(path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    file << L"[\n";
    for (size_t i = 0; i < m_messages.size(); ++i) {
        if (i > 0) file << L",\n";
        file << L"  " << m_messages[i].ToJson();
    }
    file << L"\n]\n";

    file.close();
    return true;
}

bool ChatHistory::LoadFromFile(const std::wstring& path)
{
    std::wifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    m_messages.clear();

    std::wstring content;
    std::wstring line;
    while (std::getline(file, line)) {
        content += line + L"\n";
    }
    file.close();

    // Basic parsing - extract JSON objects between { and }
    size_t pos = 0;
    while ((pos = content.find(L'{', pos)) != std::wstring::npos) {
        size_t end = content.find(L'}', pos);
        if (end == std::wstring::npos) break;
        
        std::wstring msgJson = content.substr(pos, end - pos + 1);
        ChatMessage msg = ChatMessage::FromJson(msgJson);
        m_messages.push_back(msg);
        
        pos = end + 1;
    }

    return true;
}
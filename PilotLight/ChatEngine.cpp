#include "ChatEngine.h"
#include "OpenAIClient.h"

ChatEngine::ChatEngine()
{
    InitializeSystemMessage();
}

ChatEngine::~ChatEngine()
{
}

void ChatEngine::InitializeSystemMessage()
{
    // Add system message if history is empty
    if (m_history.GetMessages().empty()) {
        ChatMessage systemMsg(ChatMessage::Role::System, 
            L"You are PilotLight, a helpful AI assistant.");
        m_history.AddMessage(systemMsg);
    }
}

void ChatEngine::AddUserMessage(const std::wstring& content, const std::vector<FileAttachment>& attachments)
{
    ChatMessage msg(ChatMessage::Role::User, content);
    msg.attachments = attachments;
    m_history.AddMessage(msg);
}

ChatMessage ChatEngine::GetAssistantResponse()
{
    OpenAIClient client;
    std::wstring response = client.Complete(m_history.GetMessages());
    
    ChatMessage assistantMsg(ChatMessage::Role::Assistant, response);
    m_history.AddMessage(assistantMsg);
    
    return assistantMsg;
}

ChatHistory& ChatEngine::GetHistory()
{
    return m_history;
}

void ChatEngine::ClearHistory()
{
    m_history.Clear();
    InitializeSystemMessage();
}

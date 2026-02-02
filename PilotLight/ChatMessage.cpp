#include "ChatMessage.h"
#include <sstream>

std::wstring ChatMessage::RoleToString() const
{
    switch (role) {
        case Role::System: return L"system";
        case Role::User: return L"user";
        case Role::Assistant: return L"assistant";
        default: return L"user";
    }
}

ChatMessage::Role ChatMessage::StringToRole(const std::wstring& str)
{
    if (str == L"system") return Role::System;
    if (str == L"assistant") return Role::Assistant;
    return Role::User;
}

// Simple JSON escaping for strings
static std::wstring EscapeJson(const std::wstring& str)
{
    std::wstring result;
    result.reserve(str.length() + 10);
    
    for (wchar_t c : str) {
        switch (c) {
            case L'\\': result += L"\\\\"; break;
            case L'"': result += L"\\\""; break;
            case L'\n': result += L"\\n"; break;
            case L'\r': result += L"\\r"; break;
            case L'\t': result += L"\\t"; break;
            default: result += c; break;
        }
    }
    
    return result;
}

std::wstring ChatMessage::ToJson() const
{
    std::wostringstream oss;
    oss << L"{";
    oss << L"\"role\":\"" << RoleToString() << L"\",";
    oss << L"\"content\":\"" << EscapeJson(content) << L"\"";
    
    // Add attachments if present
    if (!attachments.empty()) {
        oss << L",\"attachments\":[";
        for (size_t i = 0; i < attachments.size(); ++i) {
            if (i > 0) oss << L",";
            oss << L"{";
            oss << L"\"filename\":\"" << EscapeJson(attachments[i].filename) << L"\",";
            oss << L"\"mimeType\":\"" << EscapeJson(attachments[i].mimeType) << L"\",";
            oss << L"\"size\":" << attachments[i].originalSize;
            oss << L"}";
        }
        oss << L"]";
    }
    
    oss << L"}";
    return oss.str();
}

ChatMessage ChatMessage::FromJson(const std::wstring& json)
{
    // Basic JSON parsing - for full implementation, would need proper parser
    // This is a placeholder for the persistence layer
    ChatMessage msg;
    
    // Extract role
    size_t rolePos = json.find(L"\"role\":\"");
    if (rolePos != std::wstring::npos) {
        rolePos += 8;
        size_t roleEnd = json.find(L"\"", rolePos);
        if (roleEnd != std::wstring::npos) {
            msg.role = StringToRole(json.substr(rolePos, roleEnd - rolePos));
        }
    }
    
    // Extract content
    size_t contentPos = json.find(L"\"content\":\"");
    if (contentPos != std::wstring::npos) {
        contentPos += 11;
        size_t contentEnd = json.find(L"\"", contentPos);
        if (contentEnd != std::wstring::npos) {
            msg.content = json.substr(contentPos, contentEnd - contentPos);
        }
    }
    
    return msg;
}
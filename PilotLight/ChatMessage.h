#pragma once
#include <string>
#include <vector>
#include <windows.h>

// File attachment with base64 encoding support
struct FileAttachment {
    std::wstring filename;
    std::wstring mimeType;
    std::string base64Data;  // base64 encoded file content
    size_t originalSize;

    FileAttachment() : originalSize(0) {}
};

// Message representation with role, content, and optional attachments
struct ChatMessage {
    enum class Role { System, User, Assistant };
    
    Role role;
    std::wstring content;
    std::vector<FileAttachment> attachments;
    SYSTEMTIME timestamp;

    ChatMessage() : role(Role::User) {
        GetSystemTime(&timestamp);
    }

    ChatMessage(Role r, const std::wstring& c) : role(r), content(c) {
        GetSystemTime(&timestamp);
    }

    // Serialization helpers
    std::wstring RoleToString() const;
    static Role StringToRole(const std::wstring& str);
    std::wstring ToJson() const;
    static ChatMessage FromJson(const std::wstring& json);
};
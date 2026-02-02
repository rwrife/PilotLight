#pragma once
#include <string>
#include <vector>
#include <windows.h>

namespace FileUtils {
    // File selection dialog
    std::vector<std::wstring> SelectFiles(HWND parent, const wchar_t* filter, bool multiSelect = true);
    
    // Base64 encoding/decoding
    std::string EncodeBase64(const std::vector<BYTE>& data);
    std::vector<BYTE> DecodeBase64(const std::string& base64);
    
    // MIME type detection
    std::wstring GetMimeType(const std::wstring& filename);
    
    // File I/O
    bool ReadFileToBuffer(const std::wstring& path, std::vector<BYTE>& buffer);
    bool WriteBufferToFile(const std::wstring& path, const std::vector<BYTE>& buffer);
    
    // File validation
    bool ValidateFileSize(const std::wstring& path, size_t maxBytes);
    bool ValidateFileType(const std::wstring& filename);
    
    // Path helpers
    std::wstring GetAppDataPath();
    bool EnsureDirectoryExists(const std::wstring& path);
}
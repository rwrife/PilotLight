#include "FileUtils.h"
#include <windows.h>
#include <commdlg.h>
#include <wincrypt.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <fstream>
#include <algorithm>

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "comdlg32.lib")

namespace FileUtils {

std::vector<std::wstring> SelectFiles(HWND parent, const wchar_t* filter, bool multiSelect)
{
    std::vector<std::wstring> result;
    
    wchar_t fileBuffer[4096] = {0};
    
    OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = parent;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileBuffer;
    ofn.nMaxFile = 4096;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    
    if (multiSelect) {
        ofn.Flags |= OFN_ALLOWMULTISELECT | OFN_EXPLORER;
    }
    
    if (GetOpenFileName(&ofn)) {
        if (multiSelect) {
            // Parse multi-select format: directory\0file1\0file2\0\0
            std::wstring directory = fileBuffer;
            wchar_t* p = fileBuffer + directory.length() + 1;
            
            if (*p == 0) {
                // Single file selected
                result.push_back(directory);
            } else {
                // Multiple files
                while (*p) {
                    std::wstring filename = p;
                    std::wstring fullPath = directory + L"\\" + filename;
                    result.push_back(fullPath);
                    p += filename.length() + 1;
                }
            }
        } else {
            result.push_back(fileBuffer);
        }
    }
    
    return result;
}

std::string EncodeBase64(const std::vector<BYTE>& data)
{
    if (data.empty()) return "";
    
    DWORD base64Length = 0;
    if (!CryptBinaryToStringA(data.data(), (DWORD)data.size(), 
                               CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, 
                               nullptr, &base64Length)) {
        return "";
    }
    
    std::string result(base64Length, '\0');
    if (!CryptBinaryToStringA(data.data(), (DWORD)data.size(), 
                               CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, 
                               &result[0], &base64Length)) {
        return "";
    }
    
    result.resize(base64Length - 1);  // Remove null terminator
    return result;
}

std::vector<BYTE> DecodeBase64(const std::string& base64)
{
    if (base64.empty()) return {};
    
    DWORD dataLength = 0;
    if (!CryptStringToBinaryA(base64.c_str(), (DWORD)base64.length(), 
                               CRYPT_STRING_BASE64, 
                               nullptr, &dataLength, nullptr, nullptr)) {
        return {};
    }
    
    std::vector<BYTE> result(dataLength);
    if (!CryptStringToBinaryA(base64.c_str(), (DWORD)base64.length(), 
                               CRYPT_STRING_BASE64, 
                               result.data(), &dataLength, nullptr, nullptr)) {
        return {};
    }
    
    return result;
}

std::wstring GetMimeType(const std::wstring& filename)
{
    std::wstring ext = PathFindExtension(filename.c_str());
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
    
    if (ext == L".png") return L"image/png";
    if (ext == L".jpg" || ext == L".jpeg") return L"image/jpeg";
    if (ext == L".gif") return L"image/gif";
    if (ext == L".bmp") return L"image/bmp";
    if (ext == L".pdf") return L"application/pdf";
    if (ext == L".txt") return L"text/plain";
    if (ext == L".doc") return L"application/msword";
    if (ext == L".docx") return L"application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    
    return L"application/octet-stream";
}

bool ReadFileToBuffer(const std::wstring& path, std::vector<BYTE>& buffer)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return false;
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    buffer.resize((size_t)size);
    if (!file.read((char*)buffer.data(), size)) {
        buffer.clear();
        return false;
    }
    
    return true;
}

bool WriteBufferToFile(const std::wstring& path, const std::vector<BYTE>& buffer)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) return false;
    
    file.write((const char*)buffer.data(), buffer.size());
    return file.good();
}

bool ValidateFileSize(const std::wstring& path, size_t maxBytes)
{
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fileInfo)) {
        return false;
    }
    
    ULARGE_INTEGER fileSize;
    fileSize.LowPart = fileInfo.nFileSizeLow;
    fileSize.HighPart = fileInfo.nFileSizeHigh;
    
    return fileSize.QuadPart <= maxBytes;
}

bool ValidateFileType(const std::wstring& filename)
{
    std::wstring ext = PathFindExtension(filename.c_str());
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
    
    const wchar_t* allowedExts[] = {
        L".png", L".jpg", L".jpeg", L".gif", L".bmp",
        L".pdf", L".txt", L".doc", L".docx"
    };
    
    for (const wchar_t* allowed : allowedExts) {
        if (ext == allowed) return true;
    }
    
    return false;
}

std::wstring GetAppDataPath()
{
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        std::wstring result = path;
        result += L"\\PilotLight";
        return result;
    }
    return L"";
}

bool EnsureDirectoryExists(const std::wstring& path)
{
    if (PathFileExists(path.c_str())) {
        return true;
    }
    
    return CreateDirectory(path.c_str(), nullptr) != 0;
}

}  // namespace FileUtils
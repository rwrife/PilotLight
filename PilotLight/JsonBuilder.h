#pragma once
#include <string>
#include <vector>

// Lightweight JSON construction for API requests
class JsonBuilder {
public:
    JsonBuilder();
    
    void BeginObject();
    void EndObject();
    void BeginArray(const wchar_t* key = nullptr);
    void EndArray();
    
    void AddString(const wchar_t* key, const std::wstring& value);
    void AddNumber(const wchar_t* key, double value);
    void AddBool(const wchar_t* key, bool value);
    void AddNull(const wchar_t* key);
    
    void AddRawValue(const std::wstring& value);  // For nested objects
    
    std::wstring ToString() const;
    void Clear();

private:
    std::wstring m_buffer;
    std::vector<bool> m_stateStack;  // true = object, false = array
    std::vector<bool> m_needsComma;
    
    void EnsureComma();
    std::wstring EscapeString(const std::wstring& str) const;
};
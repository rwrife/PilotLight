#include "JsonBuilder.h"
#include <sstream>
#include <iomanip>

JsonBuilder::JsonBuilder()
{
}

void JsonBuilder::Clear()
{
    m_buffer.clear();
    m_stateStack.clear();
    m_needsComma.clear();
}

void JsonBuilder::BeginObject()
{
    EnsureComma();
    m_buffer += L"{";
    m_stateStack.push_back(true);
    m_needsComma.push_back(false);
}

void JsonBuilder::EndObject()
{
    m_buffer += L"}";
    if (!m_stateStack.empty()) {
        m_stateStack.pop_back();
        m_needsComma.pop_back();
    }
    if (!m_needsComma.empty()) {
        m_needsComma.back() = true;
    }
}

void JsonBuilder::BeginArray(const wchar_t* key)
{
    EnsureComma();
    if (key) {
        m_buffer += L"\"";
        m_buffer += key;
        m_buffer += L"\":";
    }
    m_buffer += L"[";
    m_stateStack.push_back(false);
    m_needsComma.push_back(false);
}

void JsonBuilder::EndArray()
{
    m_buffer += L"]";
    if (!m_stateStack.empty()) {
        m_stateStack.pop_back();
        m_needsComma.pop_back();
    }
    if (!m_needsComma.empty()) {
        m_needsComma.back() = true;
    }
}

void JsonBuilder::EnsureComma()
{
    if (!m_needsComma.empty() && m_needsComma.back()) {
        m_buffer += L",";
    }
}

std::wstring JsonBuilder::EscapeString(const std::wstring& str) const
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
            case L'\b': result += L"\\b"; break;
            case L'\f': result += L"\\f"; break;
            default:
                if (c < 32) {
                    wchar_t buf[8];
                    swprintf_s(buf, L"\\u%04x", (int)c);
                    result += buf;
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

void JsonBuilder::AddString(const wchar_t* key, const std::wstring& value)
{
    EnsureComma();
    
    if (!m_stateStack.empty() && m_stateStack.back()) {
        // In object, add key
        m_buffer += L"\"";
        m_buffer += key;
        m_buffer += L"\":";
    }
    
    m_buffer += L"\"";
    m_buffer += EscapeString(value);
    m_buffer += L"\"";
    
    if (!m_needsComma.empty()) {
        m_needsComma.back() = true;
    }
}

void JsonBuilder::AddNumber(const wchar_t* key, double value)
{
    EnsureComma();
    
    if (!m_stateStack.empty() && m_stateStack.back()) {
        m_buffer += L"\"";
        m_buffer += key;
        m_buffer += L"\":";
    }
    
    wchar_t buf[64];
    swprintf_s(buf, L"%g", value);
    m_buffer += buf;
    
    if (!m_needsComma.empty()) {
        m_needsComma.back() = true;
    }
}

void JsonBuilder::AddBool(const wchar_t* key, bool value)
{
    EnsureComma();
    
    if (!m_stateStack.empty() && m_stateStack.back()) {
        m_buffer += L"\"";
        m_buffer += key;
        m_buffer += L"\":";
    }
    
    m_buffer += value ? L"true" : L"false";
    
    if (!m_needsComma.empty()) {
        m_needsComma.back() = true;
    }
}

void JsonBuilder::AddNull(const wchar_t* key)
{
    EnsureComma();
    
    if (!m_stateStack.empty() && m_stateStack.back()) {
        m_buffer += L"\"";
        m_buffer += key;
        m_buffer += L"\":";
    }
    
    m_buffer += L"null";
    
    if (!m_needsComma.empty()) {
        m_needsComma.back() = true;
    }
}

void JsonBuilder::AddRawValue(const std::wstring& value)
{
    EnsureComma();
    m_buffer += value;
    
    if (!m_needsComma.empty()) {
        m_needsComma.back() = true;
    }
}

std::wstring JsonBuilder::ToString() const
{
    return m_buffer;
}
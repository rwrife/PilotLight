#include "RichTextRenderer.h"
#include <sstream>

std::wstring RichTextRenderer::EscapeRTF(const std::wstring& str)
{
    std::wstring result;
    result.reserve(str.length() + 10);
    
    for (wchar_t c : str) {
        switch (c) {
            case L'\\': result += L"\\\\"; break;
            case L'{': result += L"\\{"; break;
            case L'}': result += L"\\}"; break;
            default: result += c; break;
        }
    }
    
    return result;
}

std::wstring RichTextRenderer::ProcessMarkdown(const std::wstring& markdown)
{
    std::wstring result = markdown;
    std::wstring processed;
    size_t pos = 0;
    
    while (pos < result.length()) {
        // Check for code blocks (```...```)
        if (result.substr(pos, 3) == L"```") {
            size_t end = result.find(L"```", pos + 3);
            if (end != std::wstring::npos) {
                processed += L"\\f1 ";  // Monospace font
                processed += EscapeRTF(result.substr(pos + 3, end - pos - 3));
                processed += L"\\f0 ";  // Back to normal font
                pos = end + 3;
                continue;
            }
        }
        
        // Check for inline code (`...`)
        if (result[pos] == L'`') {
            size_t end = result.find(L'`', pos + 1);
            if (end != std::wstring::npos) {
                processed += L"\\f1 ";
                processed += EscapeRTF(result.substr(pos + 1, end - pos - 1));
                processed += L"\\f0 ";
                pos = end + 1;
                continue;
            }
        }
        
        // Check for bold (**...**)
        if (result.substr(pos, 2) == L"**") {
            size_t end = result.find(L"**", pos + 2);
            if (end != std::wstring::npos) {
                processed += L"\\b ";
                processed += EscapeRTF(result.substr(pos + 2, end - pos - 2));
                processed += L"\\b0 ";
                pos = end + 2;
                continue;
            }
        }
        
        // Check for italic (*...*)
        if (result[pos] == L'*') {
            size_t end = result.find(L'*', pos + 1);
            if (end != std::wstring::npos) {
                processed += L"\\i ";
                processed += EscapeRTF(result.substr(pos + 1, end - pos - 1));
                processed += L"\\i0 ";
                pos = end + 1;
                continue;
            }
        }
        
        // Regular character
        if (result[pos] == L'\\' || result[pos] == L'{' || result[pos] == L'}') {
            processed += L'\\';
        }
        processed += result[pos];
        pos++;
    }
    
    return processed;
}

std::wstring RichTextRenderer::RenderToRTF(const std::wstring& markdown)
{
    std::wostringstream rtf;
    
    rtf << L"{\\rtf1\\ansi\\deff0";
    rtf << L"{\\fonttbl{\\f0\\fswiss Segoe UI;}{\\f1\\fmodern Consolas;}}";
    rtf << L"{\\colortbl;\\red220\\green220\\blue220;}";  // Text color
    rtf << L"\\cf1 ";
    
    rtf << ProcessMarkdown(markdown);
    
    rtf << L"}";
    
    return rtf.str();
}

void RichTextRenderer::AppendFormattedText(CRichEditCtrl& ctrl, const std::wstring& text, COLORREF color)
{
    // Get current text length
    int startPos = ctrl.GetTextLength();
    
    // Append new text
    ctrl.SetSel(startPos, startPos);
    ctrl.ReplaceSel(text.c_str());
    
    // Format the newly added text
    int endPos = ctrl.GetTextLength();
    ctrl.SetSel(startPos, endPos);
    
    CHARFORMAT2 cf;
    ZeroMemory(&cf, sizeof(CHARFORMAT2));
    cf.cbSize = sizeof(CHARFORMAT2);
    cf.dwMask = CFM_COLOR;
    cf.crTextColor = color;
    ctrl.SetSelectionCharFormat(cf);
    
    // Deselect
    ctrl.SetSel(endPos, endPos);
}

void RichTextRenderer::ScrollToBottom(CRichEditCtrl& ctrl)
{
    ctrl.LineScroll(ctrl.GetLineCount());
}
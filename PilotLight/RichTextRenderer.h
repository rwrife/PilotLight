#pragma once
#include <string>

#define _WINSOCKAPI_
#include <winsock2.h>
#include <afxrich.h>

// Basic formatting renderer (bold, italic, code blocks)
class RichTextRenderer {
public:
    // Convert basic markdown to RTF
    static std::wstring RenderToRTF(const std::wstring& markdown);
    
    // Append formatted text to RichEdit control
    static void AppendFormattedText(CRichEditCtrl& ctrl, const std::wstring& text, COLORREF color);
    
    // Append a bubble-style text block
    static void AppendBubble(CRichEditCtrl& ctrl, const std::wstring& text, COLORREF textColor,
                             COLORREF backgroundColor, int indent = 18);
    
    // Scroll to bottom
    static void ScrollToBottom(CRichEditCtrl& ctrl);
    
private:
    static std::wstring EscapeRTF(const std::wstring& str);
    static std::wstring ProcessMarkdown(const std::wstring& markdown);
};
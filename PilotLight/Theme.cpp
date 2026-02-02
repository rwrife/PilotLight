#include "Theme.h"
#include <afxwin.h>
#include <string>

HFONT Theme::UIFont()
{
    static HFONT hFont = CreateFontW(
        -18, 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        VARIABLE_PITCH, L"Segoe UI Variable"
    );
    return hFont;
}

COLORREF Theme::GetButtonColor(ButtonState state, bool isCloseButton)
{
    switch (state) {
    case ButtonState::Normal:
        return FrameBackground;  // Transparent/same as background
    case ButtonState::Hover:
        return isCloseButton ? CloseButtonHover : ButtonHover;
    case ButtonState::Pressed:
        return ButtonPressed;
    default:
        return FrameBackground;
    }
}

void Theme::DrawFramelessButton(CDC* pDC, const CRect& rect, const wchar_t* text, 
                                ButtonState state, bool isCloseButton)
{
    // Get background color based on state
    COLORREF bgColor = GetButtonColor(state, isCloseButton);
    
    // Always fill background with appropriate color
    CBrush brush(bgColor);
    pDC->FillRect(&rect, &brush);
    
    // Use Segoe MDL2 Assets for window chrome icons (available on Windows 10+)
    static HFONT hIconFont = CreateFontW(
        -12, 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH, L"Segoe MDL2 Assets"
    );
    
    // Map common symbols to MDL2 glyph codes based on first character
    std::wstring glyph;
    wchar_t firstChar = text[0];
    
    // Window chrome buttons (MDL2 codes)
    // Use hex code points to avoid encoding issues
    if (firstChar == 0x2212 || firstChar == L'-') {
        // U+2212 Minus sign or regular hyphen
        glyph = L"\uE921";  // ChromeMinimize
    }
    else if (firstChar == 0x25A1) {
        // U+25A1 White square (maximize)
        glyph = L"\uE922";  // ChromeMaximize
    }
    else if (firstChar == 0x2750) {
        // U+2750 Upper right drop-shadowed white square (restore)
        glyph = L"\uE923";  // ChromeRestore
    }
    else if (firstChar == 0x00D7 || firstChar == L'X' || firstChar == L'x') {
        // U+00D7 Multiplication sign or X
        glyph = L"\uE8BB";  // ChromeClose
    }
    else if (firstChar == 0x2191) {
        // U+2191 Upwards arrow
        glyph = L"\uE724";  // Send
    }
    else if (firstChar == 0xD83D) {
        // First part of paperclip emoji surrogate pair (?? is U+1F4CE)
        glyph = L"\uE723";  // Attach
    }
    else {
        glyph = text;  // Use original text as fallback
    }
    
    // Select icon font
    HFONT hOldFont = (HFONT)pDC->SelectObject(hIconFont);
    
    // Set text color and background mode
    COLORREF oldTextColor = pDC->SetTextColor(Foreground);
    int oldBkMode = pDC->SetBkMode(TRANSPARENT);
    
    // Draw icon centered in button
    CRect textRect = rect;
    pDC->DrawTextW(glyph.c_str(), static_cast<int>(glyph.length()), &textRect, 
                   DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    // Restore DC state
    pDC->SetBkMode(oldBkMode);
    pDC->SetTextColor(oldTextColor);
    pDC->SelectObject(hOldFont);
}

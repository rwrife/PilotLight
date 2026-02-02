#include "Theme.h"
#include <afxwin.h>

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

void Theme::DrawFramelessButton(CDC* pDC, const CRect& rect, const wchar_t* emoji, 
                                ButtonState state, bool isCloseButton)
{
    // Get background color based on state
    COLORREF bgColor = GetButtonColor(state, isCloseButton);
    
    // Always fill background with appropriate color
    CBrush brush(bgColor);
    pDC->FillRect(&rect, &brush);
    
    // Create symbol font for button icons
    static HFONT hEmojiFont = CreateFontW(
        -18, 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH, L"Segoe UI Symbol"
    );
    
    // Select emoji font
    HFONT hOldFont = (HFONT)pDC->SelectObject(hEmojiFont);
    
    // Set text color and background mode
    COLORREF oldTextColor = pDC->SetTextColor(Foreground);
    int oldBkMode = pDC->SetBkMode(TRANSPARENT);
    
    // Draw emoji centered in button
    CRect textRect = rect;
    int len = static_cast<int>(wcslen(emoji));
    pDC->DrawTextW(emoji, len, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    // Restore DC state
    pDC->SetBkMode(oldBkMode);
    pDC->SetTextColor(oldTextColor);
    pDC->SelectObject(hOldFont);
}

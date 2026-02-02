#pragma once

#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>

// Forward declarations
class CDC;
class CRect;

namespace Theme {
    // Dark theme colors - new modern palette
    constexpr COLORREF FrameBackground = RGB(32, 32, 32);      // #202020
    constexpr COLORREF TitleBackground = RGB(32, 32, 32);      // #202020
    constexpr COLORREF ChatBackground = RGB(44, 44, 44);       // #2C2C2C
    constexpr COLORREF Foreground = RGB(204, 204, 204);        // #CCCCCC
    constexpr COLORREF ActiveBorder = RGB(61, 116, 208);       // #3D74D0
    constexpr COLORREF ButtonHover = RGB(45, 45, 45);          // Slightly lighter for hover
    constexpr COLORREF ButtonPressed = RGB(25, 25, 25);        // Slightly darker for pressed
    constexpr COLORREF CloseButtonHover = RGB(232, 17, 35);    // Red hover for close button
    
    // Legacy colors for compatibility
    constexpr COLORREF Background = RGB(30,30,30);
    constexpr COLORREF Surface    = RGB(37,37,38);
    constexpr COLORREF Accent     = RGB(0,122,204);
    constexpr COLORREF Text       = RGB(220,220,220);

    // Button state tracking
    enum class ButtonState {
        Normal,
        Hover,
        Pressed
    };

    // Font
    HFONT UIFont();
    
    // Button drawing helper functions
    void DrawFramelessButton(CDC* pDC, const CRect& rect, const wchar_t* emoji, 
                            ButtonState state, bool isCloseButton = false);
    COLORREF GetButtonColor(ButtonState state, bool isCloseButton);
}

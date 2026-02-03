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
    constexpr COLORREF ButtonHover = RGB(45, 45, 45);          // #2D2D2D - hover for minimize/maximize
    constexpr COLORREF ButtonPressed = RGB(25, 25, 25);        // Slightly darker for pressed
    constexpr COLORREF CloseButtonHover = RGB(211, 58, 46);    // #D33A2E - Red hover for close button
    
    // Scrollbar colors
    constexpr COLORREF ScrollbarTrack = RGB(50, 50, 50);       // #323232 - scrollbar track/background
    constexpr COLORREF ScrollbarThumb = RGB(100, 100, 100);    // #646464 - scrollbar thumb (lighter gray)
    constexpr COLORREF ScrollbarThumbHover = RGB(140, 140, 140); // #8C8C8C - scrollbar thumb hover
    
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

    // Fonts
    HFONT UIFont();
    HFONT TitleFont();  // Larger font for titlebar
    
    // Button drawing helper functions
    void DrawFramelessButton(CDC* pDC, const CRect& rect, const wchar_t* emoji, 
                            ButtonState state, bool isCloseButton = false);
    COLORREF GetButtonColor(ButtonState state, bool isCloseButton);
}

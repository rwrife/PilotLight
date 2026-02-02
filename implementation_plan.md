# Implementation Plan: Dark Modern UI Theme

## [Overview]
Transform PilotLight's UI into a modern dark-themed application with updated color scheme (#202020 background, #CCCCCC foreground, #3D74D0 accent), frameless owner-drawn buttons with Unicode emoji icons, and custom hover effects.

This implementation modernizes the visual appearance of the Windows MFC application by replacing the existing theme colors, implementing custom button rendering with emoji icons, and adding interactive visual feedback through hover states and focus borders. The changes will be isolated to the Theme, MainDlg, and resource files to minimize risk while achieving a polished modern appearance.

The existing Theme.h/cpp provides basic color constants and a UI font. We'll extend this to include the new color palette, add button state colors, and create utility functions for custom drawing. The MainDlg class currently creates standard Windows buttons - we'll convert these to owner-drawn buttons that render Unicode emojis with custom backgrounds, borders, and hover effects.

## [Types]
Define new color constants and button state enumerations for the modern dark theme.

New color constants to be added to `Theme.h`:
```cpp
namespace Theme {
    // Dark theme colors
    constexpr COLORREF FrameBackground = RGB(32, 32, 32);      // #202020
    constexpr COLORREF TitleBackground = RGB(32, 32, 32);      // #202020
    constexpr COLORREF Foreground = RGB(204, 204, 204);        // #CCCCCC
    constexpr COLORREF ActiveBorder = RGB(61, 116, 208);       // #3D74D0
    constexpr COLORREF ButtonHover = RGB(45, 45, 45);          // Slightly lighter for hover
    constexpr COLORREF ButtonPressed = RGB(25, 25, 25);        // Slightly darker for pressed
    constexpr COLORREF CloseButtonHover = RGB(232, 17, 35);    // Red hover for close button
    
    // Button state tracking
    enum class ButtonState {
        Normal,
        Hover,
        Pressed
    };
    
    // Button drawing helper functions
    void DrawFramelessButton(CDC* pDC, const CRect& rect, const wchar_t* emoji, 
                            ButtonState state, bool isCloseButton = false);
    COLORREF GetButtonColor(ButtonState state, bool isCloseButton);
}
```

## [Files]
Modify existing files to implement the dark theme and custom button rendering.

**Modified Files:**

1. **PilotLight/Theme.h**
   - Add new color constants for dark theme (#202020, #CCCCCC, #3D74D0)
   - Add button state colors (hover, pressed, close button hover)
   - Add ButtonState enum
   - Add DrawFramelessButton helper function declaration
   - Add GetButtonColor helper function declaration

2. **PilotLight/Theme.cpp**
   - Implement DrawFramelessButton function to render emoji buttons with custom backgrounds
   - Implement GetButtonColor function to return appropriate colors based on state
   - Handle Unicode emoji rendering with proper font selection

3. **PilotLight/MainDlg.h**
   - Add BS_OWNERDRAW style to button declarations
   - Add button state tracking members (m_btnMinimizeState, m_btnMaximizeState, m_btnCloseState, m_btnSendState, m_btnAttachState)
   - Add WM_DRAWITEM message handler declaration
   - Add WM_MOUSEMOVE message handler declaration
   - Add WM_MOUSELEAVE message handler declaration
   - Add TRACKMOUSEEVENT support for hover detection
   - Add member variables to track which button is currently hovered

4. **PilotLight/MainDlg.cpp**
   - Update OnInitDialog to apply BS_OWNERDRAW style to all buttons
   - Set button text to Unicode emojis: ➖ (minimize), ⬜ (maximize), ✖ (close), 📤 (send), 📎 (attach)
   - Implement OnDrawItem handler to render custom buttons using Theme::DrawFramelessButton
   - Implement OnMouseMove handler to track button hover states
   - Implement OnMouseLeave handler to reset hover states
   - Update color scheme for all controls (background #202020, foreground #CCCCCC)
   - Update OnPaint to draw background with #202020
   - Add focus border rendering with #3D74D0 for active controls

5. **PilotLight/PilotLight.rc**
   - Update PUSHBUTTON controls to use BS_OWNERDRAW style
   - Remove static text from button definitions (will be set programmatically)
   - Ensure buttons are properly sized for emoji rendering

6. **PilotLight/BorderlessFrame.cpp**
   - Update to return HTCLIENT for owner-drawn buttons to ensure proper click handling

## [Functions]
Implement new drawing functions and update existing message handlers for custom button rendering.

**New Functions:**

1. **Theme::DrawFramelessButton** (Theme.cpp)
   - Parameters: CDC* pDC, const CRect& rect, const wchar_t* emoji, ButtonState state, bool isCloseButton
   - Purpose: Render a frameless button with Unicode emoji icon, custom background, and state-based styling
   - Implementation:
     - Select appropriate background color based on state and button type
     - Fill button rectangle with background (transparent for Normal state, colored for Hover/Pressed)
     - Select "Segoe UI Emoji" font at size 16 for emoji rendering
     - Center emoji text in button rectangle
     - Draw emoji with #CCCCCC color
     - Apply subtle shadow/glow effect for pressed state

2. **Theme::GetButtonColor** (Theme.cpp)
   - Parameters: ButtonState state, bool isCloseButton
   - Returns: COLORREF
   - Purpose: Return appropriate background color for button state
   - Implementation:
     - Normal: Transparent (return FrameBackground)
     - Hover: isCloseButton ? CloseButtonHover : ButtonHover
     - Pressed: ButtonPressed

**Modified Functions:**

1. **CMainDlg::OnInitDialog** (MainDlg.cpp)
   - Add: Apply BS_OWNERDRAW style to all buttons using ModifyStyle
   - Add: Set button text to Unicode emojis
   - Add: Initialize button state tracking variables
   - Update: Set control colors to use new dark theme palette
   - Update: Configure RichEdit background to #202020
   - Update: Configure Edit control background to #202020

2. **CMainDlg::OnDrawItem** (MainDlg.cpp) - NEW HANDLER
   - Parameters: int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct
   - Purpose: Custom drawing for owner-drawn buttons
   - Implementation:
     - Determine which button is being drawn (minimize/maximize/close/send/attach)
     - Get current button state from tracking variables
     - Select appropriate emoji based on button type
     - Call Theme::DrawFramelessButton with appropriate parameters
     - Handle maximize button icon change when window is maximized (⬜ vs ◻)

3. **CMainDlg::OnMouseMove** (MainDlg.cpp) - NEW HANDLER
   - Parameters: UINT nFlags, CPoint point
   - Purpose: Track mouse position for button hover effects
   - Implementation:
     - Convert point to client coordinates
     - Check if mouse is over each button using PtInRect
     - Update button state variables (Normal -> Hover)
     - Invalidate button rectangles that changed state
     - Set up TRACKMOUSEEVENT for WM_MOUSELEAVE notification

4. **CMainDlg::OnMouseLeave** (MainDlg.cpp) - NEW HANDLER
   - Parameters: None
   - Purpose: Reset all button hover states when mouse leaves client area
   - Implementation:
     - Set all button states to Normal
     - Invalidate all button rectangles to trigger redraw

5. **CMainDlg::OnPaint** (MainDlg.cpp)
   - Update: Fill entire client area with Theme::FrameBackground (#202020)
   - Update: Draw title bar area with Theme::TitleBackground (#202020)
   - Add: Draw active control borders with Theme::ActiveBorder (#3D74D0) when focused

6. **CMainDlg::LayoutControls** (MainDlg.cpp)
   - Update: Ensure button rectangles are properly sized for emoji display (minimum 38x32px)
   - Update: Adjust spacing to accommodate frameless button design

## [Classes]
Extend existing classes with button state tracking and custom drawing capabilities.

**Modified Classes:**

1. **CMainDlg** (MainDlg.h/cpp)
   - Add private members:
     - `Theme::ButtonState m_btnMinimizeState`
     - `Theme::ButtonState m_btnMaximizeState`
     - `Theme::ButtonState m_btnCloseState`
     - `Theme::ButtonState m_btnSendState`
     - `Theme::ButtonState m_btnAttachState`
     - `BOOL m_bTrackingMouse` - flag for TRACKMOUSEEVENT
   - Add message handlers:
     - `afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)`
     - `afx_msg void OnMouseMove(UINT nFlags, CPoint point)`
     - `afx_msg void OnMouseLeave()`
   - Add helper methods:
     - `CRect GetButtonRect(int buttonID)` - Get screen rectangle for a button control
     - `void UpdateButtonState(int buttonID, Theme::ButtonState newState)` - Update state and invalidate
   - Initialize all button states to Normal in constructor

2. **Theme** (Theme.h/cpp)
   - Add static drawing helper functions
   - Add color constant definitions for dark theme
   - Add ButtonState enum definition

## [Dependencies]
No new external dependencies required. All functionality uses existing Windows GDI and MFC APIs.

**Existing Dependencies:**
- Windows SDK (GDI for custom drawing)
- MFC (CButton, CDC, CRect, DRAWITEMSTRUCT)
- Segoe UI Emoji font (built into Windows 10+)

**Font Requirements:**
- "Segoe UI Emoji" must be available (standard on Windows 10/11)
- Fallback to "Segoe UI Symbol" if Emoji font unavailable
- Unicode emoji characters: ➖ (U+2796), ⬜ (U+2B1C), ✖ (U+2716), 📤 (U+1F4E4), 📎 (U+1F4CE)

## [Testing]
Comprehensive testing approach to validate visual changes and interaction behavior.

**Visual Validation:**
1. Verify all UI elements use correct colors:
   - Frame background: #202020
   - Titlebar background: #202020
   - Text/foreground: #CCCCCC
   - Active borders: #3D74D0
   - Button hover states: visible and correct color
   - Close button hover: red (#E81123)

2. Verify button rendering:
   - All buttons display Unicode emojis correctly
   - Emojis are centered in button rectangles
   - Buttons have no visible frame/border in normal state
   - Hover state shows subtle background color
   - Pressed state shows darker background color
   - Close button hover shows red background

3. Verify interaction states:
   - Mouse hover triggers button state change
   - Mouse leave resets button state
   - Button press shows pressed state
   - Focus indicators use correct color (#3D74D0)

**Functional Testing:**
1. Test all button functions still work:
   - Minimize button minimizes window
   - Maximize button maximizes/restores window
   - Close button closes application
   - Send button sends message
   - Attach button opens file dialog

2. Test window operations:
   - Window can be dragged by titlebar
   - Window can be resized from edges
   - Window maintains rounded corners
   - Minimum size constraints still apply

3. Test control rendering:
   - RichEdit control shows dark background
   - Input control shows dark background
   - Text is visible (#CCCCCC on #202020)
   - Scrollbars are functional

**Edge Cases:**
1. Test with DPI scaling (125%, 150%, 200%)
2. Test with high contrast themes
3. Test maximize/restore icon toggle
4. Test rapid mouse movements over buttons
5. Test keyboard navigation with focus indicators

## [Implementation Order]
Sequential implementation steps to ensure smooth integration and minimize build errors.

1. **Update Theme color definitions** (Theme.h)
   - Add new color constants
   - Add ButtonState enum
   - Declare helper functions
   - Build and verify no compilation errors

2. **Implement Theme drawing functions** (Theme.cpp)
   - Implement GetButtonColor function
   - Implement DrawFramelessButton function
   - Test emoji rendering in isolation
   - Build and verify successful compilation

3. **Update MainDlg header** (MainDlg.h)
   - Add button state member variables
   - Add message handler declarations (OnDrawItem, OnMouseMove, OnMouseLeave)
   - Add helper method declarations
   - Build and verify no compilation errors

4. **Update resource file for owner-drawn buttons** (PilotLight.rc)
   - Add BS_OWNERDRAW style to all button definitions
   - Remove static text from button definitions
   - Verify resource compilation

5. **Implement button state tracking in MainDlg** (MainDlg.cpp)
   - Initialize button states in constructor
   - Update OnInitDialog to set emoji text and apply owner-draw style
   - Add GetButtonRect helper implementation
   - Add UpdateButtonState helper implementation
   - Build and test basic initialization

6. **Implement OnDrawItem handler** (MainDlg.cpp)
   - Add message map entry for WM_DRAWITEM
   - Implement OnDrawItem to call Theme::DrawFramelessButton
   - Handle maximize icon toggle based on window state
   - Build and test button rendering

7. **Implement mouse tracking** (MainDlg.cpp)
   - Add message map entries for WM_MOUSEMOVE and WM_MOUSELEAVE
   - Implement OnMouseMove with hover detection
   - Implement OnMouseLeave to reset states
   - Set up TRACKMOUSEEVENT
   - Build and test hover effects

8. **Update OnPaint for background rendering** (MainDlg.cpp)
   - Fill background with Theme::FrameBackground
   - Draw focus borders with Theme::ActiveBorder
   - Build and test paint operations

9. **Update control colors in OnInitDialog** (MainDlg.cpp)
   - Set RichEdit background to #202020
   - Set Edit control background to #202020
   - Set text colors to #CCCCCC
   - Build and test control appearance

10. **Final integration testing**
    - Run full application build
    - Test all interactions
    - Verify visual appearance matches requirements
    - Test edge cases and DPI scaling
    - Fix any rendering artifacts or interaction issues

11. **Code cleanup and optimization**
    - Remove unused code
    - Add comments for custom drawing logic
    - Optimize redraw regions to minimize flicker
    - Final build and validation
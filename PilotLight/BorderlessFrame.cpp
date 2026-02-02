#include "BorderlessFrame.h"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

void CBorderlessFrame::Apply(CWnd* wnd)
{
    // Remove standard window styles
    wnd->ModifyStyle(WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_THICKFRAME, WS_POPUP | WS_THICKFRAME);
    wnd->ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE, WS_EX_APPWINDOW);

    // Extend frame into client area to remove the visible border
    MARGINS margins = { 0, 0, 0, 1 };  // Minimal extension to enable DWM
    DwmExtendFrameIntoClientArea(wnd->GetSafeHwnd(), &margins);

    // Disable non-client rendering to remove the gray border
    DWMNCRENDERINGPOLICY policy = DWMNCRP_DISABLED;
    DwmSetWindowAttribute(wnd->GetSafeHwnd(), DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));

    // Optional: Set window corner preference (Windows 11)
    DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
    DwmSetWindowAttribute(wnd->GetSafeHwnd(), DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    // Force a frame change to apply the new styles
    wnd->SetWindowPos(nullptr, 0, 0, 0, 0,
        SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void CBorderlessFrame::UpdateRegion(CWnd* wnd, int radius)
{
    CRect rc;
    wnd->GetClientRect(&rc);

    HRGN rgn = CreateRoundRectRgn(
        0, 0, rc.Width() + 1, rc.Height() + 1,
        radius, radius
    );
    SetWindowRgn(wnd->GetSafeHwnd(), rgn, TRUE);
}

CRect CBorderlessFrame::GetTitlebarButtonRect(CWnd* wnd, int buttonIndex)
{
    CRect rc;
    wnd->GetClientRect(&rc);

    const int buttonWidth = 46;
    const int margin = 7;
    const int titlebarHeight = 40;

    // buttonIndex: 0 = minimize, 1 = maximize, 2 = close
    int right = rc.right - margin - (2 - buttonIndex) * buttonWidth;
    int left = right - buttonWidth;

    return CRect(left, margin, right, margin + titlebarHeight);
}

LRESULT CBorderlessFrame::HitTest(CWnd* wnd, CPoint pt)
{
    wnd->ScreenToClient(&pt);

    const int grip = 8;
    const int titlebarHeight = 40;
    CRect rc;
    wnd->GetClientRect(&rc);

    // Check if in titlebar button areas (should not be draggable)
    if (pt.y < titlebarHeight + 7) {
        for (int i = 0; i < 3; ++i) {
            CRect btnRect = GetTitlebarButtonRect(wnd, i);
            if (btnRect.PtInRect(pt)) {
                return HTCLIENT;  // Let buttons receive clicks
            }
        }
    }

    // Window edges for resizing - corners first
    if (pt.x < grip && pt.y < grip) return HTTOPLEFT;
    if (pt.x > rc.right - grip && pt.y < grip) return HTTOPRIGHT;
    if (pt.x < grip && pt.y > rc.bottom - grip) return HTBOTTOMLEFT;
    if (pt.x > rc.right - grip && pt.y > rc.bottom - grip) return HTBOTTOMRIGHT;

    // Window edges for resizing - sides
    if (pt.x < grip) return HTLEFT;
    if (pt.x > rc.right - grip) return HTRIGHT;
    if (pt.y < grip) return HTTOP;
    if (pt.y > rc.bottom - grip) return HTBOTTOM;

    // Titlebar area (draggable)
    if (pt.y < titlebarHeight) return HTCAPTION;

    return HTCLIENT;
}

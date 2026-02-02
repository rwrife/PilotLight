#include "BorderlessFrame.h"

void CBorderlessFrame::Apply(CWnd* wnd)
{
    wnd->ModifyStyle(WS_OVERLAPPEDWINDOW, WS_POPUP);
    wnd->ModifyStyleEx(WS_EX_CLIENTEDGE, WS_EX_APPWINDOW);
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

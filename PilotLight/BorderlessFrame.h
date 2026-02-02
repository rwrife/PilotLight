#pragma once

#define _WINSOCKAPI_
#include <winsock2.h>
#include <afxwin.h>

class CBorderlessFrame {
public:
    static void Apply(CWnd* wnd);
    static void UpdateRegion(CWnd* wnd, int radius);
    static LRESULT HitTest(CWnd* wnd, CPoint pt);
    static CRect GetTitlebarButtonRect(CWnd* wnd, int buttonIndex);
};

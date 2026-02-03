#include "ThemedRichEdit.h"

// Dynamic creation
IMPLEMENT_DYNAMIC(CThemedRichEdit, CRichEditCtrl)

CThemedRichEdit::CThemedRichEdit()
    : m_bThumbHover(false)
    , m_bThumbPressed(false)
    , m_nDragStartY(0)
    , m_nDragStartScrollPos(0)
    , m_bInitialized(false)
{
}

CThemedRichEdit::~CThemedRichEdit()
{
}

// Message map
BEGIN_MESSAGE_MAP(CThemedRichEdit, CRichEditCtrl)
    ON_WM_PAINT()
    ON_WM_VSCROLL()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSELEAVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
    ON_WM_SHOWWINDOW()
    ON_MESSAGE(WM_SETTEXT, &CThemedRichEdit::OnSetText)
END_MESSAGE_MAP()

void CThemedRichEdit::PreSubclassWindow()
{
    CRichEditCtrl::PreSubclassWindow();
    
    // Remove scrollbar styles before control is shown
    ModifyStyle(WS_VSCROLL | WS_HSCROLL, 0);
}

void CThemedRichEdit::InitThemedScrollbar()
{
    // Remove the native vertical scrollbar style completely
    ModifyStyle(WS_VSCROLL | WS_HSCROLL, 0);
    
    // Also ensure no scrollbar is shown
    ShowScrollBar(SB_VERT, FALSE);
    ShowScrollBar(SB_HORZ, FALSE);
    
    // Force the window to update without scrollbars
    SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    
    m_bInitialized = true;
}

void CThemedRichEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CRichEditCtrl::OnShowWindow(bShow, nStatus);
    
    // Hide scrollbars whenever window is shown
    if (bShow && m_bInitialized) {
        ShowScrollBar(SB_VERT, FALSE);
        ShowScrollBar(SB_HORZ, FALSE);
    }
}

CRect CThemedRichEdit::GetScrollbarTrackRect()
{
    CRect clientRect;
    GetClientRect(&clientRect);
    
    CRect trackRect;
    trackRect.right = clientRect.right;
    trackRect.left = clientRect.right - SCROLLBAR_WIDTH;
    trackRect.top = clientRect.top + 4;  // Small margin from top
    trackRect.bottom = clientRect.bottom - 4;  // Small margin from bottom
    
    return trackRect;
}

int CThemedRichEdit::GetThumbHeight()
{
    // Get total line count and visible lines
    int totalLines = GetLineCount();
    if (totalLines <= 0) totalLines = 1;
    
    CRect clientRect;
    GetClientRect(&clientRect);
    
    // Estimate visible lines based on font height
    CDC* pDC = GetDC();
    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    ReleaseDC(pDC);
    
    int lineHeight = tm.tmHeight + tm.tmExternalLeading;
    if (lineHeight <= 0) lineHeight = 20;
    
    int visibleLines = clientRect.Height() / lineHeight;
    if (visibleLines <= 0) visibleLines = 1;
    
    if (totalLines <= visibleLines) {
        return 0;  // No scrolling needed
    }
    
    CRect trackRect = GetScrollbarTrackRect();
    int trackHeight = trackRect.Height();
    
    int thumbHeight = (int)((float)visibleLines / totalLines * trackHeight);
    
    // Minimum thumb size
    return max(30, thumbHeight);
}

int CThemedRichEdit::GetThumbPosition()
{
    int totalLines = GetLineCount();
    if (totalLines <= 0) return 0;
    
    int firstVisible = GetFirstVisibleLine();
    
    CRect clientRect;
    GetClientRect(&clientRect);
    
    CDC* pDC = GetDC();
    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    ReleaseDC(pDC);
    
    int lineHeight = tm.tmHeight + tm.tmExternalLeading;
    if (lineHeight <= 0) lineHeight = 20;
    
    int visibleLines = clientRect.Height() / lineHeight;
    if (visibleLines <= 0) visibleLines = 1;
    
    int scrollableLines = totalLines - visibleLines;
    if (scrollableLines <= 0) return 0;
    
    CRect trackRect = GetScrollbarTrackRect();
    int trackHeight = trackRect.Height();
    int thumbHeight = GetThumbHeight();
    int scrollableTrack = trackHeight - thumbHeight;
    
    if (scrollableTrack <= 0) return 0;
    
    // Clamp firstVisible to valid range
    firstVisible = min(firstVisible, scrollableLines);
    firstVisible = max(0, firstVisible);
    
    int thumbPos = (int)((float)firstVisible / scrollableLines * scrollableTrack);
    
    // Clamp thumb position to ensure it stays within track bounds
    thumbPos = max(0, min(scrollableTrack, thumbPos));
    
    return thumbPos;
}

CRect CThemedRichEdit::GetThumbRect()
{
    int thumbHeight = GetThumbHeight();
    if (thumbHeight == 0) {
        return CRect(0, 0, 0, 0);
    }
    
    CRect trackRect = GetScrollbarTrackRect();
    int thumbPos = GetThumbPosition();
    
    // Ensure thumb doesn't exceed track bounds
    int maxThumbTop = trackRect.Height() - thumbHeight;
    thumbPos = min(thumbPos, maxThumbTop);
    thumbPos = max(0, thumbPos);
    
    CRect thumbRect;
    thumbRect.left = trackRect.left + 2;
    thumbRect.right = trackRect.right - 2;
    thumbRect.top = trackRect.top + thumbPos;
    thumbRect.bottom = thumbRect.top + thumbHeight;
    
    // Final clamp to ensure thumb bottom doesn't exceed track bottom
    if (thumbRect.bottom > trackRect.bottom) {
        thumbRect.bottom = trackRect.bottom;
        thumbRect.top = thumbRect.bottom - thumbHeight;
    }
    
    return thumbRect;
}

void CThemedRichEdit::DrawThemedScrollbar(CDC* pDC)
{
    // Check if scrolling is needed
    int thumbHeight = GetThumbHeight();
    if (thumbHeight == 0) {
        return;  // No scrollbar needed
    }
    
    CRect trackRect = GetScrollbarTrackRect();
    
    // Draw track background (matches chat background)
    CBrush trackBrush(Theme::ChatBackground);
    pDC->FillRect(&trackRect, &trackBrush);
    
    // Draw thumb
    CRect thumbRect = GetThumbRect();
    if (!thumbRect.IsRectEmpty()) {
        DrawScrollbarThumb(pDC, thumbRect, m_bThumbHover, m_bThumbPressed);
    }
}

void CThemedRichEdit::DrawScrollbarThumb(CDC* pDC, const CRect& thumbRect, bool isHover, bool isPressed)
{
    COLORREF thumbColor;
    if (isPressed) {
        thumbColor = Theme::ScrollbarThumbHover;
    } else if (isHover) {
        thumbColor = Theme::ScrollbarThumbHover;
    } else {
        thumbColor = Theme::ScrollbarThumb;
    }
    
    // Draw rounded thumb
    CBrush thumbBrush(thumbColor);
    CPen pen(PS_SOLID, 1, thumbColor);
    CPen* pOldPen = pDC->SelectObject(&pen);
    CBrush* pOldBrush = pDC->SelectObject(&thumbBrush);
    
    int radius = min(4, (thumbRect.right - thumbRect.left) / 2);
    pDC->RoundRect(thumbRect, CPoint(radius, radius));
    
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
}

void CThemedRichEdit::OnPaint()
{
    // Let the base class paint the text content
    Default();
    
    // Ensure scrollbars stay hidden
    ShowScrollBar(SB_VERT, FALSE);
    ShowScrollBar(SB_HORZ, FALSE);
    
    // Now draw our scrollbar overlay on top
    CClientDC dc(this);
    DrawThemedScrollbar(&dc);
}

void CThemedRichEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CRichEditCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
    
    // Redraw to update scrollbar position
    Invalidate(FALSE);
}

void CThemedRichEdit::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect thumbRect = GetThumbRect();
    CRect trackRect = GetScrollbarTrackRect();
    
    // Handle thumb dragging
    if (m_bThumbPressed) {
        int totalLines = GetLineCount();
        int firstVisible = GetFirstVisibleLine();
        
        CRect clientRect;
        GetClientRect(&clientRect);
        
        CDC* pDC = GetDC();
        TEXTMETRIC tm;
        pDC->GetTextMetrics(&tm);
        ReleaseDC(pDC);
        
        int lineHeight = tm.tmHeight + tm.tmExternalLeading;
        if (lineHeight <= 0) lineHeight = 20;
        
        int visibleLines = clientRect.Height() / lineHeight;
        int scrollableLines = totalLines - visibleLines;
        
        int trackHeight = trackRect.Height();
        int thumbHeight = GetThumbHeight();
        int scrollableTrack = trackHeight - thumbHeight;
        
        if (scrollableTrack > 0 && scrollableLines > 0) {
            int deltaY = point.y - m_nDragStartY;
            int deltaLines = (int)((float)deltaY / scrollableTrack * scrollableLines);
            int newFirstLine = m_nDragStartScrollPos + deltaLines;
            
            // Clamp to valid range
            newFirstLine = max(0, min(scrollableLines, newFirstLine));
            
            // Scroll to the new position
            int linesToScroll = newFirstLine - firstVisible;
            if (linesToScroll != 0) {
                LineScroll(linesToScroll);
            }
            
            Invalidate(FALSE);
        }
        
        CRichEditCtrl::OnMouseMove(nFlags, point);
        return;
    }
    
    // Track hover state
    bool wasHover = m_bThumbHover;
    m_bThumbHover = thumbRect.PtInRect(point);
    
    if (wasHover != m_bThumbHover) {
        // Set up mouse tracking for leave notification
        if (m_bThumbHover) {
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = m_hWnd;
            TrackMouseEvent(&tme);
        }
        Invalidate(FALSE);
    }
    
    CRichEditCtrl::OnMouseMove(nFlags, point);
}

void CThemedRichEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
    CRect thumbRect = GetThumbRect();
    CRect trackRect = GetScrollbarTrackRect();
    
    if (thumbRect.PtInRect(point)) {
        // Start dragging the thumb
        m_bThumbPressed = true;
        m_nDragStartY = point.y;
        m_nDragStartScrollPos = GetFirstVisibleLine();
        
        SetCapture();
        Invalidate(FALSE);
        return;
    }
    else if (trackRect.PtInRect(point)) {
        // Click on track - page up or down
        int totalLines = GetLineCount();
        
        CRect clientRect;
        GetClientRect(&clientRect);
        
        CDC* pDC = GetDC();
        TEXTMETRIC tm;
        pDC->GetTextMetrics(&tm);
        ReleaseDC(pDC);
        
        int lineHeight = tm.tmHeight + tm.tmExternalLeading;
        if (lineHeight <= 0) lineHeight = 20;
        
        int visibleLines = clientRect.Height() / lineHeight;
        
        if (point.y < thumbRect.top) {
            // Page up
            LineScroll(-visibleLines);
        } else if (point.y > thumbRect.bottom) {
            // Page down
            LineScroll(visibleLines);
        }
        Invalidate(FALSE);
        return;
    }
    
    CRichEditCtrl::OnLButtonDown(nFlags, point);
}

void CThemedRichEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bThumbPressed) {
        m_bThumbPressed = false;
        ReleaseCapture();
        Invalidate(FALSE);
        return;
    }
    
    CRichEditCtrl::OnLButtonUp(nFlags, point);
}

void CThemedRichEdit::OnMouseLeave()
{
    if (m_bThumbHover && !m_bThumbPressed) {
        m_bThumbHover = false;
        Invalidate(FALSE);
    }
    
    CRichEditCtrl::OnMouseLeave();
}

BOOL CThemedRichEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // Scroll 3 lines per wheel notch
    int linesToScroll = -zDelta / WHEEL_DELTA * 3;
    LineScroll(linesToScroll);
    
    // Redraw scrollbar after wheel scroll
    Invalidate(FALSE);
    
    return TRUE;  // Handled
}

void CThemedRichEdit::OnSize(UINT nType, int cx, int cy)
{
    CRichEditCtrl::OnSize(nType, cx, cy);
    
    // Hide scrollbars and redraw
    ShowScrollBar(SB_VERT, FALSE);
    ShowScrollBar(SB_HORZ, FALSE);
    Invalidate(FALSE);
}

LRESULT CThemedRichEdit::OnSetText(WPARAM wParam, LPARAM lParam)
{
    LRESULT result = Default();
    
    // Hide scrollbars and redraw after text changes
    ShowScrollBar(SB_VERT, FALSE);
    ShowScrollBar(SB_HORZ, FALSE);
    Invalidate(FALSE);
    
    return result;
}

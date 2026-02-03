#pragma once

#include <afxwin.h>
#include <afxrich.h>
#include "Theme.h"

// Custom RichEdit control with themed scrollbar overlay
class CThemedRichEdit : public CRichEditCtrl
{
    DECLARE_DYNAMIC(CThemedRichEdit)

public:
    CThemedRichEdit();
    virtual ~CThemedRichEdit();
    
    // Call after control is created to set up themed scrollbar
    void InitThemedScrollbar();

protected:
    // Override to prevent native scrollbar from showing
    virtual void PreSubclassWindow();
    
    // Scrollbar dimensions
    static const int SCROLLBAR_WIDTH = 12;
    
    // Scrollbar drawing
    void DrawThemedScrollbar(CDC* pDC);
    void DrawScrollbarThumb(CDC* pDC, const CRect& thumbRect, bool isHover, bool isPressed);
    
    // Scrollbar calculations
    CRect GetScrollbarTrackRect();
    CRect GetThumbRect();
    int GetThumbHeight();
    int GetThumbPosition();
    
    // State tracking
    bool m_bThumbHover;
    bool m_bThumbPressed;
    int m_nDragStartY;
    int m_nDragStartScrollPos;
    bool m_bInitialized;

    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg LRESULT OnShowScrollBar(WPARAM wParam, LPARAM lParam);
};

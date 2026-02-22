#pragma once

#define _WINSOCKAPI_  // Prevent winsock.h from being included
#include <winsock2.h>
#include <afxwin.h>
#include <afxdialogex.h>
#include <afxcmn.h>
#include <afxrich.h>
#include "resource.h"
#include "ChatMessage.h"
#include "Theme.h"
#include "SettingsStore.h"
#include "ThemedRichEdit.h"
#include <vector>

// Forward declarations
class ChatEngine;

// Main application dialog
class CMainDlg : public CDialogEx
{
public:
    enum { IDD = IDD_MAIN_DIALOG };

    CMainDlg(CWnd* pParent = nullptr);
    virtual ~CMainDlg();

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnMinimize();
    afx_msg void OnMaximize();
    afx_msg void OnClose();
    afx_msg void OnSendMessage();
    afx_msg void OnAttachFile();
    afx_msg void OnCopyLastResponse();
    afx_msg void OnSettingsButton();
    afx_msg void OnSettingsClose();
    afx_msg void OnStubToggle();
    afx_msg void OnClearHistory();
    afx_msg void OnAttachmentDblClick();
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
    afx_msg BOOL OnNcActivate(BOOL bActive);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

    DECLARE_MESSAGE_MAP()

private:
    // UI Controls
    CStatic m_appIcon;
    CStatic m_titlebar;
    CButton m_btnMinimize;
    CButton m_btnMaximize;
    CButton m_btnClose;
    CThemedRichEdit m_chat;  // Use themed RichEdit with custom scrollbar
    CEdit m_input;
    CButton m_btnSend;
    CButton m_btnAttach;
    CButton m_btnCopyLastResponse;
    CButton m_btnSettings;
    CListBox m_attachmentList;
    CToolTipCtrl m_tooltip;

    CStatic m_settingsOverlay;
    CStatic m_settingsPanel;
    CStatic m_settingsTitle;
    CStatic m_settingsEndpointLabel;
    CEdit m_settingsEndpoint;
    CStatic m_settingsApiKeyLabel;
    CEdit m_settingsApiKey;
    CButton m_settingsStubToggle;
    CStatic m_settingsStubHint;
    CButton m_settingsClose;
    bool m_settingsVisible;
    CRect m_settingsOverlayRect;

    // Chat engine
    ChatEngine* m_chatEngine;

    // Pending attachments
    std::vector<FileAttachment> m_pendingAttachments;

    // Button state tracking
    Theme::ButtonState m_btnMinimizeState;
    Theme::ButtonState m_btnMaximizeState;
    Theme::ButtonState m_btnCloseState;
    Theme::ButtonState m_btnSendState;
    Theme::ButtonState m_btnAttachState;
    Theme::ButtonState m_btnCopyState;
    Theme::ButtonState m_btnSettingsState;
    BOOL m_bTrackingMouse;
    CBrush m_bgBrush;

    // Layout and rendering
    void LayoutControls();
    void AppendChatMessage(const ChatMessage& msg);
    void UpdateChatDisplay();
    bool IsChatNearBottom() const;
    void ScrollChatToBottomIfPinned(bool wasNearBottom);
    void SaveChatHistory();
    void LoadChatHistory();
    void LayoutSettingsOverlay();
    void ShowSettingsOverlay(bool show);
    void ApplySettingsState();
    void SaveSettingsFromUI();
    void PopulateSampleHistory();
    void RemoveAttachmentAtIndex(int index);
    std::wstring FindLatestAssistantMessage() const;
    
    // Button helper methods
    CRect GetButtonRect(int buttonID);
    void UpdateButtonState(int buttonID, Theme::ButtonState newState);
    void DrawRoundedBorder(CDC* pDC, const CRect& rect, int radius, COLORREF color);
};

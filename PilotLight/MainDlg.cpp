#include "MainDlg.h"
#include "BorderlessFrame.h"
#include "Theme.h"
#include "ChatEngine.h"
#include "FileUtils.h"
#include "RichTextRenderer.h"
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

// Constructor
CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MAIN_DIALOG, pParent)
    , m_chatEngine(nullptr)
    , m_btnMinimizeState(Theme::ButtonState::Normal)
    , m_btnMaximizeState(Theme::ButtonState::Normal)
    , m_btnCloseState(Theme::ButtonState::Normal)
    , m_btnSendState(Theme::ButtonState::Normal)
    , m_btnAttachState(Theme::ButtonState::Normal)
    , m_bTrackingMouse(FALSE)
{
    m_chatEngine = new ChatEngine();
    m_bgBrush.CreateSolidBrush(Theme::FrameBackground);
}

// Destructor
CMainDlg::~CMainDlg()
{
    if (m_chatEngine) {
        delete m_chatEngine;
        m_chatEngine = nullptr;
    }
}

// Data exchange
void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_APP_ICON, m_appIcon);
    DDX_Control(pDX, IDC_TITLEBAR, m_titlebar);
    DDX_Control(pDX, IDC_BTN_MINIMIZE, m_btnMinimize);
    DDX_Control(pDX, IDC_BTN_MAXIMIZE, m_btnMaximize);
    DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
    DDX_Control(pDX, IDC_CHAT, m_chat);
    DDX_Control(pDX, IDC_INPUT, m_input);
    DDX_Control(pDX, IDC_BTN_SEND, m_btnSend);
    DDX_Control(pDX, IDC_BTN_ATTACH, m_btnAttach);
    DDX_Control(pDX, IDC_ATTACHMENT_LIST, m_attachmentList);
}

// Message map
BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
    ON_WM_NCHITTEST()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_GETMINMAXINFO()
    ON_WM_DRAWITEM()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
    ON_WM_SETCURSOR()
    ON_BN_CLICKED(IDC_BTN_MINIMIZE, &CMainDlg::OnMinimize)
    ON_BN_CLICKED(IDC_BTN_MAXIMIZE, &CMainDlg::OnMaximize)
    ON_BN_CLICKED(IDC_BTN_CLOSE, &CMainDlg::OnClose)
    ON_BN_CLICKED(IDC_BTN_SEND, &CMainDlg::OnSendMessage)
    ON_BN_CLICKED(IDC_BTN_ATTACH, &CMainDlg::OnAttachFile)
    ON_BN_CLICKED(IDC_BTN_CLEAR_HISTORY, &CMainDlg::OnClearHistory)
    ON_WM_NCCALCSIZE()
    ON_WM_NCACTIVATE()
END_MESSAGE_MAP()

// Initialize dialog
BOOL CMainDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CBorderlessFrame::Apply(this);
    CBorderlessFrame::UpdateRegion(this, 16);

    // Load and set application icon
    HICON hIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PILOTLIGHT), 
                                    IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
    if (hIcon) {
        SetIcon(hIcon, TRUE);   // Set big icon
        SetIcon(hIcon, FALSE);  // Set small icon
    }

    // Set up titlebar
    m_titlebar.ModifyStyle(0, SS_CENTERIMAGE);  
    m_titlebar.SetFont(CFont::FromHandle(Theme::TitleFont()));
    m_titlebar.SetWindowText(L"PilotLight");

    // Apply owner-draw style to buttons
    m_btnMinimize.ModifyStyle(0, BS_OWNERDRAW);
    m_btnMaximize.ModifyStyle(0, BS_OWNERDRAW);
    m_btnClose.ModifyStyle(0, BS_OWNERDRAW);
    m_btnSend.ModifyStyle(0, BS_OWNERDRAW);
    m_btnAttach.ModifyStyle(0, BS_OWNERDRAW);

    // Set button text using Unicode escape sequences for reliable matching
    m_btnMinimize.SetWindowText(L"\u2212");  // Minus sign (U+2212)
    m_btnMaximize.SetWindowText(L"\u25A1");  // White square (U+25A1)
    m_btnClose.SetWindowText(L"\u00D7");     // Multiplication sign (U+00D7)
    m_btnSend.SetWindowText(L"\u2191");      // Up arrow (U+2191)
    m_btnAttach.SetWindowText(L"\U0001F4CE"); // Paperclip (U+1F4CE)

    // Set up chat display
    m_chat.ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0);  // Remove 3D border
    m_chat.ModifyStyle(WS_BORDER, 0);  // Remove standard border
    m_chat.SetFont(CFont::FromHandle(Theme::UIFont()));
    m_chat.SetReadOnly(TRUE);
    m_chat.SetBackgroundColor(FALSE, Theme::ChatBackground);
    m_chat.InitThemedScrollbar();  // Initialize our custom themed scrollbar

    // Set up input
    m_input.SetFont(CFont::FromHandle(Theme::UIFont()));

    // Set up tooltips
    m_tooltip.Create(this);
    m_tooltip.Activate(TRUE);
    m_tooltip.AddTool(&m_btnMinimize, L"Minimize");
    m_tooltip.AddTool(&m_btnMaximize, L"Maximize/Restore");
    m_tooltip.AddTool(&m_btnClose, L"Close");
    m_tooltip.AddTool(&m_btnSend, L"Send Message");
    m_tooltip.AddTool(&m_btnAttach, L"Attach Files");

    // Set initial window size and position
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    // Calculate window dimensions: 800px width, 75% of screen height
    int windowWidth = 900;
    int windowHeight = (screenHeight * 2) / 4;
    
    // Center the window on screen
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;
    
    // Set window position and size
    SetWindowPos(nullptr, windowX, windowY, windowWidth, windowHeight, SWP_NOZORDER);

    // Load chat history
    LoadChatHistory();
    UpdateChatDisplay();

    // Initial layout - force after window is fully created
    LayoutControls();
    
    // Post a message to trigger layout after dialog is fully shown
    PostMessage(WM_SIZE, 0, MAKELPARAM(0, 0));

    return TRUE;
}

// Pre-translate message for tooltips
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
    if (m_tooltip.GetSafeHwnd()) {
        m_tooltip.RelayEvent(pMsg);
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}

// Hit test for window dragging
LRESULT CMainDlg::OnNcHitTest(CPoint pt)
{
    return CBorderlessFrame::HitTest(this, pt);
}

// Handle window resize
void CMainDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
    
    // Update the window region for the new size
    CBorderlessFrame::UpdateRegion(this, 16);
    
    LayoutControls();
}

// Handle paint
void CMainDlg::OnPaint()
{
    CPaintDC dc(this);
    
    // Fill main background with frame color
    CRect clientRect;
    GetClientRect(&clientRect);
    
    CBrush bgBrush(Theme::FrameBackground);
    dc.FillRect(&clientRect, &bgBrush);
    
    // Draw rounded rectangle background for the chat area
    if (m_chat.GetSafeHwnd()) {
        // Calculate the outer chat area (larger than the actual control)
        const int margin = 16;
        const int titlebarHeight = 40;
        const int inputHeight = 80;
        
        CRect chatBgRect(margin, titlebarHeight + margin,
                         clientRect.right - margin, clientRect.bottom - inputHeight - margin * 2);
        
        // Draw a filled rounded rectangle as the chat background
        CBrush chatBgBrush(Theme::ChatBackground);
        CPen pen(PS_SOLID, 1, Theme::ChatBackground);  // Same color as fill (no visible border)
        CPen* pOldPen = dc.SelectObject(&pen);
        CBrush* pOldBrush = dc.SelectObject(&chatBgBrush);
        
        // 20px corner radius
        dc.RoundRect(chatBgRect, CPoint(20, 20));
        
        dc.SelectObject(pOldBrush);
        dc.SelectObject(pOldPen);
    }
}


// Layout all controls
void CMainDlg::LayoutControls()
{
    if (!m_chat.GetSafeHwnd()) return;

    CRect clientRect;
    GetClientRect(&clientRect);

    const int margin = 16;
    const int titlebarHeight = 40;   // Taller titlebar for larger icon/text
    const int buttonHeight = 32;     // Keep buttons same height
    const int inputHeight = 80;
    const int buttonWidth = 46;      // Wider buttons for edge-to-edge look
    const int sideControlWidth = 70;
    const int iconSize = 32;         // Larger icon
    const int iconMargin = 5;

    // App icon (vertically centered in titlebar)
    int iconTop = (titlebarHeight - iconSize) / 2;
    CRect iconRect(margin, iconTop, margin + iconSize, iconTop + iconSize);
    m_appIcon.MoveWindow(&iconRect);

    // Titlebar text - start after the icon, aligned with icon vertically
    int titleLeft = margin + iconSize + iconMargin;
    int titleTop = iconTop;  // Align with icon top
    CRect titlebarRect(titleLeft, titleTop, clientRect.right - 3 * buttonWidth, titlebarHeight);
    m_titlebar.MoveWindow(&titlebarRect);

    // Titlebar buttons (right-aligned, flush with top-right, smaller height)
    int buttonTop = (titlebarHeight - buttonHeight) / 2;  // Center buttons vertically
    CRect btnMinimizeRect(clientRect.right - 3 * buttonWidth, 0, 
                          clientRect.right - 2 * buttonWidth, buttonHeight);
    m_btnMinimize.MoveWindow(&btnMinimizeRect);

    CRect btnMaximizeRect(clientRect.right - 2 * buttonWidth, 0,
                          clientRect.right - buttonWidth, buttonHeight);
    m_btnMaximize.MoveWindow(&btnMaximizeRect);

    CRect btnCloseRect(clientRect.right - buttonWidth, 0,
                       clientRect.right, buttonHeight);
    m_btnClose.MoveWindow(&btnCloseRect);

    // Chat display - position with padding inside the rounded background area
    int chatRight = clientRect.right - margin;
    int cornerPadding = 8;  // Padding to keep text away from rounded corners
    CRect chatRect(margin + cornerPadding, titlebarHeight + margin + cornerPadding,
                   chatRight - cornerPadding, clientRect.bottom - inputHeight - margin * 2 - cornerPadding);
    m_chat.MoveWindow(&chatRect);

    // Update internal padding for the chat control after resize
    // Leave extra space on right for the custom scrollbar (12px scrollbar width)
    CRect chatClientRect;
    m_chat.GetClientRect(&chatClientRect);
    chatClientRect.DeflateRect(8, 8, 16, 8);  // Internal padding plus scrollbar space
    m_chat.SetRect(&chatClientRect);

    // Calculate button widths
    const int sendWidth = 70;
    const int attachWidth = 70;
    const int buttonsWidth = sendWidth + margin + attachWidth;
    
    // Input area - bottom left, leaving room for buttons on the right
    int inputTop = clientRect.bottom - inputHeight - margin;
    int inputRight = clientRect.right - buttonsWidth - margin * 2;
    CRect inputRect(margin, inputTop, inputRight, clientRect.bottom - margin);
    m_input.MoveWindow(&inputRect);

    // Send button - next to input
    int sendLeft = inputRight + margin;
    CRect btnSendRect(sendLeft, inputTop, sendLeft + sendWidth, clientRect.bottom - margin);
    m_btnSend.MoveWindow(&btnSendRect);

    // Attach button - to the right of send button, top portion only
    int attachLeft = sendLeft + sendWidth + margin;
    int attachHeight = 35;
    CRect btnAttachRect(attachLeft, inputTop, attachLeft + attachWidth, inputTop + attachHeight);
    m_btnAttach.MoveWindow(&btnAttachRect);

    // Attachment list - under attach button
    CRect attachListRect(attachLeft, inputTop + attachHeight + 5, attachLeft + attachWidth, clientRect.bottom - margin);
    m_attachmentList.MoveWindow(&attachListRect);
}

// Minimize window
void CMainDlg::OnMinimize()
{
    ShowWindow(SW_MINIMIZE);
}

// Maximize/restore window
void CMainDlg::OnMaximize()
{
    if (IsZoomed()) {
        ShowWindow(SW_RESTORE);
    } else {
        ShowWindow(SW_MAXIMIZE);
    }
}

// Close window
void CMainDlg::OnClose()
{
    SaveChatHistory();
    CDialogEx::OnOK();  // Use OnOK to properly close modal dialog
}

// Send message
void CMainDlg::OnSendMessage()
{
    CString inputText;
    m_input.GetWindowText(inputText);

    if (inputText.IsEmpty()) return;

    // Create user message
    ChatMessage userMsg(ChatMessage::Role::User, (LPCTSTR)inputText);
    userMsg.attachments = m_pendingAttachments;

    // Add to engine and display
    m_chatEngine->AddUserMessage(userMsg.content, userMsg.attachments);
    AppendChatMessage(userMsg);

    // Clear input and attachments
    m_input.SetWindowText(L"");
    m_pendingAttachments.clear();
    m_attachmentList.ResetContent();

    // Get assistant response
    ChatMessage assistantMsg = m_chatEngine->GetAssistantResponse();
    AppendChatMessage(assistantMsg);

    // Save history
    SaveChatHistory();
}

// Attach file
void CMainDlg::OnAttachFile()
{
    const wchar_t* filter = L"Supported Files\0*.png;*.jpg;*.jpeg;*.gif;*.bmp;*.pdf;*.txt;*.doc;*.docx\0All Files\0*.*\0\0";
    std::vector<std::wstring> files = FileUtils::SelectFiles(m_hWnd, filter, true);

    const size_t maxFileSize = 10 * 1024 * 1024;  // 10MB

    for (const auto& filePath : files) {
        // Validate file type
        if (!FileUtils::ValidateFileType(filePath)) {
            CString msg;
            msg.Format(L"File type not supported: %s", filePath.c_str());
            AfxMessageBox(msg);
            continue;
        }

        // Validate file size
        if (!FileUtils::ValidateFileSize(filePath, maxFileSize)) {
            CString msg;
            msg.Format(L"File too large (max 10MB): %s", filePath.c_str());
            AfxMessageBox(msg);
            continue;
        }

        // Read and encode file
        std::vector<BYTE> fileData;
        if (!FileUtils::ReadFileToBuffer(filePath, fileData)) {
            CString msg;
            msg.Format(L"Failed to read file: %s", filePath.c_str());
            AfxMessageBox(msg);
            continue;
        }

        FileAttachment attachment;
        attachment.filename = filePath.substr(filePath.find_last_of(L"\\") + 1);
        attachment.mimeType = FileUtils::GetMimeType(filePath);
        attachment.base64Data = FileUtils::EncodeBase64(fileData);
        attachment.originalSize = fileData.size();

        m_pendingAttachments.push_back(attachment);
        m_attachmentList.AddString(attachment.filename.c_str());
    }
}

// Clear history
void CMainDlg::OnClearHistory()
{
    if (AfxMessageBox(L"Clear all chat history?", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        m_chatEngine->ClearHistory();
        m_chat.SetWindowText(L"");
        SaveChatHistory();
    }
}

// Append chat message to display
void CMainDlg::AppendChatMessage(const ChatMessage& msg)
{
    std::wstring prefix;
    COLORREF bubbleColor;

    if (msg.role == ChatMessage::Role::User) {
        prefix = L"You: ";
        bubbleColor = Theme::Accent;
    } else if (msg.role == ChatMessage::Role::Assistant) {
        prefix = L"Assistant: ";
        bubbleColor = RGB(60, 60, 60);
    } else {
        prefix = L"System: ";
        bubbleColor = RGB(100, 100, 100);
    }

    RichTextRenderer::AppendBubble(m_chat, prefix + msg.content, Theme::Text, bubbleColor);
    RichTextRenderer::ScrollToBottom(m_chat);
}

// Update entire chat display
void CMainDlg::UpdateChatDisplay()
{
    m_chat.SetWindowText(L"");

    const auto& messages = m_chatEngine->GetHistory().GetMessages();
    for (const auto& msg : messages) {
        AppendChatMessage(msg);
    }
}

// Save chat history
void CMainDlg::SaveChatHistory()
{
    std::wstring appDataPath = FileUtils::GetAppDataPath();
    if (appDataPath.empty()) return;

    FileUtils::EnsureDirectoryExists(appDataPath);

    std::wstring historyPath = appDataPath + L"\\history.json";
    m_chatEngine->GetHistory().SaveToFile(historyPath);
}

// Load chat history
void CMainDlg::LoadChatHistory()
{
    std::wstring appDataPath = FileUtils::GetAppDataPath();
    if (appDataPath.empty()) return;

    std::wstring historyPath = appDataPath + L"\\history.json";
    m_chatEngine->GetHistory().LoadFromFile(historyPath);
}

// Set minimum window size
void CMainDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 600;  // Minimum width
    lpMMI->ptMinTrackSize.y = 400;  // Minimum height
    CDialogEx::OnGetMinMaxInfo(lpMMI);
}

// Get button rectangle in client coordinates
CRect CMainDlg::GetButtonRect(int buttonID)
{
    CWnd* pButton = GetDlgItem(buttonID);
    if (!pButton) return CRect(0, 0, 0, 0);
    
    CRect rect;
    pButton->GetWindowRect(&rect);
    ScreenToClient(&rect);
    return rect;
}

// Update button state and invalidate for redraw
void CMainDlg::UpdateButtonState(int buttonID, Theme::ButtonState newState)
{
    Theme::ButtonState* pState = nullptr;
    
    switch (buttonID) {
    case IDC_BTN_MINIMIZE:
        pState = &m_btnMinimizeState;
        break;
    case IDC_BTN_MAXIMIZE:
        pState = &m_btnMaximizeState;
        break;
    case IDC_BTN_CLOSE:
        pState = &m_btnCloseState;
        break;
    case IDC_BTN_SEND:
        pState = &m_btnSendState;
        break;
    case IDC_BTN_ATTACH:
        pState = &m_btnAttachState;
        break;
    }
    
    if (pState && *pState != newState) {
        *pState = newState;
        CWnd* pButton = GetDlgItem(buttonID);
        if (pButton) {
            pButton->Invalidate();
        }
    }
}

// Owner-draw button handler
void CMainDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (lpDrawItemStruct->CtlType != ODT_BUTTON) {
        CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
        return;
    }
    
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect = lpDrawItemStruct->rcItem;
    
    // Get button text (emoji)
    CWnd* pButton = GetDlgItem(nIDCtl);
    CString text;
    if (pButton) {
        pButton->GetWindowText(text);
    }
    
    // If maximized and this is the maximize button, change icon
    if (nIDCtl == IDC_BTN_MAXIMIZE && IsZoomed()) {
        text = L"❐";  // Restore icon when maximized (overlapping squares)
    }
    
    // Get current state
    Theme::ButtonState state = Theme::ButtonState::Normal;
    bool isCloseButton = false;
    
    switch (nIDCtl) {
    case IDC_BTN_MINIMIZE:
        state = m_btnMinimizeState;
        break;
    case IDC_BTN_MAXIMIZE:
        state = m_btnMaximizeState;
        break;
    case IDC_BTN_CLOSE:
        state = m_btnCloseState;
        isCloseButton = true;
        break;
    case IDC_BTN_SEND:
        state = m_btnSendState;
        break;
    case IDC_BTN_ATTACH:
        state = m_btnAttachState;
        break;
    }
    
    // Check if button is pressed
    if (lpDrawItemStruct->itemState & ODS_SELECTED) {
        state = Theme::ButtonState::Pressed;
    }
    
    // Draw the button
    Theme::DrawFramelessButton(pDC, rect, (LPCTSTR)text, state, isCloseButton);
}

// Mouse move handler for hover effects
void CMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // Set up mouse tracking if not already tracking
    if (!m_bTrackingMouse) {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        tme.dwHoverTime = HOVER_DEFAULT;
        
        if (TrackMouseEvent(&tme)) {
            m_bTrackingMouse = TRUE;
        }
    }
    
    // Check each button for hover state
    int buttons[] = { IDC_BTN_MINIMIZE, IDC_BTN_MAXIMIZE, IDC_BTN_CLOSE, IDC_BTN_SEND, IDC_BTN_ATTACH };
    
    for (int buttonID : buttons) {
        CRect buttonRect = GetButtonRect(buttonID);
        
        if (buttonRect.PtInRect(point)) {
            UpdateButtonState(buttonID, Theme::ButtonState::Hover);
        } else {
            UpdateButtonState(buttonID, Theme::ButtonState::Normal);
        }
    }
    
    CDialogEx::OnMouseMove(nFlags, point);
}

// Mouse leave handler to reset hover states
void CMainDlg::OnMouseLeave()
{
    m_bTrackingMouse = FALSE;
    
    // Reset all button states to normal
    UpdateButtonState(IDC_BTN_MINIMIZE, Theme::ButtonState::Normal);
    UpdateButtonState(IDC_BTN_MAXIMIZE, Theme::ButtonState::Normal);
    UpdateButtonState(IDC_BTN_CLOSE, Theme::ButtonState::Normal);
    UpdateButtonState(IDC_BTN_SEND, Theme::ButtonState::Normal);
    UpdateButtonState(IDC_BTN_ATTACH, Theme::ButtonState::Normal);
    
    CDialogEx::OnMouseLeave();
}

// Erase background handler to prevent gray border
BOOL CMainDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);
    pDC->FillSolidRect(&rect, Theme::FrameBackground);
    return TRUE;
}

// Control color handler for dark theme
HBRUSH CMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    // Set text and background colors for all controls
    pDC->SetTextColor(Theme::Foreground);
    pDC->SetBkColor(Theme::FrameBackground);
    
    switch (nCtlColor) {
    case CTLCOLOR_STATIC:
        // Static text (titlebar)
        return m_bgBrush;
        
    case CTLCOLOR_EDIT:
        // Edit controls (input)
        return m_bgBrush;
        
    case CTLCOLOR_LISTBOX:
        // Listbox (attachment list)
        return m_bgBrush;
        
    case CTLCOLOR_DLG:
        // Dialog background
        return m_bgBrush;
        
    default:
        return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
    }
}

// Handle NC calc size to remove non-client area border
void CMainDlg::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    if (bCalcValidRects) {      
        // Return without calling base class to remove non-client area entirely
        return;
    }
    CDialogEx::OnNcCalcSize(bCalcValidRects, lpncsp);
}

// Handle NC activate to prevent default Windows title bar rendering
BOOL CMainDlg::OnNcActivate(BOOL bActive)
{
    // Return TRUE to prevent Windows from drawing the default non-client area
    // This stops the ghosted title bar from appearing when the window loses focus
    // We still need to redraw our custom frame
    RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
    return TRUE;
}

// Handle set cursor to track mouse over child controls for hover effects
BOOL CMainDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    // Get the control ID of the window under the cursor
    int ctrlID = pWnd->GetDlgCtrlID();
    
    // List of buttons we track hover for
    int buttons[] = { IDC_BTN_MINIMIZE, IDC_BTN_MAXIMIZE, IDC_BTN_CLOSE, IDC_BTN_SEND, IDC_BTN_ATTACH };
    
    // Reset all button states first
    for (int buttonID : buttons) {
        if (buttonID != ctrlID) {
            UpdateButtonState(buttonID, Theme::ButtonState::Normal);
        }
    }
    
    // Set hover state for the button under cursor
    for (int buttonID : buttons) {
        if (buttonID == ctrlID) {
            UpdateButtonState(buttonID, Theme::ButtonState::Hover);
            break;
        }
    }
    
    return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}

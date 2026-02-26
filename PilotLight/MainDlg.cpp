#include "MainDlg.h"
#include "BorderlessFrame.h"
#include "Theme.h"
#include "ChatEngine.h"
#include "SettingsStore.h"
#include "FileUtils.h"
#include "RichTextRenderer.h"
#include <commctrl.h>
#include <algorithm>
#include <cstring>

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
    , m_btnCopyState(Theme::ButtonState::Normal)
    , m_btnSettingsState(Theme::ButtonState::Normal)
    , m_bTrackingMouse(FALSE)
    , m_settingsVisible(false)
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
    ON_WM_CONTEXTMENU()
    ON_BN_CLICKED(IDC_BTN_MINIMIZE, &CMainDlg::OnMinimize)
    ON_BN_CLICKED(IDC_BTN_MAXIMIZE, &CMainDlg::OnMaximize)
    ON_BN_CLICKED(IDC_BTN_CLOSE, &CMainDlg::OnClose)
    ON_BN_CLICKED(IDC_BTN_SEND, &CMainDlg::OnSendMessage)
    ON_BN_CLICKED(IDC_BTN_ATTACH, &CMainDlg::OnAttachFile)
    ON_BN_CLICKED(IDC_BTN_COPY_LAST_RESPONSE, &CMainDlg::OnCopyLastResponse)
    ON_BN_CLICKED(IDC_BTN_SETTINGS, &CMainDlg::OnSettingsButton)
    ON_BN_CLICKED(IDC_BTN_CLOSE_SETTINGS, &CMainDlg::OnSettingsClose)
    ON_BN_CLICKED(IDC_SETTINGS_STUB_TOGGLE, &CMainDlg::OnStubToggle)
    ON_BN_CLICKED(IDC_BTN_CLEAR_HISTORY, &CMainDlg::OnClearHistory)
    ON_LBN_DBLCLK(IDC_ATTACHMENT_LIST, &CMainDlg::OnAttachmentDblClick)
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
    if (!m_btnCopyLastResponse.GetSafeHwnd()) {
        m_btnCopyLastResponse.Create(L"", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(0, 0, 32, 32), this, IDC_BTN_COPY_LAST_RESPONSE);
    }
    m_btnCopyLastResponse.ModifyStyle(0, BS_OWNERDRAW);

    // Set button text using Unicode escape sequences for reliable matching
    m_btnMinimize.SetWindowText(L"\u2212");  // Minus sign (U+2212)
    m_btnMaximize.SetWindowText(L"\u25A1");  // White square (U+25A1)
    m_btnClose.SetWindowText(L"\u00D7");     // Multiplication sign (U+00D7)
    m_btnSend.SetWindowText(L"\u2191");      // Up arrow (U+2191)
    m_btnAttach.SetWindowText(L"\U0001F4CE"); // Paperclip (U+1F4CE)
    m_btnCopyLastResponse.SetWindowText(L"Copy");

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
    m_tooltip.AddTool(&m_btnCopyLastResponse, L"Copy latest assistant response");
    m_tooltip.AddTool(&m_attachmentList, L"No pending files");

    // Settings button & overlay
    if (!m_btnSettings.GetSafeHwnd()) {
        m_btnSettings.Create(L"", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(0, 0, 32, 32), this, IDC_BTN_SETTINGS);
    }
    m_btnSettings.SetWindowText(L"\u2699");
    m_btnSettings.ModifyStyle(0, BS_OWNERDRAW);
    m_btnSettings.SetFont(CFont::FromHandle(Theme::TitleFont()));
    m_tooltip.AddTool(&m_btnSettings, L"Settings");

    m_settingsOverlay.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, 0);
    m_settingsOverlay.ShowWindow(SW_HIDE);
    m_settingsPanel.Create(L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN, CRect(0, 0, 0, 0), this, 0);
    m_settingsPanel.ShowWindow(SW_HIDE);
    m_settingsTitle.Create(L"Settings & Sample Data", WS_CHILD | WS_VISIBLE | SS_LEFT, CRect(0, 0, 0, 0), &m_settingsPanel, 0);
    m_settingsTitle.SetFont(CFont::FromHandle(Theme::TitleFont()));
    m_settingsTitle.ShowWindow(SW_HIDE);
    m_settingsEndpointLabel.Create(L"OpenAI Endpoint", WS_CHILD | WS_VISIBLE | SS_LEFT, CRect(0, 0, 0, 0), &m_settingsPanel, IDC_STATIC);
    m_settingsEndpointLabel.ShowWindow(SW_HIDE);
    m_settingsEndpoint.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, CRect(0, 0, 0, 0), &m_settingsPanel, IDC_SETTINGS_ENDPOINT);
    m_settingsEndpoint.ShowWindow(SW_HIDE);
    m_settingsEndpoint.LimitText(256);
    m_settingsApiKeyLabel.Create(L"OpenAI API Key", WS_CHILD | WS_VISIBLE | SS_LEFT, CRect(0, 0, 0, 0), &m_settingsPanel, IDC_STATIC);
    m_settingsApiKeyLabel.ShowWindow(SW_HIDE);
    m_settingsApiKey.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, CRect(0, 0, 0, 0), &m_settingsPanel, IDC_SETTINGS_APIKEY);
    m_settingsApiKey.ShowWindow(SW_HIDE);
    m_settingsApiKey.LimitText(256);
    m_settingsStubToggle.Create(L"Enable stub/sample-data mode", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(0, 0, 0, 0), &m_settingsPanel, IDC_SETTINGS_STUB_TOGGLE);
    m_settingsStubToggle.ShowWindow(SW_HIDE);
    m_settingsStubHint.Create(L"Stub responses help exercise the UI without calling a real LLM.", WS_CHILD | WS_VISIBLE | SS_LEFT, CRect(0, 0, 0, 0), &m_settingsPanel, IDC_STATIC);
    m_settingsStubHint.ShowWindow(SW_HIDE);
    m_settingsClose.Create(L"Close", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0, 0, 0), &m_settingsPanel, IDC_BTN_CLOSE_SETTINGS);
    m_settingsClose.ShowWindow(SW_HIDE);

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

namespace {
    constexpr UINT ID_INPUT_UNDO = 0x5001;
    constexpr UINT ID_INPUT_CUT = 0x5002;
    constexpr UINT ID_INPUT_COPY = 0x5003;
    constexpr UINT ID_INPUT_PASTE = 0x5004;
    constexpr UINT ID_INPUT_DELETE = 0x5005;
    constexpr UINT ID_INPUT_SELECT_ALL = 0x5006;

    bool HasHttpScheme(const CString& value)
    {
        return value.Left(7).CompareNoCase(L"http://") == 0 ||
               value.Left(8).CompareNoCase(L"https://") == 0;
    }
}

// Pre-translate message for tooltips + clipboard shortcuts in chat input
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
    if (m_tooltip.GetSafeHwnd()) {
        m_tooltip.RelayEvent(pMsg);
    }

    CWnd* pFocus = GetFocus();
    const bool inputFocused =
        pFocus != nullptr &&
        (pFocus->GetSafeHwnd() == m_input.GetSafeHwnd() || ::IsChild(m_input.GetSafeHwnd(), pFocus->GetSafeHwnd()));

    if (pMsg->message == WM_KEYDOWN && inputFocused) {
        const bool ctrlDown = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        const bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

        if (ctrlDown) {
            switch (pMsg->wParam) {
            case 'A':
                m_input.SetSel(0, -1);
                return TRUE;
            case 'C':
                m_input.Copy();
                return TRUE;
            case 'V':
                m_input.Paste();
                return TRUE;
            case 'X':
                m_input.Cut();
                return TRUE;
            case 'Z':
                m_input.Undo();
                return TRUE;
            case VK_INSERT:
                m_input.Copy();
                return TRUE;
            }
        }

        if (shiftDown && pMsg->wParam == VK_INSERT) {
            m_input.Paste();
            return TRUE;
        }

        if (shiftDown && pMsg->wParam == VK_DELETE) {
            m_input.Cut();
            return TRUE;
        }
    }

    // Allow quick removal of pending attachments using Delete/Backspace
    if (pMsg->message == WM_KEYDOWN && m_attachmentList.GetSafeHwnd() &&
        ::GetFocus() == m_attachmentList.GetSafeHwnd() &&
        (pMsg->wParam == VK_DELETE || pMsg->wParam == VK_BACK)) {
        const int selectedIndex = m_attachmentList.GetCurSel();
        RemoveAttachmentAtIndex(selectedIndex);
        return TRUE;
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
    const int copyWidth = 70;
    const int buttonsWidth = sendWidth + margin + attachWidth + margin + copyWidth;
    const int settingsSize = 34;
    const int settingsSpacing = 12;
    
    // Input area - bottom left, leaving room for buttons on the right
    int inputTop = clientRect.bottom - inputHeight - margin;
    int inputRight = clientRect.right - buttonsWidth - margin * 2;
    CRect btnSettingsRect(margin, inputTop + (inputHeight - settingsSize) / 2, margin + settingsSize, inputTop + (inputHeight - settingsSize) / 2 + settingsSize);
    m_btnSettings.MoveWindow(&btnSettingsRect);

    int inputLeft = margin + settingsSize + settingsSpacing;
    CRect inputRect(inputLeft, inputTop, inputRight, clientRect.bottom - margin);
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

    // Copy latest response button
    int copyLeft = attachLeft + attachWidth + margin;
    CRect copyRect(copyLeft, inputTop, copyLeft + copyWidth, clientRect.bottom - margin);
    m_btnCopyLastResponse.MoveWindow(&copyRect);

    LayoutSettingsOverlay();
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
    m_tooltip.UpdateTipText(L"No pending files", &m_attachmentList);

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

    if (!m_pendingAttachments.empty()) {
        m_tooltip.UpdateTipText(L"Double-click or press Delete to remove", &m_attachmentList);
    }
}

void CMainDlg::OnAttachmentDblClick()
{
    const int selectedIndex = m_attachmentList.GetCurSel();
    RemoveAttachmentAtIndex(selectedIndex);
}

void CMainDlg::OnCopyLastResponse()
{
    const std::wstring latestResponse = FindLatestAssistantMessage();
    if (latestResponse.empty()) {
        AfxMessageBox(L"No assistant response available to copy yet.");
        return;
    }

    if (!::OpenClipboard(m_hWnd)) {
        AfxMessageBox(L"Unable to open clipboard.");
        return;
    }

    EmptyClipboard();
    const size_t bytes = (latestResponse.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!hMem) {
        CloseClipboard();
        AfxMessageBox(L"Unable to allocate clipboard memory.");
        return;
    }

    void* pMem = GlobalLock(hMem);
    if (pMem) {
        memcpy(pMem, latestResponse.c_str(), bytes);
        GlobalUnlock(hMem);
        SetClipboardData(CF_UNICODETEXT, hMem);
    } else {
        GlobalFree(hMem);
    }

    CloseClipboard();
}

void CMainDlg::RemoveAttachmentAtIndex(int index)
{
    if (index == LB_ERR || index < 0 || index >= static_cast<int>(m_pendingAttachments.size())) {
        return;
    }

    m_pendingAttachments.erase(m_pendingAttachments.begin() + index);
    m_attachmentList.DeleteString(index);

    if (m_pendingAttachments.empty()) {
        m_tooltip.UpdateTipText(L"No pending files", &m_attachmentList);
        return;
    }

    const int nextSelection = min(index, static_cast<int>(m_pendingAttachments.size()) - 1);
    m_attachmentList.SetCurSel(nextSelection);
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

bool CMainDlg::IsChatNearBottom()
{
    if (!m_chat.GetSafeHwnd()) {
        return true;
    }

    SCROLLINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    if (!m_chat.GetScrollInfo(SB_VERT, &si)) {
        return true;
    }

    int maxScrollablePos = si.nMax - static_cast<int>(si.nPage) + 1;
    if (maxScrollablePos < 0) {
        maxScrollablePos = 0;
    }

    constexpr int kPinnedScrollSlack = 2;
    return si.nPos >= (maxScrollablePos - kPinnedScrollSlack);
}

void CMainDlg::ScrollChatToBottomIfPinned(bool wasNearBottom)
{
    if (wasNearBottom) {
        RichTextRenderer::ScrollToBottom(m_chat);
    }
}

// Append chat message to display
void CMainDlg::AppendChatMessage(const ChatMessage& msg)
{
    const bool wasNearBottom = IsChatNearBottom();

    if (msg.role == ChatMessage::Role::User) {
        RichTextRenderer::AppendBubble(m_chat, L"You: " + msg.content, Theme::Text, Theme::Accent);
    } else if (msg.role == ChatMessage::Role::Assistant) {
        RichTextRenderer::AppendFormattedText(m_chat, L"Assistant:\r\n", Theme::Foreground);
        RichTextRenderer::AppendFormattedText(m_chat, msg.content + L"\r\n\r\n", Theme::Text);
    } else {
        RichTextRenderer::AppendFormattedText(m_chat, L"System: " + msg.content + L"\r\n\r\n", Theme::Foreground);
    }

    ScrollChatToBottomIfPinned(wasNearBottom);
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

std::wstring CMainDlg::FindLatestAssistantMessage() const
{
    if (!m_chatEngine) {
        return L"";
    }

    const auto& messages = m_chatEngine->GetHistory().GetMessages();
    for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
        if (it->role == ChatMessage::Role::Assistant) {
            return it->content;
        }
    }

    return L"";
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
    case IDC_BTN_COPY_LAST_RESPONSE:
        pState = &m_btnCopyState;
        break;
    case IDC_BTN_SETTINGS:
        pState = &m_btnSettingsState;
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
    case IDC_BTN_COPY_LAST_RESPONSE:
        state = m_btnCopyState;
        break;
    case IDC_BTN_SETTINGS:
        state = m_btnSettingsState;
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
    int buttons[] = { IDC_BTN_MINIMIZE, IDC_BTN_MAXIMIZE, IDC_BTN_CLOSE, IDC_BTN_SEND, IDC_BTN_ATTACH, IDC_BTN_COPY_LAST_RESPONSE, IDC_BTN_SETTINGS };
    
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
    UpdateButtonState(IDC_BTN_COPY_LAST_RESPONSE, Theme::ButtonState::Normal);
    UpdateButtonState(IDC_BTN_SETTINGS, Theme::ButtonState::Normal);
    
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
    if (pWnd == &m_settingsOverlay) {
        static CBrush overlayBrush(RGB(8, 8, 12));
        pDC->SetBkMode(OPAQUE);
        pDC->SetBkColor(RGB(8, 8, 12));
        return overlayBrush;
    }
    if (pWnd == &m_settingsPanel) {
        static CBrush panelBrush(Theme::ChatBackground);
        pDC->SetBkColor(Theme::ChatBackground);
        return panelBrush;
    }

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
    int buttons[] = { IDC_BTN_MINIMIZE, IDC_BTN_MAXIMIZE, IDC_BTN_CLOSE, IDC_BTN_SEND, IDC_BTN_ATTACH, IDC_BTN_COPY_LAST_RESPONSE, IDC_BTN_SETTINGS };
    
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

void CMainDlg::LayoutSettingsOverlay()
{
    if (!m_settingsPanel.GetSafeHwnd()) return;

    CRect clientRect;
    GetClientRect(&clientRect);
    m_settingsOverlayRect = clientRect;
    m_settingsOverlay.MoveWindow(&clientRect);

    int panelWidth = (std::min)(460, clientRect.Width() - 80);
    int panelHeight = (std::min)(320, clientRect.Height() - 120);
    int left = clientRect.left + (clientRect.Width() - panelWidth) / 2;
    int top = clientRect.top + (clientRect.Height() - panelHeight) / 2;
    CRect panelRect(left, top, left + panelWidth, top + panelHeight);
    m_settingsPanel.MoveWindow(&panelRect);

    CRect innerRect(0, 0, panelWidth, panelHeight);
    innerRect.DeflateRect(20, 18, 20, 18);

    CRect titleRect(innerRect.left, innerRect.top, innerRect.right, innerRect.top + 26);
    m_settingsTitle.MoveWindow(titleRect);

    CRect endpointLabel(innerRect.left, titleRect.bottom + 12, innerRect.right, titleRect.bottom + 32);
    m_settingsEndpointLabel.MoveWindow(endpointLabel);

    CRect endpointEdit(innerRect.left, endpointLabel.bottom + 4, innerRect.right, endpointLabel.bottom + 34);
    m_settingsEndpoint.MoveWindow(endpointEdit);

    CRect apiLabel(innerRect.left, endpointEdit.bottom + 12, innerRect.right, endpointEdit.bottom + 32);
    m_settingsApiKeyLabel.MoveWindow(apiLabel);

    CRect apiEdit(innerRect.left, apiLabel.bottom + 4, innerRect.right, apiLabel.bottom + 34);
    m_settingsApiKey.MoveWindow(apiEdit);

    CRect stubRect(innerRect.left, apiEdit.bottom + 18, innerRect.right, apiEdit.bottom + 38);
    m_settingsStubToggle.MoveWindow(stubRect);

    CRect hintRect(innerRect.left, stubRect.bottom + 6, innerRect.right - 10, stubRect.bottom + 40);
    m_settingsStubHint.MoveWindow(hintRect);

    CRect closeRect(innerRect.right - 90, innerRect.bottom - 32, innerRect.right, innerRect.bottom);
    m_settingsClose.MoveWindow(closeRect);
}

void CMainDlg::ShowSettingsOverlay(bool show)
{
    if (!show && !SaveSettingsFromUI()) {
        return;
    }

    int cmd = show ? SW_SHOW : SW_HIDE;
    m_settingsOverlay.ShowWindow(cmd);
    m_settingsPanel.ShowWindow(cmd);
    m_settingsTitle.ShowWindow(cmd);
    m_settingsEndpointLabel.ShowWindow(cmd);
    m_settingsEndpoint.ShowWindow(cmd);
    m_settingsApiKeyLabel.ShowWindow(cmd);
    m_settingsApiKey.ShowWindow(cmd);
    m_settingsStubToggle.ShowWindow(cmd);
    m_settingsStubHint.ShowWindow(cmd);
    m_settingsClose.ShowWindow(cmd);

    m_settingsVisible = show;

    if (show) {
        LayoutSettingsOverlay();
        ApplySettingsState();
        // Ensure overlay and panel are above the rich edit/chat controls.
        m_settingsOverlay.SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        m_settingsPanel.SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

        // Endpoint is the first field in this settings flow.
        m_settingsEndpoint.SetFocus();
    } else {
        m_input.SetFocus();
    }

    Invalidate(FALSE);
}

void CMainDlg::ApplySettingsState()
{
    const auto& settings = SettingsStore::Get();
    m_settingsEndpoint.SetWindowText(settings.endpoint.c_str());
    m_settingsApiKey.SetWindowText(settings.apiKey.c_str());
    m_settingsStubToggle.SetCheck(settings.stubModeEnabled ? BST_CHECKED : BST_UNCHECKED);
}

bool CMainDlg::SaveSettingsFromUI()
{
    CString endpoint;
    m_settingsEndpoint.GetWindowText(endpoint);
    endpoint.Trim();

    if (!endpoint.IsEmpty() && !HasHttpScheme(endpoint)) {
        AfxMessageBox(L"Endpoint must start with http:// or https://", MB_ICONWARNING | MB_OK);
        m_settingsEndpoint.SetFocus();
        m_settingsEndpoint.SetSel(0, -1);
        return false;
    }

    SettingsStore::SetEndpoint(endpoint.GetString());

    CString apiKey;
    m_settingsApiKey.GetWindowText(apiKey);
    apiKey.Trim();
    SettingsStore::SetApiKey(apiKey.GetString());

    SettingsStore::SetStubModeEnabled(m_settingsStubToggle.GetCheck() == BST_CHECKED);
    SettingsStore::Save();
    return true;
}

void CMainDlg::PopulateSampleHistory()
{
    if (!SettingsStore::IsStubModeEnabled()) return;

    m_chatEngine->ClearHistory();
    ChatHistory& history = m_chatEngine->GetHistory();

    ChatMessage userSample(ChatMessage::Role::User, L"Show me a sample PilotLight conversation.");
    ChatMessage assistantSample(ChatMessage::Role::Assistant,
        L"PilotLight stub mode is active. This canned response proves the UI works without an API call.");

    history.AddMessage(userSample);
    history.AddMessage(assistantSample);

    UpdateChatDisplay();
}

void CMainDlg::OnSettingsButton()
{
    ShowSettingsOverlay(true);
}

void CMainDlg::OnSettingsClose()
{
    ShowSettingsOverlay(false);
}

void CMainDlg::OnStubToggle()
{
    bool enabled = (m_settingsStubToggle.GetCheck() == BST_CHECKED);
    SettingsStore::SetStubModeEnabled(enabled);
    SettingsStore::Save();

    if (enabled) {
        PopulateSampleHistory();
    } else {
        LoadChatHistory();
        UpdateChatDisplay();
    }
}

void CMainDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
    if (!m_input.GetSafeHwnd()) {
        CDialogEx::OnContextMenu(pWnd, point);
        return;
    }

    CWnd* pFocus = GetFocus();
    const bool inputFocused =
        pFocus != nullptr &&
        (pFocus->GetSafeHwnd() == m_input.GetSafeHwnd() || ::IsChild(m_input.GetSafeHwnd(), pFocus->GetSafeHwnd()));

    const bool inputTargeted =
        pWnd != nullptr &&
        (pWnd->GetSafeHwnd() == m_input.GetSafeHwnd() || ::IsChild(m_input.GetSafeHwnd(), pWnd->GetSafeHwnd()));

    if (!inputFocused && !inputTargeted) {
        CDialogEx::OnContextMenu(pWnd, point);
        return;
    }

    if (point.x == -1 && point.y == -1) {
        CRect rect;
        m_input.GetWindowRect(&rect);
        point = CPoint(rect.left + 16, rect.top + 16);
    }

    int selStart = 0;
    int selEnd = 0;
    m_input.GetSel(selStart, selEnd);
    const bool hasSelection = selEnd > selStart;

    CMenu menu;
    if (!menu.CreatePopupMenu()) {
        return;
    }

    menu.AppendMenu(MF_STRING, ID_INPUT_UNDO, L"Undo");
    menu.AppendMenu(MF_SEPARATOR);
    menu.AppendMenu(MF_STRING, ID_INPUT_CUT, L"Cut");
    menu.AppendMenu(MF_STRING, ID_INPUT_COPY, L"Copy");
    menu.AppendMenu(MF_STRING, ID_INPUT_PASTE, L"Paste");
    menu.AppendMenu(MF_STRING, ID_INPUT_DELETE, L"Delete");
    menu.AppendMenu(MF_SEPARATOR);
    menu.AppendMenu(MF_STRING, ID_INPUT_SELECT_ALL, L"Select All");

    if (!m_input.CanUndo()) {
        menu.EnableMenuItem(ID_INPUT_UNDO, MF_BYCOMMAND | MF_GRAYED);
    }
    if (!hasSelection) {
        menu.EnableMenuItem(ID_INPUT_CUT, MF_BYCOMMAND | MF_GRAYED);
        menu.EnableMenuItem(ID_INPUT_COPY, MF_BYCOMMAND | MF_GRAYED);
        menu.EnableMenuItem(ID_INPUT_DELETE, MF_BYCOMMAND | MF_GRAYED);
    }
    if (!::IsClipboardFormatAvailable(CF_UNICODETEXT) && !::IsClipboardFormatAvailable(CF_TEXT)) {
        menu.EnableMenuItem(ID_INPUT_PASTE, MF_BYCOMMAND | MF_GRAYED);
    }

    UINT selected = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_RIGHTBUTTON, point.x, point.y, this);
    switch (selected) {
    case ID_INPUT_UNDO:
        m_input.Undo();
        break;
    case ID_INPUT_CUT:
        m_input.Cut();
        break;
    case ID_INPUT_COPY:
        m_input.Copy();
        break;
    case ID_INPUT_PASTE:
        m_input.Paste();
        break;
    case ID_INPUT_DELETE:
        m_input.ReplaceSel(L"");
        break;
    case ID_INPUT_SELECT_ALL:
        m_input.SetSel(0, -1);
        break;
    default:
        break;

    }
}

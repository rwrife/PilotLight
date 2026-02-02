#define _WINSOCKAPI_
#include <winsock2.h>
#include <afxwin.h>
#include <afxdlgs.h>
#include "MainDlg.h"

// PilotLight application object
class CPilotLightApp : public CWinApp
{
public:
    virtual BOOL InitInstance();
};

// Single global app instance
CPilotLightApp theApp;

BOOL CPilotLightApp::InitInstance()
{
    // Enable per-monitor DPI awareness (Windows 10+)
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    CWinApp::InitInstance();

    // Enable common controls
    INITCOMMONCONTROLSEX icex = {};
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    // Initialize RichEdit (required for CRichEditCtrl)
    if (!AfxInitRichEdit2())
    {
        AfxMessageBox(L"Failed to initialize RichEdit.");
        return FALSE;
    }

    // Create and show main dialog
    CMainDlg dlg;
    m_pMainWnd = &dlg;

    dlg.DoModal();

    // Exit app when dialog closes
    return FALSE;
}

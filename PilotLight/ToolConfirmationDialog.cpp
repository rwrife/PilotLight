#include "pch.h"
#include "ToolConfirmationDialog.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CToolConfirmationDialog, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_ALLOW_ONCE, &CToolConfirmationDialog::OnAllowOnce)
    ON_BN_CLICKED(IDC_BTN_ALWAYS_ALLOW, &CToolConfirmationDialog::OnAlwaysAllow)
    ON_BN_CLICKED(IDC_BTN_NEVER_ALLOW, &CToolConfirmationDialog::OnNeverAllow)
END_MESSAGE_MAP()

ToolPermissionPolicy ToolPermissionStore::GetPolicy(const std::wstring& toolName) const {
    auto it = m_policies.find(toolName);
    if (it == m_policies.end()) {
        return ToolPermissionPolicy::Ask;
    }

    return it->second;
}

void ToolPermissionStore::SetPolicy(const std::wstring& toolName, ToolPermissionPolicy policy) {
    if (policy == ToolPermissionPolicy::Ask) {
        m_policies.erase(toolName);
        return;
    }

    m_policies[toolName] = policy;
}

CToolConfirmationDialog::CToolConfirmationDialog(
    const CString& toolName,
    const CString& actionSummary,
    CWnd* pParent)
    : CDialogEx(IDD_TOOL_CONFIRM_DIALOG, pParent)
    , m_toolName(toolName)
    , m_actionSummary(actionSummary) {
}

ToolConfirmationDecision CToolConfirmationDialog::Prompt(
    const CString& toolName,
    const CString& actionSummary,
    CWnd* pParent) {
    CToolConfirmationDialog dlg(toolName, actionSummary, pParent);
    INT_PTR result = dlg.DoModal();

    switch (result) {
    case ID_TOOL_ALLOW_ONCE:
        return ToolConfirmationDecision::AllowOnce;
    case ID_TOOL_ALWAYS_ALLOW:
        return ToolConfirmationDecision::AlwaysAllow;
    case ID_TOOL_NEVER_ALLOW:
        return ToolConfirmationDecision::NeverAllow;
    default:
        return ToolConfirmationDecision::Cancel;
    }
}

BOOL CToolConfirmationDialog::OnInitDialog() {
    CDialogEx::OnInitDialog();

    CString title;
    title.Format(L"Allow %s?", m_toolName.GetString());
    SetWindowText(title);

    CString message;
    message.Format(
        L"PilotLight wants to run the tool '%s'.\r\n\r\nRequested action:\r\n%s\r\n\r\nChoose how to proceed.",
        m_toolName.GetString(),
        m_actionSummary.GetString());

    if (CWnd* messageLabel = GetDlgItem(IDC_TOOL_CONFIRM_MESSAGE)) {
        messageLabel->SetWindowText(message);
    }

    if (CWnd* allowOnce = GetDlgItem(IDC_BTN_ALLOW_ONCE)) {
        allowOnce->SetFocus();
    }

    return FALSE;
}

void CToolConfirmationDialog::OnAllowOnce() {
    EndDialog(ID_TOOL_ALLOW_ONCE);
}

void CToolConfirmationDialog::OnAlwaysAllow() {
    EndDialog(ID_TOOL_ALWAYS_ALLOW);
}

void CToolConfirmationDialog::OnNeverAllow() {
    EndDialog(ID_TOOL_NEVER_ALLOW);
}

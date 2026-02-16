#pragma once

#include <afxwin.h>
#include <afxdialogex.h>
#include <map>
#include <string>

enum class ToolConfirmationDecision {
    Cancel = 0,
    AllowOnce,
    AlwaysAllow,
    NeverAllow
};

enum class ToolPermissionPolicy {
    Ask = 0,
    AlwaysAllow,
    NeverAllow
};

class ToolPermissionStore {
public:
    ToolPermissionPolicy GetPolicy(const std::wstring& toolName) const;
    void SetPolicy(const std::wstring& toolName, ToolPermissionPolicy policy);

private:
    std::map<std::wstring, ToolPermissionPolicy> m_policies;
};

class CToolConfirmationDialog : public CDialogEx {
public:
    CToolConfirmationDialog(const CString& toolName, const CString& actionSummary, CWnd* pParent = nullptr);

    static ToolConfirmationDecision Prompt(
        const CString& toolName,
        const CString& actionSummary,
        CWnd* pParent = nullptr);

protected:
    BOOL OnInitDialog() override;
    afx_msg void OnAllowOnce();
    afx_msg void OnAlwaysAllow();
    afx_msg void OnNeverAllow();

    DECLARE_MESSAGE_MAP()

private:
    CString m_toolName;
    CString m_actionSummary;
};

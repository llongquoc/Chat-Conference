
// ClientDlg.h : header file
//


#pragma once
#pragma warning(disable:4996)

#include"LoginDlg.h"


// CClientDlg dialog
class CClientDlg : public CDialogEx
{
// Construction
public:
	CClientDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonConnect();
private:
	LoginDlg * _csm;

	char* ConvertToChar(const CString &s);

	CString userOnline;
	int level;
	int isLogon;
	CString m_userName;
	CString m_password;
	CString m_message;
	CString m_userNamePrivate;
	CString IP;
	bool _check = false;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

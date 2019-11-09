#pragma once
#pragma warning(disable:4996)

#include"PrivateChatDlg.h"

// GroupChatDlg dialog


class GroupChatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(GroupChatDlg)

public:
	GroupChatDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~GroupChatDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GROUPCHAT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSend();
private:
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
public:
	CString _message;
	afx_msg void OnBnClickedButtonLogout();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString _name;
	CString _onlineUsers;
	afx_msg void OnBnClickedButtonPrivatechat();
	CString _inputPrivate;

	static PrivateChatDlg *_csm[MAX_CLIENTS_ONLINE];
	static int _numberPrivate;
	static void runPrivateDlg();
	static bool _running;

	thread * _thConnect;
	static bool runConnect(HWND m_hWnd);
	CEdit _output;
};

int GroupChatDlg::_numberPrivate = 0;
PrivateChatDlg * GroupChatDlg::_csm[MAX_CLIENTS_ONLINE];
bool GroupChatDlg::_running = true;

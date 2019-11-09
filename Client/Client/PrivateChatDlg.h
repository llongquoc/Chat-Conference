#pragma once
#pragma warning(disable:4996)


// PrivateChatDlg dialog
#define MAX_FILE_SIZE 1024*1024*5


class PrivateChatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(PrivateChatDlg)

public:
	PrivateChatDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~PrivateChatDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRIVATECHAT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString _name;
	CString _partner;
	CString _messagePrivate;
	afx_msg void OnBnClickedButtonSend();
	CString _fileName;
	void Cancel()
	{
		CDialogEx::OnCancel();
	}
	afx_msg void OnBnClickedButtonSendfile();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEdit _outputPrivate;
	afx_msg void OnBnClickedButton1();

	int _numberOfPacket;
	CString _fileNameReceive;
	CString * _packet;
	int _numberPacketReceive;
	CString _fileNameSend;
};

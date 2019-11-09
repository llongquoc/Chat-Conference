
// ServerDlg.h : header file
//

#ifndef SERVERDLG_H
#define SERVERDLG_H


#include <afxwin.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
using namespace std;



#define PORT 25000
#define WM_SOCKET WM_USER+1
#define MAX_CLIENTS	500
#define MAX_BUFFER_SIZE (8 * 1024)
#define MAX_BUFF_SIZE (4 * 1024)

// CServerDlg dialog
class CServerDlg : public CDialogEx
{
// Construction
public:
	CServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
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
private:
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	string ConvertToString(const CString &s);
	void mSend(SOCKET sk, CString Command);
	int mRecv(SOCKET sk, CString &Command);
	bool readDatabase();
	string toTwoCharFromInt(int n); // n < 100
	void AppendTextToEditCtrl(CEdit& edit, LPCTSTR pszText);


	struct SockName
	{
		SOCKET sockClient;
		string name;
		bool onLine;
	};

	map<string, string> Data;
	SOCKET sockServer;
	struct sockaddr_in serverAdd;
	fstream dataFile;
	bool hasData;
	int buffLength;
	int number_Socket;
	SockName *pSock;
	CString Command;
	CString msgBox;
	CString onlBox;

public:
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedListen();
	void OnCancel();

	CEdit m_msgBox;
	CEdit m_onlBox;

	char m_Buffer[MAX_BUFFER_SIZE];
	int mSend(SOCKET sk, const char *buf, int len);
	int mRecv(SOCKET sk, char *buf);
};



#endif SERVERDLG_H
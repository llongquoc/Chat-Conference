#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CServerDlg dialog



CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_msgBox);
	DDX_Control(pDX, IDC_EDIT2, m_onlBox);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET, SockMsg)
	ON_BN_CLICKED(ID_LISTEN, &CServerDlg::OnBnClickedListen)
	ON_BN_CLICKED(ID_STOP, &CServerDlg::OnBnClickedStop)
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	// TODO: Add extra initialization here
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// My functions

void splitLine(string &user, string &pass, const string &line)
{
	size_t p = line.find(" ");
	if (p != string::npos && p + 1 < line.size())
	{
		user = line.substr(0, p);
		pass = line.substr(p + 1, int(line.size()) - p - 1);
	}
}
bool CServerDlg::readDatabase()
{
	dataFile.open("dataBase.txt", ios::in);
	if (dataFile) {
		string line, user, pass;
		while (getline(dataFile, line) && line.size() > 4) {
			splitLine(user, pass, line);
			Data[user] = pass;
		}
		return 1; 
	}
	return 0;
}


string CServerDlg::ConvertToString(const CString &s)
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize + 1];
	memset(pAnsiString, 0, nSize + 1);
	wcstombs(pAnsiString, s, nSize + 1);
	string res = pAnsiString;
	delete []pAnsiString;
	return res;
}


void CServerDlg::mSend(SOCKET sk, CString Command)
{
	Command = _T("**********") + Command;
	int Len = Command.GetLength();
	Len += Len;
	PBYTE sendBuff = new BYTE[Len + 2];//
	memset(sendBuff, 0, Len + 2);//
	memcpy(sendBuff, (PBYTE)(LPCTSTR)Command, Len);
	send(sk, (char*)&Len, sizeof(Len), 0);
	send(sk, (char*)sendBuff, Len, 0);
	delete []sendBuff;
}

int CServerDlg::mRecv(SOCKET sk, CString &Command)
{
	//
	recv(sk, (char *)&buffLength, sizeof(int), 0);
	PBYTE buffer = new BYTE[buffLength + 2];//
	memset(buffer, 0, buffLength + 2);//
	recv(sk, (char *)buffer, buffLength, 0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;
	delete []buffer;
	while (int(Command.GetLength()) > 0 && Command[0] == _T('*'))
		Command.Delete(0, 1);
	if (int(Command.GetLength()) < 2)
		return -1;
	return 0;
}


void CServerDlg::OnCancel() {
	// ghi xuong Database
	if (hasData) remove("dataBase.txt");
	dataFile.open("dataBase.txt", ios::out);
	for (auto it = Data.begin(); it != Data.end(); ++it) {
		dataFile << it->first << " ";
		dataFile << it->second << "\n";
	}
	dataFile.close();
	// dong tat ca ket noi
	for (int i = 0; i < number_Socket; ++i)
		closesocket(pSock[i].sockClient);
	delete[]pSock;

	CDialogEx::OnCancel();
}


void CServerDlg::OnBnClickedStop()
{
	OnCancel(); // ket thuc
}


void CServerDlg::OnBnClickedListen()
{
	UpdateData();
	sockServer = socket(AF_INET, SOCK_STREAM, 0);
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_port = htons(PORT);
	serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockServer, (SOCKADDR *)&serverAdd, sizeof(serverAdd));
	listen(sockServer, 5);
	int err = WSAAsyncSelect(sockServer, m_hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);
	if (err)
		AfxMessageBox(L"Cant call WSAAsyncSelect"); 
	GetDlgItem(ID_LISTEN)->EnableWindow(FALSE);
	number_Socket = 0;
	pSock = new SockName[MAX_CLIENTS];
	hasData = readDatabase();	// read data to 'Data' from database 
	msgBox = _T("");
	onlBox = _T("");
	m_msgBox.SetWindowText(_T("Waiting for connections..."));
	m_onlBox.SetWindowText(_T(""));
}

string CServerDlg::toTwoCharFromInt(int n) 
{
	if (n > 99) return "99";
	string s = to_string(n);
	if (n < 10) s = "0" + s;
	return s;
}

void CServerDlg::AppendTextToEditCtrl(CEdit& edit, LPCTSTR pszText)
{
	// get the initial text length
	int nLength = edit.GetWindowTextLength();
	// put the selection at the end of text
	edit.SetSel(nLength, nLength);
	// replace the selection
	edit.ReplaceSel(pszText);
}

LRESULT CServerDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		// Display the error and close the socket
		closesocket(wParam);
		return 0;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
	{
		// tao ket noi , ko tang number_Socket vi chua login
		pSock[number_Socket].sockClient = accept(wParam, NULL, NULL);
		if (number_Socket == 0) 
			m_msgBox.SetWindowText(_T(""));
		//GetDlgItem(ID_LISTEN)->EnableWindow(FALSE);
		break;
	}
	case FD_READ:
	{	
		int post = -1;
		for (int i = 0; i < number_Socket; i++){
			if (pSock[i].sockClient == wParam){
				post = i;
				break;
			}
		}
		CString recvStr; // message client gui qua luu vao
		if (mRecv(wParam, recvStr) < 0)
			break;
		//AfxMessageBox(recvStr);
		string tmp = ConvertToString(recvStr);
		int flag = stoi(tmp.substr(0, 2));
		int Len = recvStr.GetLength();
		int userLen, passLen;
		string user, pass;

		switch (flag / 10)
		{
		case 1: // Sign up
		{
			userLen = stoi(tmp.substr(2, 2));
			passLen = stoi(tmp.substr(userLen + 4, 2));
			user = tmp.substr(4, userLen);
			pass = tmp.substr(userLen + 6, passLen);
			if (int(Data.size()) > 0) // co database
			{
				if (Data.find(user) != Data.end()) // user da ton tai
				{
					// tao thong diep signup fail ve client
					Command = _T("11");
					msgBox = ("[" + user + "] sign up failed.\r\n").c_str();
				}
				else 
				{
					// add vao database
					Data[user] = pass;
					// tao thong diep signup success ve client
					Command = _T("10");
					// thong bao len server
					msgBox = ("[" + user + "] has just signed up.\r\n").c_str();
				}
			}
			else {
				// add vao database
				Data[user] = pass;
				// tao thong diep signup success ve client
				Command = _T("10");
				// thong bao len server
				msgBox = ("[" + user + "] has just signed up.\r\n").c_str();
			}
			AppendTextToEditCtrl(m_msgBox, msgBox);
			mSend(wParam, Command); // thong bao ve client do

			break;
		}
		case 2: // Log in, log out
		{
			
			if (flag % 10 == 0) // Log in
			{
				userLen = stoi(tmp.substr(2, 2));
				passLen = stoi(tmp.substr(userLen + 4, 2));
				user = tmp.substr(4, userLen);
				pass = tmp.substr(userLen + 6, passLen);

				if (int(Data.size()) == 0 || Data.find(user) == Data.end() || Data.at(user) != pass)
				{
					msgBox = ("[" + user + "] login failed.\r\n").c_str();
					AppendTextToEditCtrl(m_msgBox, msgBox);
					Command = _T("21");
					mSend(wParam, Command);
				}
				else
				{
					bool coincide = false;
					string s = "";
					for (int i = 0; i < number_Socket; ++i) {
						if (user != pSock[i].name) {
							s += toTwoCharFromInt(int(pSock[i].name.size()));
							s += pSock[i].name;
						}
					}
					for (int i = 0; i < number_Socket; i++)
					{
						if (user == pSock[i].name) // Kiem tra trung ten user (da ket noi o may khac)
						{
							// gui thong diep log out ve user cu
							Command = _T("22");
							mSend(pSock[i].sockClient, Command);
							//closesocket(pSock[i].sockClient); // ko co
							// thong bao tren server
							msgBox = ("[" + user + "] logout and login again.\r\n").c_str();
							AppendTextToEditCtrl(m_msgBox, msgBox);
							// luu de dia chi
							pSock[i].sockClient = wParam;
							pSock[i].onLine = true;
							// thong diep login success, list user ve user moi
							Command = ("20" + s).c_str();
							mSend(wParam, Command);
							coincide = true;
							break;
						}
					}

					if (coincide == false)
					{
						// luu thong tin user moi
						pSock[number_Socket].sockClient = wParam;
						pSock[number_Socket].name = user;
						pSock[number_Socket].onLine = true;
						// Tang so user dang online
						number_Socket++;
						// Thong bao tren server
						msgBox = ("[" + user + "] has just logged in.\r\n").c_str();
						AppendTextToEditCtrl(m_msgBox, msgBox);
						onlBox = (user + "\r\n").c_str();
						AppendTextToEditCtrl(m_onlBox, onlBox);
						// thong diep log in thanh cong cho client
						Command = ("20" + s).c_str();
						mSend(wParam, Command);
						// thong bao cho cac client khac
						Command = ("23" + user).c_str();
						for (int i = 0; i < number_Socket; i++) {
							if (pSock[i].name != user) 
								mSend(pSock[i].sockClient, Command);
						}
					}
				}
			}
			else // Log out
			{
				user = pSock[post].name;
				// update pSock
				number_Socket--;
				pSock[post].sockClient = pSock[number_Socket].sockClient;
				pSock[post].name = pSock[number_Socket].name;
				// thong bao tren server
				msgBox = ("[" + user + "] has just logged out.\r\n").c_str();
				AppendTextToEditCtrl(m_msgBox, msgBox);
				//  thong bao cac client khac va cap nhat list online tren server
				onlBox = _T("");
				Command = ("24" + user).c_str();
				for (int i = 0; i < number_Socket; i++) {
					mSend(pSock[i].sockClient, Command);
					onlBox += (pSock[i].name + "\r\n").c_str();
				}
				m_onlBox.SetWindowText(onlBox);
			}
			
			break;
		}
		case 3: // Public chat
		{
			CString message = recvStr.Mid(2, Len - 2);
			user = pSock[post].name;
			msgBox = ("[" + user + "] says to Public: ").c_str();
			msgBox += message + _T(".\r\n");
			AppendTextToEditCtrl(m_msgBox, msgBox);
			Command = ("30" + toTwoCharFromInt(int(user.size())) + user).c_str();
			Command += message;
			for (int i = 0; i < number_Socket; i++) {
				if (i != post) mSend(pSock[i].sockClient, Command);
			}

			break;
		}
		case 4: // Private chat
		{
			int l = stoi(tmp.substr(2, 2));
			string sendtoUser = tmp.substr(4, l);
			CString message = recvStr.Mid(l + 4, Len - l - 4);
			user = pSock[post].name;
			msgBox = ("[" + user + "] says to [" + sendtoUser + "]: ").c_str();
			msgBox += message + _T(".\r\n");
			AppendTextToEditCtrl(m_msgBox, msgBox);
			Command = ("40" + toTwoCharFromInt(int(user.size())) + user).c_str();
			Command += message;
			for (int i = 0; i < number_Socket; i++) {
				if (pSock[i].name == sendtoUser) {
					mSend(pSock[i].sockClient, Command);
					break;
				}
			}

			break;
		}
		case 5: // File transfer
		{
			string sendtoUser;
			user = pSock[post].name;
			if (flag % 10 == 2) // gui file thanh cong
			{
				sendtoUser = tmp.substr(2, Len - 2);
				msgBox = ("[" + sendtoUser + "] sent file successfully to [" + user + "].\r\n").c_str();
				AppendTextToEditCtrl(m_msgBox, msgBox);
				Command = ("52" + user).c_str();
				for (int i = 0; i < number_Socket; i++) {
					if (pSock[i].name == sendtoUser) {
						mSend(pSock[i].sockClient, Command);
						break;
					}
				}
			}
			else if (flag % 10 == 0) //  gui ten file
			{
				int l = stoi(tmp.substr(2, 2));
				sendtoUser = tmp.substr(4, l);
				int numberChar = stoi(tmp.substr(l + 4, 7));
				msgBox = ("[" + user + "] sending file to [" + sendtoUser + "]...\r\n").c_str();
				AppendTextToEditCtrl(m_msgBox, msgBox);
				//
				int dest = -1;
				for (int i = 0; i < number_Socket; i++) {
					if (pSock[i].name == sendtoUser) {
						dest = pSock[i].sockClient;
						break;
					}
				}
				//
				CString temp = recvStr.Mid(l + 4, Len - l - 4);
				Command = ("50" + toTwoCharFromInt(int(user.size())) + user).c_str();
				Command += temp;
				mSend(dest, Command);

				char * packet = new char[MAX_BUFFER_SIZE];
				while (numberChar > 0) {
					memset(packet, 0, MAX_BUFFER_SIZE);
					int buffRecv = mRecv(wParam, packet);
					numberChar -= buffRecv;
					mSend(dest, packet, buffRecv);
				}

			}
			//else if (flag % 10 == 1) // gui 1 packet
			//{
			//	int l = stoi(tmp.substr(2, 2));
			//	sendtoUser = tmp.substr(4, l);
			//	CString temp = recvStr.Mid(l + 4, Len - l - 4);
			//	Command = ("51" + toTwoCharFromInt(int(user.size())) + user).c_str();
			//	Command += temp;
			//}


			break;
		}
		default: break;
		}

		break;
	}
	case FD_CLOSE:
	{
		//UpdateData(); // cua thay, ko hieu
		// tim vi tri socket 
		int post = -1;
		for (int i = 0; i < number_Socket; i++) {
			if (pSock[i].sockClient == wParam) {
				post = i;
				break;
			}
		}
		// ngat ket noi
		closesocket(wParam);
		if (post < 0) break;
		string user = pSock[post].name;
		// update pSock
		number_Socket--;
		pSock[post].sockClient = pSock[number_Socket].sockClient;
		pSock[post].name = pSock[number_Socket].name;
		// thong bao tren server
		msgBox += ("[" + user + "] has just logged out.\r\n").c_str();
		AppendTextToEditCtrl(m_msgBox, msgBox);

		break;
	}
	default: break;
	}

	return 0;
}

int CServerDlg::mSend(SOCKET sk, const char *buf, int len)
{
	/*m_BuffLen = sizeof(m_BuffLen) + len;
	memcpy(m_Buffer, &len, sizeof(m_BuffLen));
	memcpy(m_Buffer + sizeof(m_BuffLen), buf, len);
	return send(sk, m_Buffer, m_BuffLen, 0);*/

	int m_BuffLen = sizeof(m_BuffLen) + len;

	memcpy(m_Buffer, &len, sizeof(len));
	memcpy(m_Buffer + sizeof(len), buf, len);

	int sent = 0, sentSoFar;
	while (sent < m_BuffLen)
	{
		sentSoFar = send(sk, m_Buffer + sent, m_BuffLen - sent, 0);
		if (sentSoFar != SOCKET_ERROR)
			sent += sentSoFar;
	}

	return m_BuffLen;
}
int CServerDlg::mRecv(SOCKET sk, char *buf)
{
	/*recv(sk, (char*)&buffLength, sizeof(buffLength), 0);
	return recv(sk, buf, buffLength, 0);*/

	int received = 0, m_BuffLen, receivedSoFar;
	while (received < 4)
	{
		receivedSoFar = recv(sk, (char *)(&m_BuffLen + received), sizeof(m_BuffLen) - received, 0);
		if (receivedSoFar != SOCKET_ERROR)
			received += receivedSoFar;
	}

	received = 0;
	while (received < m_BuffLen)
	{
		receivedSoFar = recv(sk, buf + received, m_BuffLen - received, 0);
		if (receivedSoFar != SOCKET_ERROR)
			received += receivedSoFar;
	}

	return m_BuffLen;
}

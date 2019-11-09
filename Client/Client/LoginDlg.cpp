// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDlg.h"


// LoginDlg dialog

IMPLEMENT_DYNAMIC(LoginDlg, CDialogEx)

LoginDlg::LoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_DIALOG, pParent)
	, _username(_T(""))
	, _password(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

LoginDlg::~LoginDlg()
{
}


void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USERNAME, _username);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, _password);
}


BEGIN_MESSAGE_MAP(LoginDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET, SockMsg)
	ON_BN_CLICKED(IDC_BUTTON_SIGNUP, &LoginDlg::OnBnClickedButtonSignup)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &LoginDlg::OnBnClickedButtonLogin)
END_MESSAGE_MAP()


// LoginDlg message handlers


BOOL LoginDlg::OnInitDialog()
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

	CFont font;
	font.CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));                 // lpszFacename

	GetDlgItem(IDC_STATIC_USERNAME)->SetFont(&font);
	GetDlgItem(IDC_STATIC_PASSWORD)->SetFont(&font);
	GetDlgItem(IDC_BUTTON_SIGNUP)->SetFont(&font);
	GetDlgItem(IDC_BUTTON_LOGIN)->SetFont(&font);

	CFont fontUsername;
	fontUsername.CreateFont(
		20,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));                 // lpszFacename

	GetDlgItem(IDC_EDIT_USERNAME)->SetFont(&fontUsername);

	CFont fontPassword;
	fontPassword.CreateFont(
		18,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));                 // lpszFacename

	GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&fontPassword);


	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	UpdateData(FALSE);

	return TRUE;
}


void LoginDlg::OnPaint()
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


HCURSOR LoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT LoginDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		// Display the error and close the socket
		closesocket(wParam);
		AfxMessageBox(L"The server has closed the connection!");
		exit(0);
		UpdateData(FALSE);
		return 0;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
	{
		CString strResult;
		string tmp;
		if (mRecv(strResult) < 0)
			break;

		tmp = strResult[0];
		int flag1 = stoi(tmp);
		tmp = strResult[1];
		int flag2 = stoi(tmp);

		switch (flag1)
		{
			//sign up
		case 1:
		{
			switch (flag2) {
			case 0:
			{
				AfxMessageBox(L"Sign up successful!");
				_username = L"";
				_password = L"";
				CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
				tmp->SetFocus();
				tmp->SetSel(-1);
				break;
			}
			case 1:
			{
				AfxMessageBox(L"Sign up failed!");
				_username = L"";
				_password = L"";
				CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
				tmp->SetFocus();
				tmp->SetSel(-1);
				break;
			}
			case 2:
			{
				CString user = _T("");
				for (int i = 2; i < (int)strResult.GetLength(); i++)
					user += strResult[i];

				break;
			}
			break;
		}

			UpdateData(FALSE);
			break;
		}
		//log in, log out
		case 2:
		{
			switch (flag2) {
				//log in thanh cong, nhan danh sach online
			case 0:
			{
				number_User_Online = 0;
				int i = 2, cUser, lengthResult = strResult.GetLength();
				while (i < lengthResult) {
					string user = "";
					tmp = strResult[i];
					tmp += strResult[i + 1];
					cUser = stoi(tmp);
					i += 2;
					for (int j = 0; j < cUser; j++) {
						user += strResult[i];
						i++;
					}

					user_Online[number_User_Online++] = user;

				}

				_csm = new GroupChatDlg;
				CString name;
				GetDlgItemText(IDC_EDIT_USERNAME, name);
				_csm->_name = name;

				ShowWindow(SW_HIDE);
				if (_csm->DoModal() == IDOK)
					_csm->ShowWindow(SW_SHOW);
				else
				{
					delete[]_csm;
					ShowWindow(SW_SHOW);
					WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
					UpdateData(FALSE);
					_username = L"";
					_password = L"";
					CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
					tmp->SetFocus();
					tmp->SetSel(-1);
				}
				break;
			}
			case 1:
			{
				AfxMessageBox(L"Login failed!");
				_username = L"";
				_password = L"";
				CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
				tmp->SetFocus();
				tmp->SetSel(-1);
				break;
			}
			case 2:
			{
				AfxMessageBox(L"Login from another device!");
				_username = L"";
				_password = L"";
				CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
				tmp->SetFocus();
				tmp->SetSel(-1);
				break;
			}
			break;
		}

			UpdateData(FALSE);
			break;
		}

		}
		break;
	}

	case FD_CLOSE:
	{
		closesocket(sClient);
		AfxMessageBox(L"The server has closed the connection!");
		exit(0);
		UpdateData(FALSE);
		break;
	}

	}
	return 0;
}


void LoginDlg::OnBnClickedButtonSignup()
{
	GetDlgItemText(IDC_EDIT_USERNAME, _username);
	GetDlgItemText(IDC_EDIT_PASSWORD, _password);
	if (_username == "")
	{
		AfxMessageBox(L"Please enter user name!");
		CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
		tmp->SetWindowText(L"");
		tmp->SetFocus();
		tmp->SetSel(-1);
		return;
	}

	if (_password == "")
	{
		AfxMessageBox(L"Please enter password!");
		CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_PASSWORD);
		tmp->SetWindowText(L"");
		tmp->SetFocus();
		tmp->SetSel(-1);
		return;
	}

	int userLength = _username.GetLength(), passLength = _password.GetLength();
	CString Command;

	string s1;
	s1 = char(userLength / 10 + 48);
	s1 += char(userLength % 10 + 48);
	string s2;
	s2 = char(passLength / 10 + 48);
	s2 += char(passLength % 10 + 48);

	CString cs1(s1.c_str());
	CString cs2(s2.c_str());

	Command = _T("10") + cs1 + _username + cs2 + _password;

	mSend(Command);

	CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_PASSWORD);
	tmp->SetWindowText(L"");
	tmp = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
	tmp->SetWindowText(L"");
	tmp->SetFocus();
	tmp->SetSel(-1);
}

void LoginDlg::OnBnClickedButtonLogin()
{
	GetDlgItemText(IDC_EDIT_USERNAME, _username);
	GetDlgItemText(IDC_EDIT_PASSWORD, _password);
	if (_username == "")
	{
		AfxMessageBox(L"Please enter user name!");
		CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
		tmp->SetWindowText(L"");
		tmp->SetFocus();
		tmp->SetSel(-1);
		return;
	}

	if (_password == "")
	{
		AfxMessageBox(L"Please enter password!");
		CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_PASSWORD);
		tmp->SetWindowText(L"");
		tmp->SetFocus();
		tmp->SetSel(-1);
		return;
	}

	int userLength = _username.GetLength(), passLength = _password.GetLength();
	CString Command;

	string s1;
	s1 = char(userLength / 10 + 48);
	s1 += char(userLength % 10 + 48);
	string s2;
	s2 = char(passLength / 10 + 48);
	s2 += char(passLength % 10 + 48);

	CString cs1(s1.c_str());
	CString cs2(s2.c_str());

	Command = _T("20") + cs1 + _username + cs2 + _password;

	mSend(Command);
}


BOOL LoginDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		return 1;

	return CDialogEx::PreTranslateMessage(pMsg);
}

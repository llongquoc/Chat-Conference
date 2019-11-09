// GroupChatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "GroupChatDlg.h"
#include "afxdialogex.h"


// GroupChatDlg dialog

IMPLEMENT_DYNAMIC(GroupChatDlg, CDialogEx)

GroupChatDlg::GroupChatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GROUPCHAT_DIALOG, pParent)
	, _message(_T(""))
	, _onlineUsers(_T(""))
	, _inputPrivate(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

GroupChatDlg::~GroupChatDlg()
{
	_running = false;
}

void GroupChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT, _message);
	DDX_Text(pDX, IDC_EDIT_ONLINEUSERS, _onlineUsers);
	DDX_Text(pDX, IDC_EDIT_INPUTPRIVATE, _inputPrivate);
	DDX_Control(pDX, IDC_EDIT_MESSENGER, _output);
}


BEGIN_MESSAGE_MAP(GroupChatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET, SockMsg)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &GroupChatDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &GroupChatDlg::OnBnClickedButtonLogout)
	ON_BN_CLICKED(IDC_BUTTON_PRIVATECHAT, &GroupChatDlg::OnBnClickedButtonPrivatechat)
END_MESSAGE_MAP()

bool GroupChatDlg::runConnect(HWND m_hWnd)
{
	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	return _running;
}

DWORD WINAPI ThreadConnect(LPVOID lpParam)
{
	while (GroupChatDlg::runConnect( *(HWND *) lpParam));
	return 0;
}

BOOL GroupChatDlg::OnInitDialog()
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

	//CFont font;
	//font.CreateFont(
	//	12,                        // nHeight
	//	0,                         // nWidth
	//	0,                         // nEscapement
	//	0,                         // nOrientation
	//	FW_NORMAL,                 // nWeight
	//	FALSE,                     // bItalic
	//	FALSE,                     // bUnderline
	//	0,                         // cStrikeOut
	//	ANSI_CHARSET,              // nCharSet
	//	OUT_DEFAULT_PRECIS,        // nOutPrecision
	//	CLIP_DEFAULT_PRECIS,       // nClipPrecision
	//	DEFAULT_QUALITY,           // nQuality
	//	DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	//	_T("Arial"));                 // lpszFacename

	//GetDlgItem(IDC_STATIC_PUBLICCHAT)->SetFont(&font);
	//GetDlgItem(IDC_STATIC_ONLINEUSERS)->SetFont(&font);
	//GetDlgItem(IDC_STATIC_PRIVATECHAT)->SetFont(&font);
	//GetDlgItem(IDC_BUTTON_SEND)->SetFont(&font);
	//GetDlgItem(IDC_BUTTON_LOGOUT)->SetFont(&font);
	//GetDlgItem(IDC_BUTTON_PRIVATECHAT)->SetFont(&font);

	CFont fontEdit;
	fontEdit.CreateFont(
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

	GetDlgItem(IDC_EDIT_MESSENGER)->SetFont(&fontEdit);
	GetDlgItem(IDC_EDIT_INPUT)->SetFont(&fontEdit);
	//GetDlgItem(IDC_EDIT_ONLINEUSERS)->SetFont(&fontEdit);
	GetDlgItem(IDC_EDIT_INPUTPRIVATE)->SetFont(&fontEdit);

	CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
	tmp->SetFocus();
	tmp->SetSel(-1);

	_onlineUsers += _name + _T(" (me)\r\n");
	for (int i = 0; i < number_User_Online; i++)
	{
		CString s(user_Online[i].c_str());
		_onlineUsers += s + _T("\r\n");
	}

	CMenu * psysmenu = GetSystemMenu(FALSE);
	psysmenu->EnableMenuItem(SC_CLOSE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);

	HANDLE hThreadArray;
	DWORD dwThreadIdArray;

	hThreadArray = CreateThread(NULL, 0, ThreadConnect, (LPVOID)&m_hWnd, 0, &dwThreadIdArray);
	
	UpdateData(FALSE);

	return TRUE;
}


void GroupChatDlg::OnPaint()
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


HCURSOR GroupChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	GroupChatDlg::runPrivateDlg();
	return 0;
}

LRESULT GroupChatDlg::SockMsg(WPARAM wParam, LPARAM lParam)
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
		//log in, log out
		case 2:
		{
			switch (flag2) {
				//log in thanh cong, nhan danh sach online
			case 2:
			{
				AfxMessageBox(L"Login from another device!");

				while (_numberPrivate > 0) {
					_csm[0]->Cancel();
					for (int i = 0; i < (int)5E7; i++); // tao delay
				}

				OnCancel();
			}
			case 3:
			{
				string user = ("");
				for (int i = 2; i < (int)strResult.GetLength(); i++)
					user += strResult[i];

				user_Online[number_User_Online++] = user;
				
				CString s(user.c_str());
				_onlineUsers += s + _T("\r\n");

				break;
			}
			case 4:
			{
				string user = ("");
				for (int i = 2; i < (int)strResult.GetLength(); i++)
					user += strResult[i];
				CString u(user.c_str());
				for (int i = 0; i < _numberPrivate; i++)
					if (_csm[i]->_partner == u)
					{
						_csm[i]->Cancel();
						break;
					}

				for (int i = 0; i < number_User_Online; i++)
					if (user_Online[i] == user) {
						user_Online[i] = user_Online[--number_User_Online];
						break;
					}

				_onlineUsers = _T("");
				_onlineUsers += _name + _T(" (me)\r\n");
				for (int i = 0; i < number_User_Online; i++)
				{
					CString s(user_Online[i].c_str());
					_onlineUsers += s + _T("\r\n");
				}

				break;
			}
			break;
		}

			UpdateData(FALSE);
			break;
		}
		//group chat
		case 3:
		{
			tmp = strResult[2];
			tmp += strResult[3];
			int lengthUser = stoi(tmp);

			CString user = _T(""), message = _T("");

			for (int i = 4; i < 4 + lengthUser; i++)
				user += strResult[i];

			for (int i = 4 + lengthUser; i < (int)strResult.GetLength(); i++)
				message += strResult[i];

			CString output;
			output += user + _T(":\r\n") + message + _T("\r\n\r\n");
			AppendTextToEditCtrl(_output, output);

			UpdateData(FALSE);
			break;
		}
		case 4:
		{
			tmp = strResult[2];
			tmp += strResult[3];
			int lengthUser = stoi(tmp);

			CString user = _T(""), message = _T("");

			for (int i = 4; i < 4 + lengthUser; i++)
				user += strResult[i];

			for (int i = 4 + lengthUser; i < (int)strResult.GetLength(); i++)
				message += strResult[i];

			bool check = false;

			for (int i = 0; i < _numberPrivate; i++) {
				if (_csm[i]->_partner == user) {
					CString outputPrivate;
					outputPrivate += user + _T(":\r\n") + message + _T("\r\n\r\n");
					AppendTextToEditCtrl(_csm[_numberPrivate - 1]->_outputPrivate, outputPrivate);
					check = true;
					break;
				}
			}

			if (!check) {
				_numberPrivate++;

				_csm[_numberPrivate - 1] = new PrivateChatDlg;
				_csm[_numberPrivate - 1]->_name = _name;
				_csm[_numberPrivate - 1]->_partner = user;
				
				hThreadArray[_numberPrivate - 1] = CreateThread(NULL, 0, MyThreadFunction, NULL, 0,	&dwThreadIdArray[_numberPrivate - 1]);

				for (int i = 0; i < (int)5E7; i++); // tao delay
				
				CString outputPrivate;
				outputPrivate += user + _T(":\r\n") + message + _T("\r\n\r\n");
				AppendTextToEditCtrl(_csm[_numberPrivate - 1]->_outputPrivate, outputPrivate);
			}
			
			break;
		}
		case 5:
		{
			switch (flag2)
			{
				//50_xxSTR1 _yyyy_STR2: STR1 là user gửi, yyyy là số gói tin, STR2 là tên file
			case 0:
			{
				tmp = strResult[2];
				tmp += strResult[3];
				int lengthUser = stoi(tmp);

				CString user = _T(""), fileName = _T("");

				for (int i = 4; i < 4 + lengthUser; i++)
					user += strResult[i];

				tmp = "";
				for (int i = 4 + lengthUser; i < 11 + lengthUser; i++)
					tmp += strResult[i];

				int numberChar = stoi(tmp);

				for (int i = 11 + lengthUser; i < (int)strResult.GetLength(); i++)
					fileName += strResult[i];

				//tao dlg private

				bool check = false;

				for (int i = 0; i < _numberPrivate; i++) {
					if (_csm[i]->_partner == user) {
						check = true;
						break;
					}
				}

				if (!check) {
					_numberPrivate++;

					_csm[_numberPrivate - 1] = new PrivateChatDlg;
					_csm[_numberPrivate - 1]->_name = _name;
					_csm[_numberPrivate - 1]->_partner = user;

					hThreadArray[_numberPrivate - 1] = CreateThread(NULL, 0, MyThreadFunction, NULL, 0, &dwThreadIdArray[_numberPrivate - 1]);
				}

				// 51_xxSTR_yyyy_STR

				int lenFileNameReceive = fileName.GetLength();

				string fileNameTmp((CW2A)fileName.GetString());

				char * cFileName = new char[lenFileNameReceive + 1];

				for (int j = 0; j < lenFileNameReceive; j++)
					cFileName[j] = fileNameTmp[j];

				cFileName[lenFileNameReceive] = '\0';

				//ghi vao file
				FILE * f = fopen(cFileName, "wb");

				char * packet = new char[MAX_BUFFER_SIZE];
				while (numberChar > 0) {
					memset(packet, 0, MAX_BUFFER_SIZE);
					int buffRecv = mRecv(packet);
					numberChar -= buffRecv;
					fwrite(packet, sizeof(char), buffRecv, f);
				}

				fclose(f);

				for (int i = 0; i < _numberPrivate; i++) 
					if (_csm[i]->_partner == user) {
						//gui tin nhan ve server
						CString Command = _T("52") + _csm[i]->_partner;
						mSend(Command);

						for (int i = 0; i < (int)5E7; i++); // tao delay

						CString outputPrivate;
						outputPrivate += _T("Received file: ") + fileName + _T("\r\n\r\n");
						AppendTextToEditCtrl(_csm[i]->_outputPrivate, outputPrivate);
						break;
					}

				break;
			}
			//51_xxSTR1_yyyy_STR2: STR1 là user gửi, yyyy là thứ tự gói tin (đánh số từ 1), STR2 là nội dung
			//case 1:
			//{


			//	break;
			//}
			//52: gửi file thành công (bên kia đã nhận đủ số gói tin)
			case 2:
			{
				CString user = _T("");
				for (int i = 2; i < (int)strResult.GetLength(); i++)
					user += strResult[i];

				for (int i = 0; i < _numberPrivate; i++)
					if (_csm[i]->_partner == user) {
						CString outputPrivate;
						outputPrivate += _T("Sent file: ") +
							_csm[i]->_fileNameSend + _T("\r\n\r\n");
						AppendTextToEditCtrl(_csm[i]->_outputPrivate, outputPrivate);
						break;
					}
			}
			break;
			}
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


void GroupChatDlg::OnBnClickedButtonSend()
{
	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);

	GetDlgItemText(IDC_EDIT_INPUT, _message);

	if (_message == _T(""))
	{
		CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
		tmp->SetFocus();
		tmp->SetSel(-1);
		return;
	}

	CString Command = _T("30") + _message;

	CString output;
	output += _name + _T(" (me):\r\n") + _message + _T("\r\n\r\n");
	AppendTextToEditCtrl(_output, output);

	_message = L"";
	CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
	tmp->SetFocus();
	tmp->SetSel(-1);

	mSend(Command);

	UpdateData(FALSE);
}


void GroupChatDlg::OnBnClickedButtonLogout()
{
	CString Command = _T("21");
	mSend(Command);

	while (_numberPrivate > 0) {
		_csm[0]->Cancel();
		for (int i = 0; i < (int)5E7; i++); // tao delay
	}

	OnCancel();
}


BOOL GroupChatDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		return 1;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void GroupChatDlg::OnBnClickedButtonPrivatechat()
{
	GetDlgItemText(IDC_EDIT_INPUTPRIVATE, _inputPrivate);
	string userNamePrivate(CW2A(_inputPrivate.GetString()));
	bool checkOnl = false;
	for (int i = 0; i < number_User_Online; i++)
		if (user_Online[i] == userNamePrivate) {
			checkOnl = true;
			break;
		}

	if (_inputPrivate == _name)
	{
		AfxMessageBox(L"You can not chat to yourself!");
		CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUTPRIVATE);
		_inputPrivate = L"";
		tmp->SetWindowTextW(L"");
		tmp->SetFocus();
		tmp->SetSel(-1);
		return;
	}

	if (!checkOnl) {
		AfxMessageBox(L"User is offline or does not exist!");
		CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUTPRIVATE);
		_inputPrivate = L"";
		tmp->SetWindowTextW(L"");
		tmp->SetFocus();
		tmp->SetSel(-1);
		return;
	}

	for (int i = 0; i < _numberPrivate; i++)
		if (_inputPrivate == _csm[i]->_partner)
		{
			AfxMessageBox(L"Already chat to " + _inputPrivate + L"!");
			_inputPrivate = L"";
			CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUTPRIVATE);
			tmp->SetWindowTextW(L"");
			tmp->SetFocus();
			tmp->SetSel(-1);
			return;
		}

	CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUTPRIVATE);
	tmp->SetWindowTextW(L"");
	tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
	tmp->SetFocus();
	tmp->SetSel(-1);
	_numberPrivate++;
	_csm[_numberPrivate - 1] = new PrivateChatDlg;
	_csm[_numberPrivate - 1]->_name = _name;
	_csm[_numberPrivate - 1]->_partner = _inputPrivate;
	_inputPrivate = L"";
	hThreadArray[_numberPrivate - 1] = CreateThread(NULL, 0, MyThreadFunction, NULL, 0, &dwThreadIdArray[_numberPrivate - 1]);
}

void  GroupChatDlg::runPrivateDlg()
{
	PrivateChatDlg * DlgCurrent = _csm[_numberPrivate - 1];
	if (DlgCurrent->DoModal() == IDOK)
		DlgCurrent->ShowWindow(SW_SHOW);
	else {
		
		for (int i = 0; i < _numberPrivate; i++) {
			if (_csm[i] == DlgCurrent) {
				delete[] _csm[i];
				_csm[i] = _csm[_numberPrivate - 1];
				hThreadArray[i] = hThreadArray[_numberPrivate - 1];
				dwThreadIdArray[i] = dwThreadIdArray[_numberPrivate - 1];
				break;
			}
		}
		_numberPrivate--;
	}
}
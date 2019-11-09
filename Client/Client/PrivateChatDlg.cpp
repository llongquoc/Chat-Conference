// PrivateChatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "PrivateChatDlg.h"
#include "afxdialogex.h"


// PrivateChatDlg dialog

IMPLEMENT_DYNAMIC(PrivateChatDlg, CDialogEx)

PrivateChatDlg::PrivateChatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PRIVATECHAT_DIALOG, pParent)
	, _partner(_T(""))
	, _messagePrivate(_T(""))
	, _fileName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

PrivateChatDlg::~PrivateChatDlg()
{
}

void PrivateChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_PARTNER, _partner);
	DDX_Text(pDX, IDC_EDIT_INPUT, _messagePrivate);
	DDX_Text(pDX, IDC_EDIT_INPUTFILE, _fileName);
	DDX_Control(pDX, IDC_EDIT_MESSENGER, _outputPrivate);
}


BEGIN_MESSAGE_MAP(PrivateChatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &PrivateChatDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_SENDFILE, &PrivateChatDlg::OnBnClickedButtonSendfile)
	ON_BN_CLICKED(IDC_BUTTON1, &PrivateChatDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


BOOL PrivateChatDlg::OnInitDialog()
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

	GetDlgItem(IDC_STATIC_PARTNER)->SetFont(&font);
	GetDlgItem(IDC_STATIC_FILE)->SetFont(&font);
	GetDlgItem(IDC_BUTTON_SEND)->SetFont(&font);
	//GetDlgItem(IDC_BUTTON_SENDFILE)->SetFont(&font);

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

	CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
	tmp->SetFocus();
	tmp->SetSel(-1);

	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	UpdateData(FALSE);


	return TRUE;
}


void PrivateChatDlg::OnPaint()
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


HCURSOR PrivateChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void PrivateChatDlg::OnBnClickedButtonSend()
{
	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);

	GetDlgItemText(IDC_EDIT_INPUT, _messagePrivate);

	if (_messagePrivate == _T(""))
	{
		CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
		tmp->SetFocus();
		tmp->SetSel(-1);
		return;
	}

	CString outputPrivate;
	outputPrivate += _name + _T(" (me):\r\n") + _messagePrivate + _T("\r\n\r\n");
	AppendTextToEditCtrl(_outputPrivate, outputPrivate);


	int userLength = _partner.GetLength();

	CString Command = _T("40");
	Command += char(userLength / 10 + 48);
	Command += char(userLength % 10 + 48);
	Command += _partner + _messagePrivate;

	mSend(Command);

	_messagePrivate = L"";
	CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
	tmp->SetFocus();
	tmp->SetSel(-1);
	UpdateData(FALSE);
}


void PrivateChatDlg::OnBnClickedButtonSendfile()
{
	GetDlgItemText(IDC_EDIT_INPUTFILE, _fileName);

	_fileNameSend = _fileName;
	
	char * fileName = ConvertToChar(_fileName);

	for (int i = _fileName.GetLength() - 1; i >= 0; i--)
		if (_fileName[i] == _T('\\')) {
			_fileName.Delete(0, i + 1);
			break;
		}

	FILE * f = fopen(fileName, "rb");

	if (!f) {
		AfxMessageBox(L"File does not exist!");
		return;
	}

	char * allPacket = new char[MAX_FILE_SIZE + 1];

	int numberOfChar = fread(allPacket, sizeof(char), MAX_FILE_SIZE + 1, f);

	if (numberOfChar > MAX_FILE_SIZE) {
		AfxMessageBox(L"File can not be bigger than 5MB!");
		return;
	}

	fclose(f);

	delete[]fileName;

	int lenUserReceive = _partner.GetLength();
	string sLenUserReceive = to_string(lenUserReceive), sNumberOfChar = to_string(numberOfChar);
	while (sLenUserReceive.size() < 2)
		sLenUserReceive = "0" + sLenUserReceive;
	while (sNumberOfChar.size() < 7)
		sNumberOfChar = "0" + sNumberOfChar;

	CString csLenUserReceive(sLenUserReceive.c_str()), csNumberOfChar(sNumberOfChar.c_str());

	CString Command = _T("50") + csLenUserReceive + _partner + csNumberOfChar + _fileName;
	mSend(Command);
	for (int i = 0; i < (int)1E5; i++); // tao delay

	//AfxMessageBox(Command);

	int i = 0;

	while (i < numberOfChar) {

		char * tmpPacket = new char[MAX_BUFFER_SIZE];

		memset(tmpPacket, 0, MAX_BUFFER_SIZE);

		int len = min(MAX_BUFFER_SIZE, numberOfChar - i);

		memcpy(tmpPacket, allPacket + i, len);

		mSend(tmpPacket, len);

		i += len;

		delete[]tmpPacket;
	}

	delete[]allPacket;

	_fileName = L"";
	UpdateData(FALSE);
	CEdit *tmp = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
	tmp->SetFocus();
	tmp->SetSel(-1);
}


BOOL PrivateChatDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		return 1;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void PrivateChatDlg::OnBnClickedButton1()
{
	CFileDialog dialog(true);
	if (dialog.DoModal() == IDOK)
	{
		_fileName = dialog.GetPathName();
		UpdateData(FALSE);
	}
}

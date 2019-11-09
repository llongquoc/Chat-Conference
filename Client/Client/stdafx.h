
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#include <string>
#include <thread>

using namespace std;

#define MAX_CLIENTS_ONLINE 200
#define PORT 25000
#define WM_SOCKET WM_USER + 2
#define MAX_BUFFER_SIZE (8 * 1024)
#define MAX_BUFF_SIZE (4 * 1024)

SOCKET sClient;
sockaddr_in servAdd;
string user_Online[MAX_CLIENTS_ONLINE];
int number_User_Online = 0;
HMODULE hModule = ::GetModuleHandle(NULL);
HANDLE hThreadArray[MAX_CLIENTS_ONLINE];
DWORD dwThreadIdArray[MAX_CLIENTS_ONLINE];

void mSend(CString Command);
int mRecv(CString &Command);
int buffLength;

void mSend(CString Command)
{
	Command = _T("**********") + Command;
	int Len = Command.GetLength();
	Len += Len;
	PBYTE sendBuff = new BYTE[Len + 2]{};
	//memset(sendBuff, 0, Len);
	memcpy(sendBuff, (PBYTE)(LPCTSTR)Command, Len);
	send(sClient, (char*)&Len, sizeof(Len), 0);
	send(sClient, (char*)sendBuff, Len, 0);
	delete []sendBuff;
}


int mRecv(CString &Command)
{
	recv(sClient, (char*)&buffLength, sizeof(int), 0);
	PBYTE buffer = new BYTE[buffLength + 2]{};
	//memset(buffer, 0, buffLength + 2);
	recv(sClient, (char*)buffer, buffLength, 0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;
	delete[]buffer;
	if (Command.GetLength() == 0 || Command[0] != _T('*'))
		return -1;

	while (Command[0] == _T('*'))
		Command.Delete(0, 1);

	return 0;
}

char * ConvertToChar(const CString &s)
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize + 1];
	memset(pAnsiString, 0, nSize + 1);
	wcstombs(pAnsiString, s, nSize + 1);
	return pAnsiString;
}

void AppendTextToEditCtrl(CEdit& edit, LPCTSTR pszText)
{
	// get the initial text length
	int nLength = edit.GetWindowTextLength();
	// put the selection at the end of text
	edit.SetSel(nLength, nLength);
	// replace the selection
	edit.ReplaceSel(pszText);
}

char m_Buffer[MAX_BUFFER_SIZE];

int mSend(const char *buf, int len)
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
		sentSoFar = send(sClient, m_Buffer + sent, m_BuffLen - sent, 0);
		if (sentSoFar != SOCKET_ERROR)
			sent += sentSoFar;
	}

	return m_BuffLen;
}

int mRecv(char *buf)
{
	int received = 0, m_BuffLen, receivedSoFar;
	while (received < 4)
	{
		receivedSoFar = recv(sClient, (char *)(&m_BuffLen + received), sizeof(m_BuffLen) - received, 0);
		if (receivedSoFar != SOCKET_ERROR)
			received += receivedSoFar;
	}

	received = 0;
	while (received < m_BuffLen)
	{
		receivedSoFar = recv(sClient, buf + received, m_BuffLen - received, 0);
		if (receivedSoFar != SOCKET_ERROR)
			received += receivedSoFar;
	}

	return m_BuffLen;
}
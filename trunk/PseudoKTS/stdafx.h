
// stdafx.h: �������� ���� ��� ���������� ����������� ��������� ������
//��� ���������� ������ ��������, ����� ������������,
// �� ����� ����������

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ��������� ����� ������������ ���������� �� ���������� Windows
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ��������� ������������ CString ����� ������

// ��������� ������� ������� ��������� ����� � ����� ������������ �������������� MFC
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // �������� � ����������� ���������� MFC
#include <afxext.h>         // ���������� MFC

#include <afxdisp.h>        // ������ ������������� MFC

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // ��������� MFC ��� ������� ��������� ���������� Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // ��������� MFC ��� ������� ��������� ���������� Windows
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // ��������� MFC ��� ���� � ������� ����������

//HHH
#include <math.h>
#include <vector>
#include <map>

#define _USE_SQL
#include <afxdb.h>
#include <Odbcinst.h>

#define _CRTDBG_MAP_ALLOC

#include <common_identifiers.h>
#include "identifiers.h"

#include <hstring.h>

#define PTR_DB ((HWinAppKTS *) AfxGetApp ())->GetPtrDB ()

#define ENUM_PREFIXINCREMENT(enumInstance) inline enumInstance& operator++ (enumInstance &id) { return id = static_cast<enumInstance>(static_cast<int>(id) + 1); }
#define ENUM_POSTFIXINCREMENT(enumInstance) inline enumInstance operator++ (enumInstance &id, int) { enumInstance prev = id; id = static_cast<enumInstance>(static_cast<int>(id) + 1); return prev; }
#define ENUM_PLUS(enumInstance) inline enumInstance operator+ (enumInstance &id, int inc) { return id = (enumInstance) (id + inc); }

#include <hwinappkts.h>

#include <HFileInfo.h>

#include <hthreadfilelog.h>

//���������� ������ � ������ ������������� ���������� � �� �������� � ��������
#include <HDateTimeInterval.h>

#define PAR_ENERG_VER_6

//HHH

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



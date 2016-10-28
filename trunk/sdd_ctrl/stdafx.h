// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

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
//#include <afxdao.h>        // MFC DAO object's

#include <direct.h>         //Directory control
#include <shlwapi.h>        //???
#include <math.h>           //Math function

#include <afxmt.h>			//Поддержка event's

#include <winsock.h>

#include <vector>
#include <map>

#define ENUM_PREFIXINCREMENT(enumInstance) inline enumInstance& operator++ (enumInstance &id) { return id = static_cast<enumInstance>(static_cast<int>(id) + 1); }
#define ENUM_POSTFIXINCREMENT(enumInstance) inline enumInstance operator++ (enumInstance &id, int) { enumInstance prev = id; id = static_cast<enumInstance>(static_cast<int>(id) + 1); return prev; }
#define ENUM_PLUS(enumInstance) inline enumInstance operator+ (enumInstance &id, int inc) { return id = (enumInstance) (id + inc); }

#define PTR_DB ((HWinAppKTS *) AfxGetApp ())->GetPtrDB ()

#include <hstring.h>
//#include <HA2KW.h>

#include <HFileInfo.h>

#include <common_identifiers.h>
#include <identifiers.h>

//Глобальный объект с картой идентификатов интервалов и их значений в СЕКУНДАХ
#include <HDateTimeInterval.h>

#include <hthreadfilelog.h>

#include "hwinapp.h"

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afx.h>

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>

#include <mscoree.h>

#include <time.h>
#include <math.h>


#define CLOSE_HANDLE(h) { \
                            if (h == NULL) \
                                ; /*return 0;*/ \
                            else { \
                                if (CloseHandle (h)) { \
                                    h = NULL; \
                                    /*return 0;*/ \
                                } \
                                else \
                                    ; /*return GetLastError ();*/  \
                            } \
                        }

#define TIMEOUT_FORCED_TERMINATE 6 /*в секундах*/
#define DELETE_THREAD(ptr,type) { ((type *) ptr)->Delete ();  delete ((type *) ptr); }

#define PAR_ENERG
#define PAR_ENERG_VER_5

#define ENERG_CHANNEL               "chnl"
#define ENERG_GROUP                 "grp"
//#define TORNADO_SIGNAL              "sgnl"

#define ENUM_PREFIXINCREMENT(enumInstance) inline enumInstance& operator++ (enumInstance &id) { return id = static_cast<enumInstance>(static_cast<int>(id) + 1); }
#define ENUM_POSTFIXINCREMENT(enumInstance) inline enumInstance operator++ (enumInstance &id, int) { enumInstance prev = id; id = static_cast<enumInstance>(static_cast<int>(id) + 1); return prev; }
#define ENUM_PLUS(enumInstance) inline enumInstance operator+ (enumInstance &id, int inc) { return id = (enumInstance) (id + inc); }

#include <hstring.h>
#include <HA2KW.h>

#include <common_identifiers.h>

#include <HFileInfo.h>			//Класс для получения информации о файле

//#include "HError.h"


#include <hwinapp.h>

#include <hthreadfilelog.h>		//Для подключения возможности открыть ПОТОК с сообщениями в ЛОГ-файл

#include <HFileIni.h>			//Работа с инициализационным файлом

#ifdef _USE_SQL
//#include <sql.h>
//#include <sqlext.h>
#include <afxdb.h>
#include <Odbcinst.h>
#endif

//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
//#include <atlstr.h>

// TODO: reference additional headers your program requires here

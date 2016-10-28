// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

#include "targetver.h"

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxtempl.h>

#include <Winsock2.h>

#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <tchar.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // поддержка MFC дл€ типовых элементов управлени€ Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // поддержка MFC дл€ типовых элементов управлени€ Windows
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <vector>

// TODO: ”становите здесь ссылки на дополнительные заголовки, требующиес€ дл€ программы

#define ENUM_PREFIXINCREMENT(enumInstance) inline enumInstance& operator++ (enumInstance &id) { return id = static_cast<enumInstance>(static_cast<int>(id) + 1); }
#define ENUM_POSTFIXINCREMENT(enumInstance) inline enumInstance operator++ (enumInstance &id, int) { enumInstance prev = id; id = static_cast<enumInstance>(static_cast<int>(id) + 1); return prev; }
#define ENUM_PLUS(enumInstance) inline enumInstance operator+ (enumInstance &id, int inc) { return id = (enumInstance) (id + inc); }

#include <common_identifiers.h>
#include "identifiers.h"

#include <hstring.h>
#include <HA2KW.h>

#include <HFileInfo.h>

#include <hwinapp.h>
#include <hthreadfilelog.h>
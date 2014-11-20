#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

//#include "resource.h"		// main symbols

#include "HThreadUndoClear.h"

// CUserKTSApp:
// See UserKTS.cpp for the implementation of this class
//

class CUserKTSApp : public HWinAppKTS {
public:
	CUserKTSApp ();
	~CUserKTSApp ();

    virtual int SetOption ();

    HThreadUndoClear *GetPtrThreadUndoClear (void) { return m_ptrThreadUndoClear; }
    CTime GetUndoClearTimeRun (void) { return CTime (*(SYSTEMTIME *) m_ptrThreadUndoClear->GetDataOfId (HThreadUndoClear::TIME_RUN)); }
    HString GetUndoClearPath (void) { return m_ptrThreadUndoClear->stringData (m_ptrThreadUndoClear->GetDataOfId (HThreadUndoClear::STRING_DIRECTORYUNDO)); }

// Overrides
public:
	virtual BOOL InitInstance();

private:
    HThreadUndoClear *m_ptrThreadUndoClear;

    afx_msg void OperationUndoClear (WPARAM, LPARAM);
    afx_msg void ExitUndoClear (WPARAM, LPARAM);

    DECLARE_MESSAGE_MAP ()
};

#define PTR_THREAD_UNDOCLEAR ((CUserKTSApp *) AfxGetApp ())->GetPtrThreadUndoClear ()
#define UNDOCLEAR_TIMERUN ((CUserKTSApp *) AfxGetApp ())->GetUndoClearTimeRun ()
#define UNDOCLEAR_PATH ((CUserKTSApp *) AfxGetApp ())->GetUndoClearPath ()
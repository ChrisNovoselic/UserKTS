// UserKTS.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "UserKTS.h"
#include "UserKTSDlg.h"

EXT_DECLARE_HTHREAD_FILELOG

// CUserKTSApp

BEGIN_MESSAGE_MAP(CUserKTSApp, HWinAppKTS)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	ON_THREAD_MESSAGE (WM_USER_THREADUNDOCLEAR_RUN, &OperationUndoClear)
    ON_THREAD_MESSAGE (WM_USER_EXIT_UNDO_CLEAR, &ExitUndoClear)
END_MESSAGE_MAP()

// The one and only CUserKTSApp object
CUserKTSApp theApp;

// CUserKTSApp construction

#define lenof(x) (sizeof (x) / sizeof ((x) [0]))

//CUserKTSApp::CUserKTSApp () : m_ptrThreadFileLog (0x0)
CUserKTSApp::CUserKTSApp () {
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
    m_ptrThreadUndoClear = DEBUG_NEW HThreadUndoClear ();
}

CUserKTSApp::~CUserKTSApp (void) {
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
    DELETE_HTHREAD (m_ptrThreadUndoClear, HThreadUndoClear);
}

// CUserKTSApp initialization
BOOL CUserKTSApp::InitInstance (void) {
    BOOL iRes = HWinAppKTS::InitInstance ();

    if (iRes) {
        if (SetOption () == 0) {
            CUserKTSDlg *dlg = NULL; 
		    dlg = DEBUG_NEW CUserKTSDlg;
		    dlg->PreCreate (CRect (0, 0, 406, 210), HDLG_MODALES);
            m_pMainWnd = dlg;

            /*if (iRes) {*/
		        m_ptrMenuTray = DEBUG_NEW CMenu ();
		        iRes = m_ptrMenuTray->CreatePopupMenu ();

		        if (iRes) {
			        m_ptrMenuTray->AppendMenu (MF_BYPOSITION | MF_STRING, IDM_RESTORE, _T ("¬ÓÒÒÚ‡ÌÓ‚ËÚ¸"));
			        m_ptrMenuTray->AppendMenu(MF_SEPARATOR);
			        m_ptrMenuTray->AppendMenu (MF_BYPOSITION | MF_STRING, IDM_EXIT, _T ("«‡Í˚Ú¸"));
		        }
		        else
			        ;
	        /*}
	        else ;*/

            //dlg.ManagerIconTray (NIM_ADD);

            //CString strLog;
            //strLog.Format (_T ("—Í˚ÎË √À¿¬ÕÓÂ ÓÍÌÓ ‚ 'SystemTray'..."));
            //FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2);
            //       
            //INT_PTR nResponse = dlg.ShowWindow (SW_SHOWNORMAL);
            INT_PTR nResponse = dlg->DoModal ();

            if (nResponse == IDOK) {
	            // TODO: Place code here to handle when the dialog is
	            // dismissed with OK
            }
            else
                if (nResponse == IDCANCEL) {
	                // TODO: Place code here to handle when the dialog is
	                // dismissed with Cancel
                }
                else
                    ;

		    delete dlg;
        }
        else
            ; //”ÒÚ‡ÌÓ‚Í‡ Ô‡‡ÏÂÚÓ‚ - Œÿ»¡ ‡
    }
    else
        ;

    return iRes;
}

int CUserKTSApp::SetOption () {
    int iRes = false;
    FILELOG_VARIABLE
    
    strLog = "¬ıÓ‰  ‚ CUserKTSApp::SetOption";
    //FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), USER_KTS_APP)
    
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HCOMMON)
    /*{ \
        ptrMsgFileLog = DEBUG_NEW MESSAGE_FILELOG; \
        ptrMsgFileLog->mes = HString (MB_HSTRING (strLog)); \
        memset (&ptrMsgFileLog->stmOccurred, 0x0, sizeof (SYSTEMTIME)); \
        CTime::GetCurrentTime ().GetAsSystemTime (ptrMsgFileLog->stmOccurred); \
        ptrMsgFileLog->type = HCOMMON; \
        ::PostThreadMessage (PTR_HTHREAD_FILELOG->m_nThreadID, WM_APP_FILELOGMSG_WITHDT, (WPARAM) ptrMsgFileLog, 0); \
    }*/
    
    strLog.free ();

    if (PTR_DB) {
        HString strPathUndoClear = theApp.GetAppDirectory ();
        strPathUndoClear.appendChar ('\\');        
        strPathUndoClear.append (PTR_SETTINGS->GetOption (_T ("–¿«Ã≈Ÿ≈Õ»≈_Œ“Ã≈Õ¿")));
        
        HString strTimeRun, strPartTimeRun;
        int iPosPartTime = 0, iNumberPartTime = 0, iValuePartTime = -1;
        
        SYSTEMTIME stm;
	    (CTime::GetCurrentTime ()).GetAsSystemTime (stm);
	    strTimeRun = PTR_SETTINGS->GetOption (_T ("–¿«Ã≈Ÿ≈Õ»≈_Œ“Ã≈Õ¿_«¿œ”— "));
	    strPartTimeRun = strTimeRun.tokenize (_T (":"), iPosPartTime);
	    while ((strPartTimeRun.isUses () == 1) && (iNumberPartTime < 3)) {
            //¬ Õ¿ÿÂÏ ÙÓÏ‡“Â ‚ÂÏÂÌË 3 ˜‡Ò“Ë hh:nn:ss
	        iValuePartTime = strPartTimeRun.number ();

	        switch (iNumberPartTime) {
	            case 0: //◊¿—˚
	                stm.wHour = iValuePartTime;
	                break;
                case 1: //Ã»Õ”“€˚
                    stm.wMinute = iValuePartTime;
	                break;
	            case 2: //—≈ ”Õƒ˚
	                stm.wSecond = iValuePartTime;
	                break;
	            default:
	                ;
	        }
	    
	        strPartTimeRun = strTimeRun.tokenize (_T (":"), iPosPartTime);
	        iNumberPartTime ++;
	    }
        
        m_ptrThreadUndoClear->AddData (HThreadUndoClear::TIME_RUN, &stm, sizeof (SYSTEMTIME));	      

        long lSpanTimeHistory = PTR_DATETIMEINTERVAL->GetInterval (PTR_SETTINGS->GetOption (HString (_T ("–¿«Ã≈Ÿ≈Õ»≈_Œ“Ã≈Õ¿_»—“Œ–»ﬂ"))).number ());
        m_ptrThreadUndoClear->AddData (HThreadUndoClear::TIMESPAN_HISTORY, &lSpanTimeHistory, sizeof (long));
                
        if (! PathIsDirectory (strPathUndoClear.GetBuffer ()))
            CreateDirectory (strPathUndoClear.GetBuffer (), NULL);
        else
            ;

        m_ptrThreadUndoClear->AddData (HThreadUndoClear::STRING_DIRECTORYUNDO, strPathUndoClear);

        m_tmSpanWorkApp = 0;
        iPosPartTime = 0, iNumberPartTime = 0, iValuePartTime = -1;
        strTimeRun = PTR_SETTINGS->GetOption (_T ("¬–≈Ãﬂ_–¿¡Œ“€"));
	    strPartTimeRun = strTimeRun.tokenize (_T (":"), iPosPartTime);
	    while ((strPartTimeRun.isUses () == 1) && (iNumberPartTime < 3)) {
            //¬ Õ¿ÿÂÏ ÙÓÏ‡“Â ‚ÂÏÂÌË 3 ˜‡Ò“Ë hh:nn:ss

	        iValuePartTime = strPartTimeRun.number ();

	        switch (iNumberPartTime) {
	            case 0: //◊¿—˚
	                m_tmSpanWorkApp += iValuePartTime * 60 * 60;
	                break;
                case 1: //Ã»Õ”“€˚
                    m_tmSpanWorkApp += iValuePartTime * 60;
	                break;
	            case 2: //—≈ ”Õƒ˚
	                m_tmSpanWorkApp += iValuePartTime;
	                break;
	            default:
	                ;
	        }
	    
	        strPartTimeRun = strTimeRun.tokenize (_T (":"), iPosPartTime);
	        iNumberPartTime ++;	      
	    }
        
        m_tmSpanUpdate = 0;
        iPosPartTime = 0, iNumberPartTime = 0, iValuePartTime = -1;
        strTimeRun = PTR_SETTINGS->GetOption (_T ("Œœ–Œ—_—Œ—“ŒﬂÕ»ﬂ"));
	    strPartTimeRun = strTimeRun.tokenize (_T (":"), iPosPartTime);
	    while ((strPartTimeRun.isUses () == 1) && (iNumberPartTime < 3)) {
            //¬ Õ¿ÿÂÏ ÙÓÏ‡“Â ‚ÂÏÂÌË 3 ˜‡Ò“Ë hh:nn:ss
	        iValuePartTime = strPartTimeRun.number ();

	        switch (iNumberPartTime) {
	            case 0: //◊¿—˚
	                m_tmSpanUpdate += iValuePartTime * 60 * 60;
	                break;
                case 1: //Ã»Õ”“€˚
                    m_tmSpanUpdate += iValuePartTime * 60;
	                break;
	            case 2: //—≈ ”Õƒ˚
	                m_tmSpanUpdate += iValuePartTime;
	                break;
	            default:
	                ;
	        }
	    
	        strPartTimeRun = strTimeRun.tokenize (_T (":"), iPosPartTime);
	        iNumberPartTime ++;	      
	    }
        
        iRes = 0;
    }
    else
        iRes = -1; //ÕÂÚ ÒÓÂ‰ËÌÂÌËˇ Ò ¡ƒ

    strLog.format ("¬˚ıÓ‰ ËÁ CUserKTSApp::SetOption Ò ÍÓ‰ÓÏ: %i", iRes);
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), USER_KTS_APP)
    strLog.free ();
    
    return iRes;
}

afx_msg void CUserKTSApp::OperationUndoClear (WPARAM wp, LPARAM) {
    FILELOG_VARIABLE
    strLog.format (_T ("%S"), _T ("¬ıÓ‰ ‚ 'CUserKTSApp::OperationUndoClear' ·ÂÁ ‡„ÛÏÂÌÚŒ¬"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_APP);
    
    int iRes = 0,
        iIdProgressCtrl = -1; //»‰ÂÌÚËÙËÍ‡ÚÓ ‰ËÌ‡ÏË˜ÂÒÍË ÒÓÁ‰‡‚‡ÂÏŒ√Œ 'CProgressCtrl'
    
    if (m_ptrThreadUndoClear->CreateThread (CREATE_SUSPENDED)) {
        strLog.format (_T ("ƒÓ·‡‚ÎˇÂÏ Ô‡‡ÏÂÚ€ ‰Îˇ ÔÓÚÓÍ‡ ID=%i"), m_ptrThreadUndoClear->m_nThreadID);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_APP);
        strLog.free ();

        iRes = m_ptrThreadUndoClear->ResumeThread ();
        if (iRes == 0xFFFFFFFF) {
            iRes = GetLastError ();
            //PostMessage (WM_USER_EXIT_UNDO_CLEAR, NULL, m_pThreadUndoClear->m_nThreadID);
    	    
            //return ;
        }
		else {
			PostMessage ((HWND) wp, WM_USER_MAKEPROGRESSCTRL, m_ptrThreadUndoClear->m_nThreadID, 0);

            //::PostThreadMessage (m_pThreadUndoClear->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);
            //m_pThreadUndoClear->PostThreadMessage (WM_USER_THREAD_RESUME, 0, 0);
        }
    }
    else
        ; //œŒ“Œ  ÌÂ ÒÓÁ‰‡Ì ???

	strLog.format (_T ("%S –ÂÁ-Ú: %i"), _T ("¬˚ıÓ‰ ËÁ 'CUserKTSDlg::OperationUndoClear'"), iRes);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_APP);
}

afx_msg void CUserKTSApp::ExitUndoClear (WPARAM wp, LPARAM lp) {
    TerminateHThread (m_ptrThreadUndoClear);

    PostMessage ((HWND) wp, WM_USER_EXIT_UNDO_CLEAR, (WPARAM) *(int *) m_ptrThreadUndoClear->GetDataOfId (HThreadUndoClear::NUMBER_IDPROGRESSBAR), 0);
}
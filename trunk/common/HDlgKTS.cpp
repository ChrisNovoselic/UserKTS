#include "stdafx.h"
#include "resource.h" //Идентификаторы 'ICON...'
#include <HDlgKTS.h>

IMPLEMENT_DYNAMIC(HDlgKTS, HDlg)

EXT_DECLARE_HTHREAD_FILELOG

// CAboutDlg dialog used for App About

class HAboutDlg : public HDlg {
public:
	HAboutDlg (CWnd *pParent = 0x0) : HDlg (pParent) {
	}
	virtual ~HAboutDlg (void) {
	}
	void OnDestroy (void) {
		HDlg::OnDestroy ();
		
        delete GetDlgItem (IDOK); 
        delete GetDlgItem (IDC_STATIC + 666); 
        delete GetDlgItem (IDC_STATIC + 667); 
        delete GetDlgItem (IDC_STATIC + 668); 
	}

// Dialog Data
	//enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX)  {
		HDlg::DoDataExchange (pDX); // DDX/DDV support
	}
	virtual int OnInitDialog (void) {
		BOOL bRes = HDlg::OnInitDialog ();

		ModifyStyle (WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX, 0x0);

        UINT uiIdStaticCtrl = -1;

		CStatic *pStatic = 0x0;
        uiIdStaticCtrl = IDC_STATIC + 666;
		pStatic = DEBUG_NEW CStatic ();
		//bRes = pStatic->Create (_T (""), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE | /*SS_BITMAP |*/ /*SS_REALSIZEIMAGE |*/ SS_ICON /*| SS_BLACKRECT*/, CRect (20, 20, 60, 60), this, uiIdStaticCtrl);
		bRes = pStatic->Create (0x0, WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE | /*SS_BITMAP |*/ SS_REALSIZEIMAGE | SS_ICON /*| SS_BLACKRECT*/, CRect (10, 20, 60, 60), this, uiIdStaticCtrl);
		//pStatic->ModifyStyle (pStatic->GetStyle (), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE | SS_ICON | SS_BLACKRECT);
		//HANDLE hIcon = ::LoadIcon (AfxGetApp ()->m_hInstance, MAKEINTRESOURCE (IDI_ICON_H_48));
        HANDLE hIcon = GetIcon (true);
		pStatic->SetIcon ((HICON) hIcon);
		//::SendDlgItemMessage (m_hWnd, uiIdStaticCtrl, WM_SETICON, (WPARAM) hIcon, TRUE);
		DDX_Control (m_ptrDataExchange, uiIdStaticCtrl, *pStatic);

        //Получение информации из ресурсв о версии модуля
        //Вариант №1
        /*HString strProduct, strVersion;
        HWinApp::GetProductAndVersion (strProduct, strVersion);
        strProduct.to (HString::MBSTR); strVersion.to (HString::MBSTR);*/

        //Получение информации из ресурсв о версии модуля
        //Вариант №2
        /*_TCHAR strVer [MAX_PATH + 1], strModuleFileName [MAX_PATH + 1];
        int iVer =-1, szModuleFileName = -1;
        GetModuleFileName (AfxGetApp ()->m_hInstance, strModuleFileName, szModuleFileName);
        HWinApp::ReadVersion (strVer, iVer, strModuleFileName, _T ("FileVersion"));*/

        CDC * pDC = GetDC ();
        TEXTMETRIC pTextMetric;
        CRect rcThis; GetWindowRect (&rcThis); //GetClientRect (&rcThis);
        pDC->GetOutputTextMetrics (&pTextMetric);
        
        //Получение информации из ресурсв о версии модуля
        //Вариант №3
        HString strResourceValue;
        /*strResourceValue = HWinApp::ResourceVersion (_T ("FileVersion"));
        strResourceValue.to (HString::MBSTR); strResourceValue.trim ();*/
        strResourceValue = HWinApp::ResourceVersion (_T ("ProductVersion"));
        //strResourceValue.to (HString::MBSTR); strResourceValue.trim ();

        strResourceValue = HWinApp::ResourceVersion (_T ("ProductName")) + /*HWinApp::ResourceVersion (_T ("ProductVersion"))*/strResourceValue;
        LONG pxStaticWidth = pTextMetric.tmAveCharWidth * strResourceValue.length ();

        pStatic = 0x0;
        uiIdStaticCtrl = IDC_STATIC + 667;
		pStatic = DEBUG_NEW CStatic ();
		//bRes = pStatic->Create (WC_HSTRING (strResourceValue), WS_VISIBLE | WS_CHILD | SS_SIMPLE, CRect (60, 15, rcParent.Width () - 10, 30), this, uiIdStaticCtrl);
        bRes = pStatic->Create (WC_HSTRING (strResourceValue), WS_VISIBLE | WS_CHILD | SS_SIMPLE, CRect (60, 15, 60 + (int) pxStaticWidth, 30), this, uiIdStaticCtrl);
        ::SendDlgItemMessage (m_hWnd, uiIdStaticCtrl, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);
		DDX_Control (m_ptrDataExchange, uiIdStaticCtrl, *pStatic);

        strResourceValue = HWinApp::ResourceVersion (_T ("LegalCopyright"));
        if (pxStaticWidth < pTextMetric.tmAveCharWidth * strResourceValue.length ()) pxStaticWidth = pTextMetric.tmAveCharWidth * strResourceValue.length (); else ;

        //SetWindowPos (SWP_NOMOVE | SWP_NOZORDER);
        if ((pxStaticWidth + 80) < rcThis.Width ())
            ;
        else
            //MoveWindow (rcThis.left, rcThis.top, rcThis.left + pxStaticWidth + 80, rcThis.bottom);
            MoveWindow (rcThis.left, rcThis.top, pxStaticWidth + 80, rcThis.bottom - rcThis.top);

        pStatic = 0x0;
        uiIdStaticCtrl = IDC_STATIC + 668;
		pStatic = DEBUG_NEW CStatic ();
		bRes = pStatic->Create (WC_HSTRING (strResourceValue), WS_VISIBLE | WS_CHILD | SS_SIMPLE, CRect (60, 40, 60 + (int) pxStaticWidth, 55), this, uiIdStaticCtrl);
        ::SendDlgItemMessage (m_hWnd, uiIdStaticCtrl, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);
		DDX_Control (m_ptrDataExchange, uiIdStaticCtrl, *pStatic);

        CRect rcCtrl;
		/*GetWindowRect (&rcThis);*/ GetClientRect (&rcThis);
		rcCtrl.left = rcThis.Width () / 2 - 70 / 2; rcCtrl.right = rcThis.Width () / 2 + 70 / 2;
		rcCtrl.top = rcThis.Height () - 26 - 5; rcCtrl.bottom = rcThis.Height ()  - 5;

        uiIdStaticCtrl = IDOK;
		CButton *pButton = 0x0;
		pButton = DEBUG_NEW CButton ();
		bRes = pButton->Create (_T ("OK"), WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | WS_TABSTOP, /*CRect (90, 65, 160, 90)*/rcCtrl, this, uiIdStaticCtrl);
		::SendDlgItemMessage (m_hWnd, uiIdStaticCtrl, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);
		DDX_Control (m_ptrDataExchange, uiIdStaticCtrl, *pButton);

		SetWindowText (_T ("О программе"));

		return bRes;
	}

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(HAboutDlg, HDlg)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

HDlgKTS::HDlgKTS (CWnd *pParent) : HDlg (pParent),
									m_ptrStatusBar (0x0),
                                    m_arIdIndicators (0x0) {
}

HDlgKTS::~HDlgKTS (void) {
    //delete m_arIdIndicators;
}

BEGIN_MESSAGE_MAP(HDlgKTS, HDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
    ON_NOTIFY (NM_CLICK, IDC_TREECTRL_TEMPLATE, &OnNotifyNMClick)
	ON_COMMAND (IDM_HELP_ABOUT, &HDlgKTS::OnAboutHelp)
    //ON_COMMAND (IDM_RESTORE, &HDlgKTS::OnCommandRestoreDlg)
	ON_MESSAGE (WM_USER_MESSAGE_ICON_TRAY, &HDlgKTS::OnCommandIconTray)
	ON_MESSAGE (WM_USER_TOSYSTEMTRAY, &HDlgKTS::OnCommandToSystemTray)
END_MESSAGE_MAP ()

void HDlgKTS::DoDataExchange(CDataExchange* pDX) {
	HDlg::DoDataExchange (pDX);
}

BOOL HDlgKTS::OnInitDialog (void) {
    m_hIcon = ::LoadIcon (AfxGetApp ()->m_hInstance, MAKEINTRESOURCE (IDI_ICON_H_48));

    SetIcon (m_hIcon, TRUE);		// Set big icon
	SetIcon (m_hIcon, FALSE);		// Set small icon

	BOOL bRes = HDlg::OnInitDialog ();

	ModifyStyle (WS_MAXIMIZEBOX | WS_SIZEBOX, 0);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu (FALSE);
	if (pSysMenu != NULL) {
		pSysMenu->AppendMenu (MF_SEPARATOR);
		pSysMenu->AppendMenu (MF_STRING, IDM_ABOUTBOX, _T ("&О Программе"));
	}
	else
	    ;

	return bRes;
}

void HDlgKTS::OnSysCommand(UINT nID, LPARAM lParam) {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)	{
		HAboutDlg *ptrDlgAbout = DEBUG_NEW HAboutDlg (this);
		ptrDlgAbout->PreCreate (CRect (0, 0, 185, 60), HDLG_MODALES);
		ptrDlgAbout->DoModal ();
		ptrDlgAbout->DestroyWindow ();
		delete ptrDlgAbout;
	}
	else {
	    switch (nID) {
	        case SC_MINIMIZE:
			case SC_CLOSE:
	            SendMessage (WM_USER_TOSYSTEMTRAY);
	            break;
	        default:
	            HDlg::OnSysCommand (nID, lParam);
	    }
	}
}

BOOL HDlgKTS::PreTranslateMessage (MSG* pMsg) {
    BOOL bRet = TRUE;

    if (pMsg->message == WM_KEYDOWN) {
        switch (pMsg->wParam) {
            case VK_RETURN:
            case VK_ESCAPE:
                bRet = FALSE;
            default: ;
        }
    }
    else
        ;

    if (bRet)
        return HDlg::PreTranslateMessage (pMsg);
    else
        return bRet;
 }

void HDlgKTS::OnAboutHelp (void) {
    // TODO: Add your command handler code here
    SendMessage (WM_SYSCOMMAND, IDM_ABOUTBOX);
}

LRESULT HDlgKTS::OnCommandIconTray (WPARAM wp, LPARAM lp) {
    LRESULT lResult = TRUE;
	//CMenu *ptrMmenuTray = 0x0;
	CPoint ptCursor = 0x0;
    FILELOG_VARIABLE

	if (GetCursorPos (&ptCursor)) {    
		switch (lp) {
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
				((HWinAppKTS *) AfxGetApp ())->m_ptrMenuTray->TrackPopupMenu (TPM_LEFTALIGN, ptCursor.x, ptCursor.y, this);
				//m_ptrMenuTray->GetSubMenu (0)->TrackPopupMenu (TPM_LEFTALIGN, 0, 0, this);
				break;
			default:
				;
		}
	}
	else
		;

	//delete ptrMmenuTray;
    
    return lResult; //true ???
    //return GetParent ()->w
}

void HDlgKTS::OnDestroy (void) {
	m_ptrStatusBar->DestroyWindow ();
    delete m_ptrStatusBar;
    
    delete m_arIdIndicators;

    delete GetDlgItem (IDCANCEL);

	((HWinAppKTS *) AfxGetApp ())->ManagerIconTray (m_hWnd, NIM_DELETE);

	HDlg::OnDestroy ();
}

LRESULT HDlgKTS::OnCommandToSystemTray (WPARAM wp, LPARAM lp) {
	LRESULT lRes = 0;
    
   ((HWinAppKTS *) AfxGetApp ())->ManagerIconTray (m_hWnd, NIM_ADD, m_hIcon);
    
    FILELOG_VARIABLE
    strLog.format (_T ("Скрыли ГЛАВНое окно в 'SystemTray'..."));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HCOMMON);
    
    if (! ::ShowWindow (m_hWnd, SW_HIDE))
        lRes = -1;
    else
        ;

    return lRes;
}

CString HDlgKTS::makeTimeSpanRemaining (CTimeSpan tmSpan) { return (CTimeSpan (WORK_SECONDS) - tmSpan).Format (_T ("Осталось: %H:%M:%S")); }

CString HDlgKTS::makeTimeSpanNonstop (CTimeSpan tmSpan) {
    CString strRes;

    int days = tmSpan.GetDays ();
    TCHAR *pwchDays = NULL;
    
    switch (days % 10) {
        case 0:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            pwchDays = _T ("дней");
            break;
        case 1:
            pwchDays = _T ("день");
            break;
        case 2:
        case 3:
        case 4:
            pwchDays = _T ("дня");
            break;
        default:
            ;
    }
    
    strRes.Format (_T ("Всего: %i %s "), days, pwchDays);
    strRes += tmSpan.Format (_T ("%H:%M:%S"));
    
    return strRes;
}

HString HDlgKTS::TimeIndicators (int idTimePassed, int idTimeCurrent) {
    HString strRes;

    CTime tmCurrent = CTime::GetCurrentTime ();

    m_ptrStatusBar->SetPaneText (idTimePassed, tmCurrent.Format (_T ("%d.%m.%Y %H:%M:%S")));
	if (IS_RUN_REMAINING) {
        //Время работы в режиме запуска по расписанию (осталось до АВТОзавершения)
        strRes = makeTimeSpanRemaining (tmCurrent - ((HWinApp *) AfxGetApp ())->GetTimeStart ());
    }
    else {
        //Время работы в беспрерывном режиме (ВСЕГо)
        strRes = makeTimeSpanNonstop (tmCurrent - ((HWinApp *) AfxGetApp ())->GetTimeStart ());
    }
    
    m_ptrStatusBar->SetPaneText (idTimeCurrent, WC_HSTRING (strRes));

    return strRes;
}

BOOL HDlgKTS::OnCmdMsg (UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
    FILELOG_VARIABLE

    int i = -1;
    CString strTemp;
    strTemp.Format (_T ("CDialog::OnCmdMsg::default {nID=%x, nCode=%x}\n"), nID, nCode);
    
    if (pHandlerInfo == NULL) {
        switch (nCode) {
            //case -1:
            case CN_COMMAND: {
                    if ((nID == IDM_RESTORE) || (nID == IDM_RESTORE_CONTROL) || (nID == IDM_RESTORE_FILES_DB)) {
                        if ((nID == IDM_RESTORE) ||
                            ((nID == IDM_RESTORE_CONTROL) && (HString (GetRuntimeClass ()->m_lpszClassName).compareNoCase (_T ("HDlgGUIControl")) == 0)) ||
                            ((nID == IDM_RESTORE_FILES_DB) && (HString (GetRuntimeClass ()->m_lpszClassName).compareNoCase (_T ("HDlgGUIFilesDB")) == 0))) {
                            ::ShowWindow (m_hWnd, SW_RESTORE);

                            strLog.format (_T ("Восстановили окно %s из 'SystemTray'..."), MB_HSTRING (HString (GetRuntimeClass ()->m_lpszClassName)));
                            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HCOMMON);
                        }
                        else {
                            ::ShowWindow (m_hWnd, SW_HIDE);
                            
                            AfxGetApp ()->PostThreadMessage (IDM_RESTORE, nID, 0);
                        }

                        ((HWinAppKTS *) AfxGetApp ())->ManagerIconTray (m_hWnd, NIM_DELETE);
                    }
                    else ;                    
                }
                break;
            case WM_NOTIFY:
                {
                    //TRACE ("HDlgKTS::OnCmdMsg::WM_NOTIFY");
                }
                break;
            case 0x4efff4:
                //TRACE (strTemp);
                break;
            case 0x4effef:
                //TRACE (strTemp);
                break;
            case 0x4efffe:
                //TRACE (strTemp);
                break;
            default:
            {
                //TRACE (strTemp);
            }
        }
    }
    
    return HDlg::OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
}

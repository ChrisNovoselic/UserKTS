// UserKTSDlg.cpp : implementation file
//

#include "stdafx.h"

#ifdef PTR_DB
#include "UserKTS.h"
extern CUserKTSApp theApp;
#else
extern CWinApp theApp;
#endif

#ifdef _DEBUG
extern CMemoryState g_memStateBegin, g_memStateEnd, g_memStateDiff;
#endif

#include "UserKTSDlg.h"
#include "HSortableArray.h"
#include "HObjectTemplate.h"
#include "HDateTimeInterval.h"
#include "HDlgChoiceDirectoryTemplate.h"

#if ((defined PTR_DB) || (defined PTR_DAO_DB))
extern BOOL FreeMemoryDBObject (CObject *&);
#endif

#ifdef _USE_SQL
#include "HData.h"
#endif

EXT_DECLARE_HDATETIMEINTERVAL
EXT_DECLARE_HTHREAD_FILELOG

#if defined _WORK_THREAD
extern DWORD WINAPI fThreadDirectoryWatched (LPVOID );
//extern DWORD WINAPI fThreadCreateTemplate (LPVOID ); //Теперь этОТ ПОТОК реализован на ОСНОВе класса 'HThreadCreateTemplate'
#else
#endif

// CUserKTSDlg dialog

CUserKTSDlg::CUserKTSDlg (CWnd* pParent /*=NULL*/) : HDlgKTSFiles (pParent),
                                                    m_iIdTimerUpdate (0x0),
                                                    m_ptrStaticFullPath (0x0)
{
    FILELOG_VARIABLE

	//m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_H_48);

    m_pmapDBRS.RemoveAll ();

    FILELOG_WRITE_WITHDT ("Конструктор главного окна...", HCOMMON);

	m_iIdTimerUpdate = -1;

	m_mapPtrThread.InitHashTable (47);

	FILELOG_WRITE_WITHDT ("Создание главного ОКНа - успешно...", HCOMMON);
}

CUserKTSDlg::~CUserKTSDlg () {
    //Destructor ();

    int i;
    FILELOG_VARIABLE

	for (i = 0; i < m_arpProgressCtrl.GetSize (); i ++) {
		m_arpProgressCtrl.GetAt (i)->DestroyWindow ();
	    delete m_arpProgressCtrl.GetAt (i);
	}
	m_arpProgressCtrl.RemoveAll ();

	FILELOG_WRITE_WITHDT ("Деструктор ГЛАВНого ОКНа ...", HCOMMON);
}

void CUserKTSDlg::Destructor (void) {
}

void CUserKTSDlg::DoDataExchange(CDataExchange* pDX) {
    HDlgKTSFiles::DoDataExchange (pDX);
}

BEGIN_MESSAGE_MAP(CUserKTSDlg, HDlgKTSFiles)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    /*ON_WM_KEYDOWN ()
    ON_WM_SYSKEYDOWN ()*/
	//}}AFX_MSG_MAP
    ON_WM_CREATE ()
    ON_BN_CLICKED (IDC_COMMAND_DIR_LIST, &CUserKTSDlg::OnBnClickedCommandDirList)
    ON_NOTIFY (TVN_SELCHANGED, IDC_TREECTRL_TEMPLATE, &CUserKTSDlg::OnTvnSelchangedTemplate)
    ON_COMMAND (IDM_TEMPLATE_ADD, &CUserKTSDlg::OnAddTemplate)
    ON_COMMAND (IDM_TEMPLATE_CHANGE, &CUserKTSDlg::OnChangeTemplate)
    ON_COMMAND (IDM_TEMPLATE_DEL, &CUserKTSDlg::OnDelTemplate)
    ON_MESSAGE (WM_USER_MESSAGE_DIR_WATCH, &CUserKTSDlg::OnCommandThreadDirWatch)
	ON_COMMAND (IDM_EXIT, &CUserKTSDlg::OnCommandExit)
    ON_MESSAGE (WM_USER_MESSAGE, &CUserKTSDlg::OnUserMsg)
    ON_MESSAGE (WM_USER_MAKEPROGRESSCTRL, &OnMakeProgressCtrl)
    ON_MESSAGE (WM_USER_EXIT_CREATE_TEMPLATE, &CUserKTSDlg::OnCommandExitCreateTemplate)
	ON_MESSAGE (WM_USER_LIVE_CREATE_TEMPLATE, &CUserKTSDlg::OnCommandLiveCreateTemplate)
    ON_MESSAGE (WM_USER_ENTER_DIRECTORY_WATCHED, &CUserKTSDlg::OnCommandEnterDirWatched)
    ON_MESSAGE (WM_USER_LEAVE_DIRECTORY_WATCHED, &CUserKTSDlg::OnCommandLeaveDirWatched)
    ON_MESSAGE (WM_USER_PROGRESS_RULES, &CUserKTSDlg::OnCommandProgressRules)
    ON_MESSAGE (WM_USER_ENDWORKBD_CREATE_TEMPLATE, &CUserKTSDlg::OnCommandEndWorkBDCreateTemplate)
    ON_WM_TIMER ()
    ON_WM_CLOSE ()
    ON_WM_DESTROY ()
    ON_BN_CLICKED (IDCANCEL, &CUserKTSDlg::OnBnClickedCancel)
    ON_MESSAGE (WM_USER_EXIT_UNDO_CLEAR, &CUserKTSDlg::OnCommandExitUndoClear)
END_MESSAGE_MAP();

// CUserKTSDlg message handlers
BOOL CUserKTSDlg::OnInitDialog () {
	HDlgKTSFiles::OnInitDialog ();

	SetWindowText (_T ("UserKTS"));

	//HANDLE hMainMenu = LoadMenu (AfxGetApp ()->m_hInstance, );
	/*CMenu *ptrMainMenu = DEBUG_NEW CMenu ();
	ptrMainMenu->LoadMenu (IDR_MENU_MAIN);*/
	SetMenu (CreateMainMenu ());

    int iRes = 0;

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon (m_hIcon, TRUE);		// Set big icon
	SetIcon (m_hIcon, FALSE);		// Set small icon
	
	m_ToolBar.Create (this);

	CRect rcThis, rcStatusBar,
            rcCtrl;
	GetClientRect (&rcThis);

	CreateStatusBar (this);
    m_ptrStatusBar->GetClientRect (&rcStatusBar);
    
    CRect rcTreeCtrl;
    m_pTreeCtrlTemplate = DEBUG_NEW HTreeCtrlTemplate ();
    rcTreeCtrl.left = OFFSET_CONTROL; rcTreeCtrl.right = 0.35 * rcThis.Width ();
    rcTreeCtrl.top = OFFSET_CONTROL; rcTreeCtrl.bottom = rcThis.bottom - rcStatusBar.Height () - OFFSET_CONTROL;
	m_pTreeCtrlTemplate->Create (WS_VISIBLE | WS_BORDER | WS_OVERLAPPED | WS_CHILD | WS_HSCROLL  | WS_VSCROLL | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS /*| TVS_CHECKBOXES | TVS_SINGLEEXPAND*/, rcTreeCtrl, this, IDC_TREECTRL_TEMPLATE);
    
    CreateCommonCtrl (this);

    /*pButton = DEBUG_NEW CButton ();
    iRes = pButton->Create (_T ("->"), WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | WS_TABSTOP, CRect (489, 277, 519, 299), this, IDC_COMMAND_OPERATION);*/

    /*pButton = DEBUG_NEW CButton ();
    iRes = pButton->Create (_T ("Закрыть"), WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | WS_TABSTOP, CRect (529, 277, 602, 299), this, IDCANCEL);
    ::SendDlgItemMessage (m_hWnd, IDCANCEL, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);*/

    m_ptrStaticFullPath = DEBUG_NEW CStatic ();
    rcCtrl.left = rcTreeCtrl.right + OFFSET_CONTROL; rcCtrl.right = rcThis.Width () - 30 - 2 * OFFSET_CONTROL;
    rcCtrl.top = OFFSET_CONTROL; rcCtrl.bottom = rcCtrl.top + HEIGHT_CONTROL;
    m_ptrStaticFullPath->Create (_T (""), WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | WS_BORDER | SS_SUNKEN /*| SS_SIMPLE*/, rcCtrl, this, IDC_STATIC_FULLPATH);
    ::SendDlgItemMessage (m_hWnd, IDC_STATIC_FULLPATH, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);
    DDX_Control (m_ptrDataExchange, IDC_STATIC_FULLPATH, *m_ptrStaticFullPath);

    CButton *pButton = 0x0;
    pButton = DEBUG_NEW CButton ();
    rcCtrl.left = rcThis.Width () - 30 - OFFSET_CONTROL; rcCtrl.right = rcThis.Width () - OFFSET_CONTROL;
    rcCtrl.top = OFFSET_CONTROL; rcCtrl.bottom = rcCtrl.top + HEIGHT_CONTROL;
    iRes = pButton->Create (_T ("->"), WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | WS_TABSTOP, rcCtrl, this, IDC_COMMAND_DIR_LIST);
	
	//m_pTreeCtrlTemplate->ModifyStyle (TVS_CHECKBOXES, 0); 
    //m_pTreeCtrlTemplate->ModifyStyle (0, TVS_CHECKBOXES);

	//ЭТОт RS ОСОБЕНный
	CRecordset *pRS = DEBUG_NEW CRecordset (PTR_DB);
    pRS->m_strFilter.Format (_T ("ПАРАМЕТР='?'"));
    HString strQueryOption; strQueryOption.format ( _T ("SELECT ЗНАЧЕНИЕ FROM [%S]"), _T (TABLENAME_OPTION));
    pRS->Open (CRecordset::dynaset, WC_HSTRING (strQueryOption));
    strQueryOption.free ();
    m_pmapDBRS.SetAt (_T (TABLENAME_OPTION), pRS);
	
	if ((m_pTreeCtrlTemplate->InitializeRS (_T (TABLENAME_TEMPLATE)) == 0) && (m_pTreeCtrlTemplate->InitializeRS (_T (TABLENAME_TIME)) == 0)) {
        //ПроИНИЦИАЛИЗируем 'HMapInterval' в 'HTreeCtrlTeplate', раз уж там есть RS с ТАБЛицей 'Time'
        //При ЭТОм 'HDateTimeInterval' - класс для ГЛОБАЛьнОГо обЪектА
    
        //if ((InitializeRS (_T ("Source")) == 0) && (InitializeRS (_T ("queryOption")) == 0))
        if (PTR_DATETIMEINTERVAL->IsInitialize () > 0) {
            //УЖЕ д.б. проИНИЦИАЛИЗироВан 'HDateTimeInterval'
            ////Д.б. интерпретирована таблица 'Option' для приложения
            //Д.б. установлен РЕЖИМ_РАБОТЫ приложениЯ (вызов 'SetModeApp')

            m_iIdTimerUpdate = SetTimer (WM_USER_ID_TIMER_UPDATE, UPDATE_SECONDS * 1000, NULL);

            iRes = m_pTreeCtrlTemplate->InitializeImageList ();
            
            if (iRes == 0)
                if (m_pTreeCtrlTemplate->InitializeItems (0) == 1)
                    //Была добавлена ХОТя бы ОДНа записЬ
                    iRes = 0;
                else
                    iRes = -1;
            else
                ;
        }
        else
            ;
                
        m_pTreeCtrlTemplate->ReleaseRS (_T (TABLENAME_TEMPLATE));
        m_pTreeCtrlTemplate->ReleaseRS (_T (TABLENAME_TIME));
    }
    else  {
        //Инициализация 'Template' и 'Time'
        MessageBox (_T("Инициализация 'Template' и 'Time'!"), _T("Ошибка!"), MB_ICONINFORMATION); //а для КУЧи ЕЩё и 'Operation' ???
        iRes = -1;
    }

    //ОсвобождаЕМ ОСОБЕНный RS
    FreeMemoryDBObject ((CObject *&) pRS);
    m_pmapDBRS.RemoveKey (_T (TABLENAME_OPTION));
        
    if (iRes == 0)
        if (!
#if defined _WORK_THREAD
        CreateThreadsDirectoryWatched
#else
        CreateUIThreadsDirectoryWatched
#endif
        ())
            iRes = -1; //ERROR
    else
        ;

    if (iRes == 0)
        //if (InitializeRS (_T (TABLENAME_OPERATION)) == 0)
        {
            iRes = InitializeOperation ();
        }
        //else
        //    iRes = -1;
    else
        ;
        
    if (iRes == 0) {        
        if (MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_VIEW)
        {//Работа в РЕЖИМЕ 'Обзор, КОНТРОЛЬ и ИЗМЕНЕНИЕ состояния ШАБЛОНов'
            //ПодМЕНю 'ШАБЛОНы' - НЕдоступно
            GetMenu ()->GetSubMenu (0)->EnableMenuItem(IDM_TEMPLATE_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
            GetMenu ()->GetSubMenu (0)->EnableMenuItem(IDM_TEMPLATE_CHANGE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
            GetMenu ()->GetSubMenu (0)->EnableMenuItem(IDM_TEMPLATE_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
            //Кнопка '->' (ChangeDirectory) НЕдоступна
            GetDlgItem (IDC_COMMAND_DIR_LIST)->EnableWindow (false);
        }
        else
            ;

        //Освобождение БД
        PTR_DB->Disconnect ();
    }
    else
        ;

#if defined _DEBUG
    g_memStateEnd.Checkpoint ();
    if (g_memStateDiff.Difference (g_memStateBegin, g_memStateEnd)) {
        //g_memStateDiff.DumpAllObjectsSince ();
        //g_memStateDiff.DumpStatistics ();
    }
    else
        ;
        
    g_memStateEnd.DumpAllObjectsSince ();
#endif
  
    if (iRes == 0) {        
        m_pTreeCtrlTemplate->SelectItem (m_pTreeCtrlTemplate->GetRootItem ());
        m_pTreeCtrlTemplate->Expand (m_pTreeCtrlTemplate->GetRootItem (), TVE_EXPAND);
        
        if (IS_RUN_REMAINING)
            PostMessage (WM_USER_TOSYSTEMTRAY);
        else
            ;
            
//#ifdef _DEBUG
    //g_memStateEnd.Checkpoint ();
    //if (g_memStateDiff.Difference (g_memStateBegin, g_memStateEnd))
    //{
    //    g_memStateDiff.DumpAllObjectsSince ();
    //    g_memStateDiff.DumpStatistics ();
    //}
    //else
    //    ;
//#endif
        
        return true;
    }
    else
        return false;
}

//afx_msg void CUserKTSDlg::OnKeyDown(UINT , UINT , UINT) {
//}
//
//afx_msg void CUserKTSDlg::OnSysKeyDown(UINT , UINT , UINT) {
//}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUserKTSDlg::OnPaint (void) {
	if (IsIconic ()) {
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
	else {
		HDlgKTSFiles::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUserKTSDlg::OnQueryDragIcon (void) {
	return static_cast<HCURSOR>(m_hIcon);
}

int CUserKTSDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    //DWORD dwExStyle = 0;
    //dwExStyle = lpCreateStruct->dwExStyle;
    ////dwExStyle |= SW_SHOWNORMAL;
    //dwExStyle &= ~SW_SHOWNORMAL;
    //dwExStyle &= ~SW_SHOW;
    //dwExStyle &= ~SW_SHOWMAXIMIZED;
    //dwExStyle &= ~SW_SHOWDEFAULT;

    //lpCreateStruct->dwExStyle = dwExStyle;
    if (HDlgKTSFiles::OnCreate (lpCreateStruct) == -1)
        return -1;

    // TODO:  Add your specialized creation code here
    return 0;
}

LRESULT CUserKTSDlg::OnUserMsg (WPARAM wp, LPARAM lp) {
    return 0;
}

LRESULT CUserKTSDlg::OnCommandExitUndoClear (WPARAM wp, LPARAM lp) {
    int iRes = 0; //Success
        
    iRes = RemoveProgressCtrl ((int) wp);
    
    return iRes;
}

int CUserKTSDlg::getIndexThreadCreateTemplate (int idThread) {
	FILELOG_VARIABLE
	int i = -1;
	void *ptr = 0x0;

	//if (m_arPtrThreadCreateTemplate.GetSize ()) {//Ещё бы он не имел элементов !!! - кто-то же прислал это сообщение ???
 //       //НайдЁм обЪект описания, соответствующий завершившемуся потоку
 //       for (i = 0; i < m_arPtrThreadCreateTemplate.GetSize (); i ++)
 //           if (((HThreadCreateTemplate *) m_arPtrThreadCreateTemplate.GetAt (i))->m_nThreadID == idThread) {
 //               //return ((HThreadCreateTemplate *) m_arPtrThreadCreateTemplate.GetAt (i));
	//			break;
 //           }
 //           else
 //               ;
 //   }
 //   else {
 //       //Нет ни ОДНого ПОТОКа
 //       strLog.format (_T ("Нет ни ОДНого ПОТОКа ..."));
 //       FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
 //   }

	//if (i < m_arPtrThreadCreateTemplate.GetSize ())
	if (m_mapPtrThread.Lookup (idThread, ptr))
		return 0;
	else
		return -1;
}

LRESULT CUserKTSDlg::OnCommandLiveCreateTemplate (WPARAM wp, LPARAM lp) {
    FILELOG_VARIABLE

	int iRes = 0, //Success
        i, idThread = (int) wp;
	HThreadCreateTemplate *pThreadCreateTemplate = NULL;

	strLog.format (_T ("Принято сообщение о состоянии ПОТОКа ID = %i"), idThread);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);

	return iRes;
}

LRESULT CUserKTSDlg::OnCommandExitCreateTemplate (WPARAM wp, LPARAM lp) {
    FILELOG_VARIABLE
    //strLog.format (_T ("Принято сообщение о завершении ПОТОКа (ID в следующем сообщении)..."));
    //FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
    //FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);

    //HThreadCreateTemplate *pThreadCreateTemplate = NULL;
    //HCheckStateTemplate *pCST = NULL;
    int iRes = 0; //Success

    strLog.format (_T ("Принято сообщение о завершении ПОТОКа ID = %i"), (int) wp);
    //FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);

	if (! (getIndexThreadCreateTemplate ((int) wp) < 0)) {        
		//TerminateHThread ((HThreadCreateTemplate *) m_mapPtrThread [(int) wp], true);
		((HWinApp *) AfxGetApp ())->TerminateHThread ((HThreadCreateTemplate *) m_mapPtrThread [(int) wp]);
		TerminateHThreadCreateTemplate ((HThreadCreateTemplate *) m_mapPtrThread [(int) wp]);
		DELETE_HTHREAD (m_mapPtrThread [(int) wp], HThreadCreateTemplate)
        
        m_mapPtrThread.RemoveKey ((int) wp);
    }
    else {
        //Не найден ЗАВЕРШИВШИЙся ПОТОК, который об этом уведомил САМ ???
        strLog.format (_T ("НЕ найден обЪект описания ПОТОКа ID = "), (int) wp);
        //FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);
    }
        
    if (m_mapPtrThread.GetSize () == 0) {
        strLog.format (_T ("ПОТОКов больше НЕТ - Закрываем БД!"));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);
        
        if (PTR_DB) {
            //БЕЗ УСЛОВий ЗАКРоем БД (из-за ПОВТОРного открытия 'TEMPLATE_XXX')
            //InitializeRS (_T ("queryOption"));
            //int iModeWork = _wtoi (GetOption (_T("РЕЖИМ_РАБОТЫ")).GetBuffer ());
            //ReleaseRS (_T ("queryOption"), true, true);
        
            //if (iModeWork == 0)
            {//Освобождение БД
                theApp.DisconnectDB ();
            }
        }
        else
            ;
    }
    else {//БД ещё НЕОБХОДИМа другим ПОТОКам, НО ПОТОК необходимо удалить
    }
        
#ifdef _DEBUG
    //g_memStateEnd.Checkpoint ();
    //BOOL bMemState = g_memStateBegin.Difference (g_memStateBegin, g_memStateEnd);
    //g_memStateBegin.DumpAllObjectsSince();
    //g_memStateBegin.DumpStatistics ();
    //g_memStateEnd.DumpAllObjectsSince();
#endif
        
    return 0;
}

LRESULT CUserKTSDlg::OnCommandEnterDirWatched (WPARAM wp, LPARAM lp) {
    return 0x0; //???
}

LRESULT CUserKTSDlg::OnCommandLeaveDirWatched (WPARAM wp, LPARAM lp) {
    HThreadDirectoryWatched *pThreadDirWatched = NULL;
    //HCheckStateTemplate *pCST = NULL;
    int iRes = 0, //Success
        i, idThread = *(int *) wp;
    
    FILELOG_VARIABLE
    strLog.format (_T ("Принято сообщение о завершении ПОТОКа ID = %i"), idThread);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
    
    if (m_arPtrThreadDirWatched.GetSize ())
    {//Ещё бы он не имел элементов !!! - кто-то же прислал это сообщение ???
        //НайдЁм обЪект описания, соответствующий завершившемуся потоку
        for (i = 0; i < m_arPtrThreadDirWatched.GetSize (); i ++)
            if (((HThreadDirectoryWatched *) m_arPtrThreadDirWatched.GetAt (i))->m_nThreadID == idThread)
            {
                pThreadDirWatched = ((HThreadDirectoryWatched *) m_arPtrThreadDirWatched.GetAt (i));
                break;
            }
            else
                ;

        if (pThreadDirWatched)
        {
        }
        else
            ;
    }
    else
        ;
        
    return iRes;
}

LRESULT CUserKTSDlg::OnCommandProgressRules (WPARAM wp, LPARAM lp) {
    CProgressCtrl *pProgressCtrl = NULL;
    int i, idProgressCtrl = -1;

    FILELOG_VARIABLE

    strLog.format (_T ("Принято сообщение от ПОТОКа с HANDLE=%i - состояние процесса выполнения операция: %i"), (int) wp, lp);
    FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), USER_KTS_DLG);

    //Выбор ПОТОКа, отправивШЕГо СООБЩение (ОЧИСТКа/Создание файла)
	if (! (getIndexThreadCreateTemplate ((int) wp) < 0)) {
        idProgressCtrl = *((int *) ((HThreadCreateTemplate *) m_mapPtrThread [(int) wp])->GetDataOfId (HThreadCreateTemplate::NUMBER_IDPROGRESSBAR)); //'::NUMBER_IDPROGRESSBAR' для разНЫх клаССов ПОТОКов НЕ ОДИНАКов !!!
		((HThreadCreateTemplate *) m_mapPtrThread [(int) wp])->setLastResponce ();
	}
    else //Нет ПОТОКа с известным 'ID' ???
        if (PTR_THREAD_UNDOCLEAR) {//Возможно, это ПОТОк очистки директории с временными резервными копиями ???
            try { idProgressCtrl = *((int *) PTR_THREAD_UNDOCLEAR->GetDataOfId (HThreadUndoClear::NUMBER_IDPROGRESSBAR)); } //'::NUMBER_IDPROGRESSBAR' для разНЫх клаССов ПОТОКов НЕ ОДИНАКов !!!
            catch (CException *err) {
                strLog.format (_T ("Ошибка обращения по указателю PTR_THREAD_UNDOCLEAR (...)"));
                FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HERROR);

                idProgressCtrl = -1;
            }
        }
        else
            ; //ДЕЙСТВИТЕЛЬно Нет НИ ОДНого ПОТОКа ??? (А КТО же прислал сообщение...)

    if (m_arpProgressCtrl.GetSize () && (idProgressCtrl > 0)) {
        //НайдЁм 'ProgressCtrl' соответствующий ПОТОКу
        for (i = 0; i < m_arpProgressCtrl.GetSize (); i ++)
            if (m_arpProgressCtrl.GetAt (i)->GetDlgCtrlID () == idProgressCtrl) {
                pProgressCtrl = m_arpProgressCtrl.GetAt (i);
                break;
            }
            else
                ;

        if (pProgressCtrl) {
            if (((int) lp) == 0)
                pProgressCtrl->OffsetPos (1);
			else {                    
                if (((int) lp) > 0)
					pProgressCtrl->SetRange (0, (int) lp);
				else {
					if ((pProgressCtrl->GetPos () + (int) lp) > 0)
						pProgressCtrl->OffsetPos ((int) lp);
					else
						pProgressCtrl->SetPos (0);
				}
            }
        }
        else
            ; //Нет НИ ОДНого 'ProgressCtrl' с известным id ???
    }
    else
        ; //Нет НИ ОДНого 'ProgressCtrl' ???

    return 0;
}

LRESULT CUserKTSDlg::OnCommandEndWorkBDCreateTemplate (WPARAM , LPARAM ) {
    return 0;
}

void CUserKTSDlg::OnCommandExit (void) {
	int iRes = -1;
	
	iRes = GetDlgItem (IDCANCEL)->SendMessage (BM_CLICK);

	//OnBnClickedCancel ();
}

LRESULT CUserKTSDlg::OnCommandThreadDirWatch (WPARAM ptrId, LPARAM bytes) {
    LRESULT lResult = 0;

#if defined _DEBUG
    CMemoryState memStateBegin, memStateEnd, memStateDiff;
    memStateBegin.Checkpoint ();
#endif

    if (bytes > 0) {
        //BOOL bWorking;
	    DWORD bytesDiff = bytes;
	    PFILE_NOTIFY_INFORMATION pFNI = NULL;

        FILELOG_VARIABLE
	    //CString strFullPath, strNameTemplate, strExt;
	    HString pwchDirWatched;
	    LPWSTR pwchNameTemplate = NULL, pwchExt = NULL, pwchFullPath = NULL;
	    DWORD shLenNameTemplate, shLenExt = 3;

	    int i, iItem = -1, idItem = 0, iCountRec = 0;

	    HCheckStateTemplate *pCST = NULL;

	    SYSTEMTIME sTime;

	    CFileFind filefind;
	    CFileStatus fStatus;

	    NMHDR hdr;

	    LVITEM lvItem;
	    lvItem.mask = LVIF_STATE;
        lvItem.stateMask = LVIS_SELECTED;
        
	    /*
	    BYTE *pLocalData = DEBUG_NEW BYTE [bytes];
	    //ZeroMemory (pLocalData, bytes + 1);
	    //CopyMemory ((void *) ptr, pLocalData, bytes);
	    memset (pLocalData, 0x0, bytes + 1);
	    memcpy ((void *) ptr, pLocalData, bytes);
	    pFNI = (FILE_NOTIFY_INFORMATION *) pLocalData;
        */
    	
	    //MessageBox (_T("WM_USER"), _T("Отладка!"), MB_OK);
    	
	    //Информация о ПОТОКе
	    HThreadDirectoryWatched *pThreadDirWatched = NULL;
	    for (i = 0; i < m_arPtrThreadDirWatched.GetSize (); i ++) {
	        if (((HThreadDirectoryWatched *) m_arPtrThreadDirWatched.GetAt (i))->m_nThreadID == *((DWORD *) ptrId)) {
	            pThreadDirWatched = (HThreadDirectoryWatched *) m_arPtrThreadDirWatched.GetAt (i);
	            break;
	        }
	        else
	            ;
	    }

	    //if (indxThreadDirWatched > -1)
	    //if (indxThreadDirWatched < m_arDescThreadDirWatched.GetSize ())
	    if (pThreadDirWatched) {
            strLog.format ("'::OnCommandThreadDirWatch...поток с id = %i, получено байт: %i", pThreadDirWatched->m_hThread, bytes);
            FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), USER_KTS_DLG);
            strLog.free ();
            
            pFNI = (FILE_NOTIFY_INFORMATION *) pThreadDirWatched->getFNI ();

            //FREE_PTR (pwchDirWatched)
            pwchDirWatched.free ();
            
            pwchDirWatched = pThreadDirWatched->stringData (pThreadDirWatched->GetDataOfId (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED));
            
            strLog.format (_T ("'::OnCommandThreadDirWatch...целевой каталог = %S"), WC_HSTRING (pwchDirWatched));
            FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), THREAD_WATCH_DIRECTORY);
            strLog.free ();

            do {
                shLenNameTemplate = pFNI->FileNameLength / sizeof (WCHAR);
                if ((shLenNameTemplate > bytes) || (! (shLenNameTemplate > 0)))
                    break;
                else
                    ;
                    
                //pFNI->FileName [pFNI->FileNameLength / 2] = 0x0;
                //Имя файла у нас дается относительно папки
                //т.е.если изменится файл 'strCurDir'\File\test.dat, то FileName
                //будет равно File\test.dat, где 'strCurDir' - исследуемый каталог
                
                //strNameTemplate = CString (pFNI->FileName);
                
                pwchNameTemplate = DEBUG_NEW WCHAR [shLenNameTemplate - (shLenExt + 1) + 1];
                pwchExt = DEBUG_NEW WCHAR [shLenExt + 1];
                i = shLenNameTemplate;
                while (i) {   
                    if (i < (shLenExt + 1)) {
                        pwchExt [shLenExt - i] = *(((WCHAR *) pFNI->FileName) + (shLenNameTemplate - i));
                        //if (i == shLenExt)
                        //    pwchNameTemplate [shLenNameTemplate - i] = '*';
                        //else
                        //    ;
                    }
                    else {
                        //strNameTemplate.appendChar (*(((WCHAR *) pFNI->FileName) + (pFNI->FileNameLength - shLenNameTemplate)));
                        if ((! (i == shLenExt)) && ((! (i == (shLenExt + 1)))))
                            pwchNameTemplate [shLenNameTemplate - i] = *(((WCHAR *) pFNI->FileName) + (shLenNameTemplate - i));
                        else
                            ;
                    }
                    
                    i --;
                }
                
                pwchNameTemplate [shLenNameTemplate - (shLenExt + 1)] = 0x0;
                pwchExt [shLenExt] = 0x0;
                
		        //strExt = strNameTemplate.Right (strNameTemplate.GetLength () - strNameTemplate.ReverseFind ('.') - 1);

		        //MessageBox (strNameTemplate, _T("Отладка!"), MB_OK);
    		    
		        //strNameTemplate.Empty ();
		        //strExt.Empty ();
    		    
		        //strNameTemplate = CString (pwchNameTemplate);
		        //strExt = CString (pwchExt);
    		    
		        //delete pwchNameTemplate;
		        //delete pwchExt;

		        //if (HTreeCtrlTemplate::IsValidExt (CString (pwchExt)) == 0)
		        if (HTreeCtrlTemplate::IsValidExt (pwchExt) == 0) {
					//Изменения произошли с НАШим файлОМ 
			        //Директория в кот. произошло изменение
			        //strDirectoryChange = strNameTemplate.Left (strNameTemplate.ReverseFind ('\\'));
			        //Шаблон ИМЕНи файла (ПОЛНЫЙ путь ШАБЛОНа)
			        //strNameTemplate.Replace (strNameTemplate.Right (3), _T("*"));
			        //Шаблон ТОЛЬКО ИМени файла
			        //strNameTemplate = strNameTemplate.Right (strNameTemplate.GetLength () - strNameTemplate.ReverseFind ('\\') - 1);
        			

			        //MessageBox (_T("Ext Is Valid!"), _T("Отладка!"), MB_OK);
        			
			        /*
			        for (i = 0; i < arStrNameTemplate.GetSize (); i++)
			        {
				        if ((strNameTemplate.Compare (arStrNameTemplate.GetAt (i)) == 0))
					        break;
			        }
			        if (i > arStrNameTemplate.GetUpperBound ())
				        arStrNameTemplate.Add (strNameTemplate);
			        */

			        //По имени ШАБЛОНа 'strNameTemplate' в БД найдём 'ID'
			        //По имени ШАБЛОНа 'strNameTemplate' в БД НЕЛЬЗЯ НАЙТИ 'ID' !!!
			        //idItem = m_pTreeCtrlTemplate->GetItemID (strNameTemplate);
			        //if (idItem > 0)
			        //Но по (Wildcard) найдём ~ об'ект 'HCheckStateTemplate'
			        //Шаблон ПОЛНого ПУТи к изменившемУСЯ файлу
		            //strFullPath = m_pTreeCtrlTemplate->GetItemFullPath (pCST->GetHTreeItem ());
		            /*strFullPath.Empty ();
		            strFullPath.Append (pDescThreadDirWatched->stringData (pDescThreadDirWatched->GetDataOfId (HThreadDescription::PATH_DIRECTORY_WATCHED)));
		            strFullPath.Append (_T ("\\"));
		            strFullPath.Append (pwchNameTemplate);
		            strFullPath.Append (_T (".*"));*/
    		        		        
		            FREE_PTR (pwchFullPath)                    
		            pwchFullPath = _wcsadd (WC_HSTRING (pwchDirWatched), _T ("\\"));
		            pwchFullPath = _wcsadd (pwchFullPath, pwchNameTemplate, true);
		            pwchFullPath = _wcsadd (pwchFullPath, _T (".*"), true);
    		        
		            //strFullPath = m_pTreeCtrlTemplate->GetItemFullPath (idItem);
		        
#if defined _DEBUG
    MEMORY_LEAKED (_T ("Set pwchFullPath"))
#endif

			        //pCST = m_pTreeCtrlTemplate->GetObjectCheckStateTemplate (m_arDescThreadDirWatched.GetAt (indxThreadDirWatched)->stringData (m_arDescThreadDirWatched.GetAt (indxThreadDirWatched)->GetDataOfNumber (1)) + CString (_T("\\")) + strNameTemplate);
			        pCST = m_pTreeCtrlTemplate->GetObjectCheckStateTemplate (pwchFullPath);
			    
			        //strFullPath.ReleaseBuffer (strFullPath.GetLength ());
			        //strFullPath.Empty ();
			    
#if defined _DEBUG
    MEMORY_LEAKED (_T ("Get 'HObjectCheckStateTemplate'"))
#endif

			        if (pCST) {
				        //По 'ID' найдём ~ об'ект 'HCheckStateTemplate'
				        //pCST = m_pTreeCtrlTemplate->GetObjectCheckStateTemplate (idItem);
				        //В 'pCST' есть 1) 'HTREEITEM'; 2) 'ID_OWNER'
				        //А где проверка 'pCST' на NULL ??? - ВЫШе !!!
				        if (m_pTreeCtrlTemplate->GetSelectedItem () == pCST->GetHTreeItem ()) {
							//ШАБЛОН в котором произошли изменения ОТОБРАЖАЕТСЯ - ПРОБЛЕМЫ !!!
				            //if (pCST->IsUpdate ())
				            //{
					            //strFullPath.Replace (_T ("*"), CString (pwchExt));
					            FREE_PTR (pwchFullPath)
					            pwchFullPath = _wcsadd (WC_HSTRING (pwchDirWatched), _T ("\\"));
		                        pwchFullPath = _wcsadd (pwchFullPath, pwchNameTemplate, true);
		                        pwchFullPath = _wcsadd (pwchFullPath, _T ("."), true);
		                        pwchFullPath = _wcsadd (pwchFullPath, pwchExt, true);
		                    
#if defined _DEBUG
    MEMORY_LEAKED (_T ("Set pwchFullPath"))
#endif
					        
					            //Определим МЕСТО искомого файла в 'm_ListItemFiles'
					            //for (i = 0; i < pCST->GetCountFileFind (); i ++)
					            iItem = pCST->GetPosItemOfExt (pwchExt);
					        
#if defined _DEBUG
    MEMORY_LEAKED (_T ("pCST->GetPosItemOfExt"))
#endif
					            if (iItem > -1) {
						            //Определяем тип произошедшего действия
						            switch (pFNI->Action) {
							            case FILE_ACTION_ADDED: //Файл был создан (1)
							            case FILE_ACTION_MODIFIED: //Файл был изменен (3)
								            filefind.Close ();
								            //if (filefind.FindFile (strFullPath.GetBuffer ()))
								            if (filefind.FindFile (pwchFullPath)) {
									            if (CFile::GetStatus (pwchFullPath, fStatus)) {
										            pCST->SetDTInfoTemplate (iItem, fStatus.m_mtime);
										            pCST->SetState (iItem);
            				                        
										            SetListBoxItemFilesText (pCST, iItem, false);
									            }
									            else
										            ; //Нет информации о статусе файла
								            }
								            else
									            ; //Не найден файл-источник для копирования
								            break;
							            case FILE_ACTION_REMOVED: //Файл был удален (2)
								            pCST->ClearDTInfoTemplate (iItem); // -1 ???
								            pCST->SetState (iItem);
            				                
								            SetListBoxItemFilesText (pCST, iItem, false);
								            break;
							            case FILE_ACTION_RENAMED_OLD_NAME: //Файл был переименован и в имени файла - предыдущее имя (4)
								            //Определим номер РАСШИРЕНИя
								            //iNumExtPrev = _wtoi ((strExt.GetBuffer ()));

								            pCST->ClearDTInfoTemplate (iItem);
								            pCST->SetState (iItem);
            		                        
								            SetListBoxItemFilesText (pCST, iItem, false);

								            lvItem.state = 0;
								            m_ptrListItemFiles->SetItemState (iItem, &lvItem);
								            break;
							            case FILE_ACTION_RENAMED_NEW_NAME: //Новое имя после переименования (5)
								            filefind.Close ();
								            if (filefind.FindFile (pwchFullPath)) {
									            if (CFile::GetStatus (pwchFullPath, fStatus)) {
										            //iNumExtDiff = _wtoi ((strExt.GetBuffer ())) - iNumExtPrev;
										            pCST->SetDTInfoTemplate (iItem, fStatus.m_mtime);
										            pCST->SetState (iItem);
            				                        
										            SetListBoxItemFilesText (pCST, iItem, false);

										            lvItem.state = LVIS_SELECTED;
										            m_ptrListItemFiles->SetItemState (iItem, &lvItem);
									            }
									            else
										            ; //Нет информации о статусе файла
								            }
								            else
									            ; //Не найден файл-источник для копирования
								            break;
							            default:
								            ; //Неизвестная операция
						            }
						        
#if defined _DEBUG
    MEMORY_LEAKED (_T ("pFNI->Action"))
#endif

						            //Время, когда произошло событие
						            GetLocalTime (&sTime);
					            }
					            else
						            ; //Не НАЙДЕН в списке 'm_ListItemFiles'
//                            }
//                            else {
//                                //Ппринудительно обновляем инфрмацию о ШАБЛОНе
//					            pCST->ClearTimeUpdate ();
//					            pCST->Update ();
//					        
//#if defined _DEBUG
//    MEMORY_LEAKED (_T ("pCST->Update () - item"))
//#endif
//                                if (! (m_ptrCheckBoxAutoUpdate->GetCheck () == BST_UNCHECKED)) {
//					                //Структура для посылки сообщения
//					                hdr.code = TVN_SELCHANGED;
//					                hdr.hwndFrom = m_pTreeCtrlTemplate->m_hWnd;
//					                hdr.idFrom = m_pTreeCtrlTemplate->GetDlgCtrlID ();
//        					        
//					                //Сообщаем об изменении
//					                this->SendMessage(WM_NOTIFY, (WPARAM) m_pTreeCtrlTemplate->m_hWnd, (LPARAM) &hdr);
//                                }
//                                else
//                                    ;
//                            }
				        }
				        else {
							//ШАБЛОН в котором произошли изменения НЕ отображается
					        //Тупо (принудительно) обновляем инфрмацию о ШАБЛОНе
					        pCST->ClearTimeUpdate ();
					        pCST->Update ();
					    
#if defined _DEBUG
    MEMORY_LEAKED (_T ("pCST->Update () - container"))
#endif

					        //Структура для посылки сообщения
					        hdr.code = TVN_SELCHANGED;
					        hdr.hwndFrom = m_pTreeCtrlTemplate->m_hWnd;
					        hdr.idFrom = m_pTreeCtrlTemplate->GetDlgCtrlID ();

					        /*
					        //Так коротко, но НЕ ПОНЯТНО
					        if ((m_pTreeCtrlTemplate->GetSelectedItem () == m_pTreeCtrlTemplate->GetParentItem (pCST->GetHTreeItem ()) ||
						        (m_pTreeCtrlTemplate->GetSelectedItem () == m_pTreeCtrlTemplate->GetRootItem ()))
							        this->SendMessage(WM_NOTIFY, (WPARAM) m_pTreeCtrlTemplate->m_hWnd, (LPARAM) &hdr);
					        */

					        //Так ДЛИННЕЙ, но ПОНЯТНО
					        if (m_pTreeCtrlTemplate->GetSelectedItem () == m_pTreeCtrlTemplate->GetParentItem (pCST->GetHTreeItem ())) {
								//КОНТЕЙНЕР для ШАБЛОНа в котором произошли изменения ОТОБРАЖАЕТСЯ
						        //Отсылаем сообщение на ПЕРЕСЕЛЕКТ КОНТЕЙНЕРа для ШАБЛОНа
						        this->SendMessage(WM_NOTIFY, (WPARAM) m_pTreeCtrlTemplate->m_hWnd, (LPARAM) &hdr);
					        }
					        else {
								//КОНТЕЙНЕР для ШАБЛОНа в котором произошли изменения НЕ отображается
						        if (m_pTreeCtrlTemplate->GetSelectedItem () == m_pTreeCtrlTemplate->GetRootItem ()) {
									//ОТОБРАЖАЕТСЯ КОРНЕВАой элемент дерева
							        //Отсылаем сообщение на ПЕРЕСЕЛЕКТ КОРНЕВого элемента дерева
							        this->SendMessage(WM_NOTIFY, (WPARAM) m_pTreeCtrlTemplate->m_hWnd, (LPARAM) &hdr);
						        }
						        else {
									//Вообще НЕТ НЕОБХОДИМОСТИ что-то ДЕЛАТЬ
						        }
					        }
					    
#if defined _DEBUG
    MEMORY_LEAKED (_T ("SendMessage"))
#endif
				        }
			        }
		        }
		        else
		            ; //ИзменниЯ НЕ для этоГО приложения

		        /*
		        //КЛАССИКА
                //Если эта запись не КРАЙНЯЯ (NextEntryOffset < >  0), то...
                if (pFNI->NextEntryOffset == 0) {
			        bWorking = false;
		        }
                else {
                    //Передвигаем указатель на NextEntryOffset байт вперёд
			        //Передвигать надо именно Ptr, а не bytesBuf
                    pFNI = (FILE_NOTIFY_INFORMATION *) (((BYTE *) pFNI) + pFNI->NextEntryOffset);
                    iCountRec ++;
                }
		        */
		    
//#if defined _DEBUG
//    g_memStateEnd.Checkpoint ();
//    if (g_memStateDiff.Difference (g_memStateBegin, g_memStateEnd)) {
//        TRACE ("Memory leaked!\n" );
//        //g_memStateDiff.DumpStatistics();
//        //g_memStateDiff.DumpAllObjectsSince ();
//    }
//    else
//        ;
//#endif
    	
    	        FREE_PTR (pwchNameTemplate)
		        FREE_PTR (pwchExt)
    		    
		        FREE_PTR (pwchFullPath)
    		        		
		        //НЕ КЛАССИКА
		        DWORD iNextEntryOffset = -1;
		        if (pFNI->NextEntryOffset)
			        iNextEntryOffset = pFNI->NextEntryOffset;
		        else
			        iNextEntryOffset = 3 * sizeof (DWORD) + pFNI->FileNameLength;
        		
		        bytesDiff -= iNextEntryOffset;
		        pFNI = (FILE_NOTIFY_INFORMATION *) (((BYTE *) pFNI) + iNextEntryOffset);
    		    
		        /*strLog.format ("'::OnCommandThreadDirWatch...обработано байт: %i", bytes - bytesDiff);
                FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), USER_KTS_DLG);
                strLog.free ();*/
            }
            //КЛАССИКА
	        //while (bWorking);
	        //НЕ КЛАССИКА
	        while (bytesDiff);
    	    
	        //FREE_PTR (pwchDirWatched)
	        pwchDirWatched.free ();
        }
        else
            ; //НЕТ информации о ПОТОКе ???

	    /*
	    for (i = 0; i < arStrNameTemplate.GetSize (); i++)
	    {
	    }
        */
        //delete pLocalData;
	    pFNI = NULL;
    	
	    strLog.free ();
    }
    else
        ; //Число полученных данных НЕ КОРРЕКТНО
	
#if defined _DEBUG
    g_memStateEnd.Checkpoint ();
    if (g_memStateDiff.Difference (g_memStateBegin, g_memStateEnd)) {
        //TRACE ("::OnCommandThreadDirWatch - Выход - Memory leaked!\n" );
        //g_memStateDiff.DumpStatistics();
        //g_memStateDiff.DumpAllObjectsSince ();
    }
    else
        ;
        
    //g_memStateEnd.DumpAllObjectsSince ();
    
    //memStateEnd.Checkpoint ();
    //if (memStateDiff.Difference (memStateBegin, memStateEnd))
    //{
    //    TRACE ("::OnCommandThreadDirWatch - Выход - Memory leaked!\n" );
    //    memStateDiff.DumpStatistics();
    //    //memStateDiff.DumpAllObjectsSince ();
    //}
    //else
    //    ;
    
#endif

	SetEvent (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::DIRECTORY_WATCHED));
    
    return lResult;
}

void CUserKTSDlg::OnBnClickedCommandDirList ()
{
    // TODO: Add your control notification handler code here
    HString strParentDirectory, strSubDirectory;
    BOOL bRes = false;
    FILELOG_VARIABLE
	HDlgChoiceDirectoryTemplate *pDlgChoiceDirectoryTemplate = NULL;
	int iRes = -1, indxThread = -1, iLevel = m_pTreeCtrlTemplate->GetSelectedLevel ();
    if ((iLevel | HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_ROOT) && (iLevel | HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_CONTAINER)) {
		//ОСТАНОВка ВСЕХ ПОТОКов ???
		/*
		switch (WaitForSingleObject (m_hEvtDirectoryWatched, INFINITE))
		{
			case WAIT_ABANDONED:
			case WAIT_TIMEOUT:
				break;
			case WAIT_OBJECT_0:
			    FILELOG_WRITE_WITHDT ("Запрет работы ВСЕМ ПОТОКам...", LEVEL_INFO_2);
				ResetEvent (m_hEvtDirectoryWatched);
			default:
				;
		}
        */
        pDlgChoiceDirectoryTemplate = DEBUG_NEW HDlgChoiceDirectoryTemplate (this);
        TCHAR path [MAX_PATH];
        memset (path, 0x0, MAX_PATH * sizeof (TCHAR));
	    m_ptrStaticFullPath->GetWindowText (path, MAX_PATH);
	    pDlgChoiceDirectoryTemplate->m_strPath.init (path, HString::WCSTR);

        pDlgChoiceDirectoryTemplate->PreCreate (CRect (0, 0, 160, 180), HDLG_MODALES);
		iRes = pDlgChoiceDirectoryTemplate->DoModal ();
        
        if ((iRes == IDOK) && (pDlgChoiceDirectoryTemplate->m_strPath.isUses () == 1)) {
            HThreadDirectoryWatched *pThreadDirWatched = NULL;
        
            if (pDlgChoiceDirectoryTemplate->m_strPath.find (0, _T (":")) < 0)
				//Очистить 'PATH'
				pDlgChoiceDirectoryTemplate->m_strPath = CString (_T (""));
		    else
		        ;
            
            NMHDR hdr;
            hdr.code = TVN_SELCHANGED;
            hdr.hwndFrom = m_pTreeCtrlTemplate->m_hWnd;
            hdr.idFrom = m_pTreeCtrlTemplate->GetDlgCtrlID ();
            
            m_pTreeCtrlTemplate->InitializeRS (_T (TABLENAME_TEMPLATE));
            m_pTreeCtrlTemplate->InitializeRS (_T (TABLENAME_TIME));
        
            switch (iLevel) {
				case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_ROOT:
	                //Завершение (принудительное) ВСЕХ ПОТОКов ???
#if defined _WORK_THREAD
					DestroyThreadsDirectoryWatched ();
#else
                    DestroyUIThreadsDirectoryWatched ();
#endif
					
					m_ptrStaticFullPath->SetWindowText (pDlgChoiceDirectoryTemplate->m_strPath.GetBuffer ());
					//НЕОБХОДИМО ПРОВЕРИТь значение, возвращаемое 'SetSelectedItemPATH' ???
	                m_pTreeCtrlTemplate->SetSelectedItemPATH (pDlgChoiceDirectoryTemplate->m_strPath);
		            
		            //Проверить на корректность 'PATH' дочерних записей ???
		            //bRes = PathIsDirectory (pDlgChoiceDirectoryTemplate->m_strPath);
	            
		            m_pTreeCtrlTemplate->ReInitializeItems (m_pTreeCtrlTemplate->GetSelectedItem ());

					//ПЕРЕЗАПУСК ВСЕХ ПОТОКов с НОВЫМи НАСТРОЙКАми ???
					if (!
#if defined _WORK_THREAD
					    CreateThreadsDirectoryWatched
#else
                        CreateUIThreadsDirectoryWatched
#endif
					    ())
						; //ПОТОКи запустить НЕ УДАЛОСЬ ???
			        else
			            ;
		            
		            this->SendMessage(WM_NOTIFY, (WPARAM) m_pTreeCtrlTemplate->m_hWnd, (LPARAM) &hdr);
		            break;
	            case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_CONTAINER:
					//Завершение (принудительное) ОДНОГо ПОТОКа ???
					for (indxThread = 0; indxThread < m_arPtrThreadDirWatched.GetSize (); indxThread ++) {
						//if (m_pTreeCtrlTemplate->GetSelectedItem () == * ((HTREEITEM *) m_arPtrThreadDirWatched.GetAt (indxThread)->GetDataOfNumber (HThreadDirectoryWatched::HTREEITEM_TEMPLATE)))
						if (m_pTreeCtrlTemplate->GetSelectedItem () == * ((HTREEITEM *) (m_arPtrThreadDirWatched.GetAt (indxThread))->GetDataOfId (HThreadDirectoryWatched::HTREEITEM_TEMPLATE))) {
							pThreadDirWatched = (HThreadDirectoryWatched *) m_arPtrThreadDirWatched.GetAt (indxThread);
							if (! GetExitCodeThread (pThreadDirWatched->m_hThread, &pThreadDirWatched->m_idExitCodeThread))
								iRes = GetLastError ();
							else {
								if (pThreadDirWatched->m_idExitCodeThread == STILL_ACTIVE) {
                                    //TerminateHThread (pThreadDirWatched);
									((HWinApp *) AfxGetApp ())->TerminateHThread ((HThreadDirectoryWatched *) pThreadDirWatched);
								}
								else
								    ;
					
								//Удаляются НЕ ВСЕ данные ???
								//pThreadDirWatched->DeleteData ();
							}
							break;
						}
					}

					if (pThreadDirWatched) {
					    if (pDlgChoiceDirectoryTemplate->m_strPath.isUses () == 0) {
                            //Если ПУСТая БЕЗУСЛОВНо установим ЗНАЧЕНие
					        //НЕОБХОДИМО ПРОВЕРИТь значение, возвращаемое 'SetSelectedItemPATH' ???
						    m_pTreeCtrlTemplate->SetSelectedItemPATH (pDlgChoiceDirectoryTemplate->m_strPath);
						    m_ptrStaticFullPath->SetWindowText (pDlgChoiceDirectoryTemplate->m_strPath.GetBuffer ());
    		                
						    m_pTreeCtrlTemplate->ReInitializeItems (m_pTreeCtrlTemplate->GetSelectedItem ());
					    }
					    else {
                            //Если НЕ ПУСТая ПРОВЕРяем ДОПОЛНительно
						    if (pDlgChoiceDirectoryTemplate->m_strPath.find (0, m_pTreeCtrlTemplate->GetItemFullPath (m_pTreeCtrlTemplate->GetParentItem (m_pTreeCtrlTemplate->GetSelectedItem ()))) > -1) {
							    //Изменение в БД записи текущего БЛОКа
							    strParentDirectory = pDlgChoiceDirectoryTemplate->m_strPath.left (pDlgChoiceDirectoryTemplate->m_strPath.reverseFind ('\\')),
                                strSubDirectory = pDlgChoiceDirectoryTemplate->m_strPath.right (pDlgChoiceDirectoryTemplate->m_strPath.length () - pDlgChoiceDirectoryTemplate->m_strPath.reverseFind ('\\') - 1);
							    if (strParentDirectory.compareNoCase (m_pTreeCtrlTemplate->GetItemPATH (m_pTreeCtrlTemplate->GetParentItem (m_pTreeCtrlTemplate->GetSelectedItem ()))) == 0) {
								    //НЕОБХОДИМО ПРОВЕРИТь значение, возвращаемое 'SetSelectedItemPATH' ???
								    m_pTreeCtrlTemplate->SetSelectedItemPATH (strSubDirectory);
								    m_ptrStaticFullPath->SetWindowText (pDlgChoiceDirectoryTemplate->m_strPath.GetBuffer ());
    				                
								    m_pTreeCtrlTemplate->ReInitializeItems (m_pTreeCtrlTemplate->GetSelectedItem ());

								    //m_pTreeCtrlTemplate->SelectItem (m_pTreeCtrlTemplate->GetSelectedItem ());
								    /*
								    NMTREEVIEW tv;
								    tv.hdr.code = TVN_SELCHANGED;
								    tv.hdr.idFrom = m_pTreeCtrlTemplate->GetDlgCtrlID ();
								    tv.hdr.hwndFrom = m_pTreeCtrlTemplate->m_hWnd;
    	                            
								    TVITEM tvItem;
								    m_pTreeCtrlTemplate->GetItem (&tvItem);
								    tv.itemNew = tvItem;
								    tv.ptDrag = CPoint (0, 0);
								    this->SendMessage(WM_NOTIFY, (WPARAM) m_pTreeCtrlTemplate->m_hWnd, (LPARAM) &tv);
								    */
    	                            
								    this->SendMessage(WM_NOTIFY, (WPARAM) m_pTreeCtrlTemplate->m_hWnd, (LPARAM) &hdr);
							    }
							    else
								    MessageBox (CString (_T("Выбранный каталог является дочерней директорией для '")) + m_pTreeCtrlTemplate->GetItemText (m_pTreeCtrlTemplate->GetSelectedItem ()) + CString (_T("'!")), _T("Внимание!"), MB_ICONINFORMATION); //???
						    }
						    else {
							    MessageBox (CString (_T("Выбранный каталог не в директории для '")) + m_pTreeCtrlTemplate->GetItemText (m_pTreeCtrlTemplate->GetParentItem (m_pTreeCtrlTemplate->GetSelectedItem ())) + CString (_T("'!")), _T("Внимание!"), MB_ICONINFORMATION);
						    }
					    }

					    //ПЕРЕЗАПУСК ОДНОГо ПОТОКа с НОВЫМи НАСТРОЙКАми ???
					    //if (m_pTreeCtrlTemplate->GetSelectedItem () == * ((HTREEITEM *) m_arPtrThreadDirWatched.GetAt (indxThread)->GetDataOfNumber (HThreadDirectoryWatched::HTREEITEM_TEMPLATE)))
					    if (m_pTreeCtrlTemplate->GetSelectedItem () == * ((HTREEITEM *) pThreadDirWatched->GetDataOfId (HThreadDirectoryWatched::HTREEITEM_TEMPLATE))) {
						    pThreadDirWatched->AddData (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED, m_pTreeCtrlTemplate->GetItemFullPath (* ((HTREEITEM *) pThreadDirWatched->GetDataOfId (HThreadDirectoryWatched::HTREEITEM_TEMPLATE))));
						    //m_arPtrThreadDirWatched.GetAt (indxThread).strPath = ((CString) m_pTreeCtrlTemplate->GetItemFullPath (m_pTreeCtrlTemplate->GetSelectedItem ());
					    }
                        else
                            ;

#if defined _WORK_THREAD
                        pThreadDirWatched->m_hThread = CreateThread (NULL, 0, fThreadDirectoryWatched, m_arPtrThreadDirWatched.GetAt (indxThread), 0, &pThreadDirWatched->m_nThreadID);
                        
                        if (! (pThreadDirWatched->m_hThread == NULL))
#else					    
					    if (pThreadDirWatched->CreateThread ()) //CREATE_SUSPENDED
#endif
                        {
                            strLog.format (_T ("ПЕРЕЗАПУСК ПОТОКа: %i, %S"), pThreadDirWatched->m_nThreadID, WC_HSTRING (pThreadDirWatched->stringData (pThreadDirWatched->GetDataOfId (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED))));
                            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
							::PostThreadMessage (pThreadDirWatched->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);
                        }
                        else
                            iRes = GetLastError ();
                    }
                    else
                        ; //ПОТОК для ... НЕ найден ???
		            break;
	            default: ;
            }

			//Изменил НАСТРОЙКи - ОСТАНОВИ и ПЕРЕЗАПУСТи ~ ПОТОК ???
			m_pTreeCtrlTemplate->ReleaseRS (_T (TABLENAME_TEMPLATE));
			m_pTreeCtrlTemplate->ReleaseRS (_T (TABLENAME_TIME));
        }
        
        //Возобновление работы ВСЕХ ПОТОКов ???
		/*
        FILELOG_WRITE_WITHDT ("Разрешение работы ВСЕМ ПОТОКам...", LEVEL_INFO_2);
		SetEvent (m_hEvtDirectoryWatched);
		*/
	}
	else
	    MessageBox (_T ("Каталог можно назначить ТОЛЬКО записям с более ВЫСОКИМ уровнем!"), _T("Ошибка!"), MB_ICONINFORMATION);
	    
	if (! pDlgChoiceDirectoryTemplate == NULL)
		delete pDlgChoiceDirectoryTemplate;
	//m_ptrStaticFullPath->SetCueBanner (_T("1234567890123456"));
}

void CUserKTSDlg::OnTvnSelchangedTemplate(NMHDR *pNMHDR, LRESULT *pResult) {
    GetDlgItem (IDC_STATIC_FULLPATH)->SetWindowText (_T (""));

    HDlgKTSFiles::OnTvnSelchangedTemplate (pNMHDR, pResult);
    
    m_ptrStaticFullPath->SetWindowText (m_pTreeCtrlTemplate->GetSelectedItemFullPath ().GetBuffer ());
}

int CUserKTSDlg::InitializeRS (TCHAR *nameRS) {
    FILELOG_VARIABLE
    int iRes = 0;
    void *pRefRecordset = NULL;
    
    m_pmapDBRS.Lookup (nameRS, pRefRecordset);

    if (pRefRecordset) {
        //ERROR - ПОВТОРная попытка инициализации ???
        strLog.format (_T ("Попытка ПОВТОРно инициализировать ТАБЛицу=%S"), WC_HSTRING (HString (nameRS)));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
    }
    else
        if (PTR_DB) {
            pRefRecordset = PTR_DB->GetRecordset (this, HString (nameRS));
            if (pRefRecordset)
                m_pmapDBRS.SetAt (nameRS, pRefRecordset);
            else
                iRes = -1;
        }
        else
            iRes = -1;

    return iRes;
}

int CUserKTSDlg::ReleaseRS (TCHAR *nameRS, BOOL bClose, BOOL bDelete) {
    PTR_DB->ReleaseRS (this, HRECORDSET (m_pmapDBRS [nameRS]), bClose, bDelete);
    m_pmapDBRS.RemoveKey (nameRS);
        
    return 0;
}

//int SeekRSTemplateById (int id)
//{
//    int iRes = 0;
//
//    return iRes;
//}

//CTime CUserKTSDlg::GetOptionTime ()
//{
//}

void CUserKTSDlg::OnAddTemplate (void) {
    // TODO: Add your command handler code here
    m_pTreeCtrlTemplate->SendMessage (WM_COMMAND, IDM_TEMPLATE_ADD);
}

void CUserKTSDlg::OnChangeTemplate (void) {
    // TODO: Add your command handler code here
    m_pTreeCtrlTemplate->SendMessage (WM_COMMAND, IDM_TEMPLATE_CHANGE);
}

void CUserKTSDlg::OnDelTemplate () {
    // TODO: Add your command handler code here
    m_pTreeCtrlTemplate->SendMessage (WM_COMMAND, IDM_TEMPLATE_DEL);
}

void CUserKTSDlg::OperationCreate (HCheckStateTemplate *pCST, short shNumBeginSource) {
#ifdef _DEBUG
    //g_memStateEnd.Checkpoint ();
    //if (g_memStateDiff.Difference (g_memStateBegin, g_memStateEnd))
    //{
    //    g_memStateDiff.DumpAllObjectsSince ();
    //    g_memStateDiff.DumpStatistics ();
    //}
    //else
    //    ;
#endif

    FILELOG_VARIABLE
	strLog.format (_T ("Вход в 'CUserKTSDlg::OperationCreate' с аргументом: объект шаблона с ID = %i"), pCST->GetID ());
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
    
    int iRes = 0,
        idParentItem = -1, i = 0, iPrevSuspendCount;

    HString strConnectDB, strTypeSource, arstrExtSelectedItem, strSubItem;
    //HThreadDescription *pDescThreadCreateTemplate = NULL;
    HThreadCreateTemplate *pThreadCreateTemplate = NULL;
    HContentTemplate *pContentTemplate = NULL;

    //CProgressCtrl *pProgressCtrl = NULL;
    int *arPosSelectedItem = NULL, iIdProgressCtrl = -1;
    
    //Для динамического 'm_pProgressCtrl'
    /*CRect rcDlg, rcDlgClient, rcRelativeCtrl, rcProgressCtrl;
    DWORD styleProgressCtrl = 0;*/
        
    //CRecordset *pRS = NULL;
    
    //_Отладка
    /*
    for (int i = 0; i < 3; i ++)
    {
        InitializeRS (_T ("TEMPLATE_0070"));
        pRS = (CRecordset *) m_pmapDBRS [_T ("TEMPLATE_0070")];
        ReleaseRS (_T ("TEMPLATE_0070"), true, true);
    }
    */  
    
    /*CString strLog;
    strLog.Format (_T ("'WaitFor...m_hEvtOperation' для 'CUserKTSDlg::OperationCreate'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_3);*/
      
  //  switch (WaitForSingleObject (pCST->m_hEvtOperation, 0x0))
  //  {
  //      case WAIT_ABANDONED:
		//case WAIT_TIMEOUT:
		//    //По ЭТОМу ШАБЛОНу всё ещё продолжается РАБОТа
		//    MessageBox (_T ("Повторите попытку после окончания текущей операции!"), _T ("Внимание!"), MB_ICONINFORMATION);
		//	break;
		//case WAIT_OBJECT_0:
		    /*ResetEvent (pCST->m_hEvtOperation);*/
		    //ВСя  "карусель" с 'HContentTemplate' из-за 'As stated by the MFC 4.2 documentation, the Dao database classes are not thread-safe, and that is indeed true'
		    //pContentTemplate = new HContentTemplate ();
		    //pContentTemplate->GetContentTemplate (pCST->GetID ());
            pThreadCreateTemplate = DEBUG_NEW HThreadCreateTemplate ();
            //pThreadCreateTemplate->GetContentTeplate (pContentTemplate->CopyContentTemplate ());
            //delete pContentTemplate;
            
            if (pThreadCreateTemplate->CreateThread (CREATE_SUSPENDED)) {
                //Инициализирован в КОНСТРУКТОРе
                //pThreadCreateTemplate->m_bAutoDelete = false;
                //pDescThreadCreateTemplate->m_idExitCodeThread = WAIT_IO_COMPLETION;
                
                strLog.format (_T ("Добавляем параметрЫ для потока ID = %i"), pThreadCreateTemplate->m_nThreadID);
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);

                //pDescThreadCreateTemplate->AddData (HThreadDescription::PTR_TREECTRLTEMPLATE, &m_pTreeCtrlTemplate, sizeof (HTreeCtrlTemplate *));
                ((HWinThread *) pThreadCreateTemplate)->AddData (HThreadCreateTemplate::PTR_CHECKSTATETEPMLATE, &pCST, sizeof (HCheckStateTemplate *));
                
				strLog.format (_T ("Параметр для потока 'PTR_CHECKSTATETEPMLATE': объект шаблона с ID = %i"), pCST->GetID ());
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);

                ITEM_DATA *pItemData = ((ITEM_DATA *) m_pTreeCtrlTemplate->GetItemData (m_pTreeCtrlTemplate->GetSelectedItem ()));
                BOOL bAutoCreate = false;
                int iCountItemCreate = -1;
                
                //if (m_iModeApp & ID_MODE_APP::MODE_LOCAL) {
                //    if (pItemData->auto_create)
                //        bAutoCreate = true;
                //    else
                //        ;
                //}
                //else {
                //    if (m_iModeApp & ID_MODE_APP::MODE_REMOTE) {
                //    }
                //    else
                //        ;
                //}

                if ((MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_DISABLED_AUTOCREATE) == 0) {
                    strLog.format (_T ("АвтоСоздание файла НЕ отключено; MODE_APP = %i"), MODE_APP);
                    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG); //USER_KTS_DLG
#if defined _RELEASE
                    if (pItemData->auto_create && (MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_LOCAL))
#else
                    if (pItemData->auto_create)
#endif
                        bAutoCreate = TRUE;
                    else
                        ;
                } else {
                    strLog.format (_T ("АвтоСоздание файла отключено; MODE_APP = %i"), MODE_APP);
                    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG); //USER_KTS_DLG
                }

                strLog.format (_T ("АвтоСоздание файла = %i"), bAutoCreate);
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG); //USER_KTS_DLG

                if (bAutoCreate/* == TRUE*/) {//АВТОМАТическое формирование файлов - необходимО АВТОМАТисески Же сформировать m_arPosSelectedItem
                    arPosSelectedItem = pCST->FillArrayPseudoPosSelectedItem (iCountItemCreate);
                    iRes = pCST->FillArrayStringPseudoSelectedItem (arstrExtSelectedItem);
                }
                else
                {//Формирование по ЗАПРОСу
                    //Д.Б. ОСВОБОЖДЕНы в ПОТОКе
                    iCountItemCreate = m_arPosSelectedItem.GetSize ();
                    
                    arPosSelectedItem = DEBUG_NEW int [iCountItemCreate + 1];
                    memset (arPosSelectedItem, 0x0, (sizeof (int) * (iCountItemCreate + 1)));
                }

                strLog.format (_T ("Подготовка массива элементов списка: %i, с размерОМ: %i"), arPosSelectedItem, iCountItemCreate);
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG); //USER_KTS_DLG

                if (! bAutoCreate) {
                    for (i = 0; i < iCountItemCreate; i++)
                    {
                        arPosSelectedItem [i] = (int) m_arPosSelectedItem.GetAt (i);

                        arstrExtSelectedItem.append (m_ptrListItemFiles->GetItemText ((int) (arPosSelectedItem [i] - 1), 3).GetBuffer ());
                        if ((i + 1) <  iCountItemCreate)
                            arstrExtSelectedItem.appendChar (';');
                    }
                }
                else
                    ;

                strLog.format (_T ("Подготовка параметрОВ для потока 'PTR_ARPOSSELECTEDITEM, STRING_AREXTSELECTEDITEM' - количество: %i"), iCountItemCreate);
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);

                (DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplate))->AddData (HThreadCreateTemplate::PTR_ARPOSSELECTEDITEM, arPosSelectedItem, sizeof (int) * (iCountItemCreate + 1));

                strLog.format (_T ("ПередаН параметР для потока 'PTR_ARPOSSELECTEDITEM'"));
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);

                delete arPosSelectedItem;
                arPosSelectedItem = NULL;

                strLog.format (_T ("УдалЁн параметР для потока 'PTR_ARPOSSELECTEDITEM'"));
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);

                (DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplate))->AddData (HThreadCreateTemplate::STRING_AREXTSELECTEDITEM, arstrExtSelectedItem);
                arstrExtSelectedItem.free ();

                strLog.format (_T ("ПередаН параметР для потока 'STRING_AREXTSELECTEDITEM'"));
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);

                //shNumBeginSource, SHORT_NUMBERBEGINSOURCE
                (DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplate))->AddData (HThreadCreateTemplate::SHORT_NUMBERBEGINSOURCE, &shNumBeginSource, sizeof (short));

                //Надо ОПРЕДЕЛиться с РОДИТЕЛями выбраннОГО ШАБЛОНа для получения строк соединения с БД (strConnectDB) и типа источника данных (strTypeSource)
                //idParentItem = m_pTreeCtrlTemplate->GetItemID (m_pTreeCtrlTemplate->GetParentItem (m_pTreeCtrlTemplate->GetSelectedItem ()));
                idParentItem = m_pTreeCtrlTemplate->GetItemID (m_pTreeCtrlTemplate->GetParentItem (pCST->GetHTreeItem ()));

                strTypeSource = PTR_SETTINGS->CompileTypeSource (idParentItem, HDBSettings::SQL_DB | HDBSettings::FILES_DB);

                strLog.format (_T ("Параметр для потока 'STRING_TYPESOURCE': %S"), WC_HSTRING (strTypeSource));
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
                
                (DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplate))->AddData (HThreadCreateTemplate::STRING_TYPESOURCE, strTypeSource);
                
                strLog.format (_T ("%S"), _T ("ПередаН параметР для потока 'STRING_TYPESOURCE'"));
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);                
                
				strConnectDB = PTR_SETTINGS->CompileConnectDB (idParentItem, HDBSettings::SQL_DB | HDBSettings::FILES_DB);
                
                strLog.format (_T ("Параметр для потока 'STRING_CONNECTDB': %S"), WC_HSTRING (strConnectDB));
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
                
                (DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplate))->AddData (HThreadCreateTemplate::STRING_CONNECTDB, strConnectDB);
                strConnectDB.free ();
                strTypeSource.free ();
                //ReleaseRS (_T ("Source"), true, true);
                
                iRes = m_ptrComboBoxOperation->GetItemData (m_ptrComboBoxOperation->GetCurSel ());
                (DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplate))->AddData (HThreadCreateTemplate::NUMBER_OPERATION, &iRes, sizeof (DWORD_PTR));
                (DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplate))->AddData (HThreadCreateTemplate::STRING_DIRECTORYUNDO, UNDOCLEAR_PATH);
                
                InitializeRS (_T (TABLENAME_TEMPLATE));
                
                //Для ПРОБы открыл 2 'RecordSet' = success !!!
                //m_pTreeCtrlTemplate->InitializeRS (_T (TABLENAME_TEMPLATE));
                //m_pTreeCtrlTemplate->ReleaseRS (_T (TABLENAME_TEMPLATE));
                
                TIMESTAMP_STRUCT tmstmp_st;
                BOOL bAutoChangeSource = false;
                if (! (HRECORDSET (m_pmapDBRS [_T (TABLENAME_TEMPLATE)]) == 0x0)) {
                    //if ((RECORDSET (m_pmapDBRS [_T (TABLENAME_TEMPLATE)]))->SeekRSTemplateToItem (m_pTreeCtrlTemplate->GetItemID (m_pTreeCtrlTemplate->GetSelectedItem ())))
                    if ((HRECORDSET (m_pmapDBRS [_T (TABLENAME_TEMPLATE)]))->SeekRSTemplateToItem (pCST->GetID ())) {
                        (HRECORDSET (m_pmapDBRS [_T (TABLENAME_TEMPLATE)]))->GetFieldValue (_T ("INTERVAL_DATA"), tmstmp_st);
                        LONGLONG llSpanData = tmstmp_st.hour * 60 * 60 + tmstmp_st.minute * 60 + tmstmp_st.second;
                        (DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplate))->AddData (HThreadCreateTemplate::SPAN_INTERVALDATA, &llSpanData, sizeof (LONGLONG));
                    
                        if (PTR_SETTINGS->IsAlternativeSource (idParentItem))
                            (HRECORDSET (m_pmapDBRS [_T (TABLENAME_TEMPLATE)]))->GetFieldValue (_T ("AUTO_CHANGE_SOURCE"), bAutoChangeSource);
                        else
                            ;
                        
                        (DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplate))->AddData (HThreadCreateTemplate::BOOL_AUTOCAHNGESOURCE, &bAutoChangeSource, sizeof (BOOL));
                    }
                    else
                        ;
                
                    ReleaseRS (_T (TABLENAME_TEMPLATE), true, true);
                
                    iIdProgressCtrl = MakeProgressCtrl ();
                    (DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplate))->AddData (HThreadCreateTemplate::NUMBER_IDPROGRESSBAR, &iIdProgressCtrl, sizeof (int));

                    //Добавление в массив (список) действующих ПОТОКов 
                    //m_arPtrDescThreadCreateTemplate.Add (pDescThreadCreateTemplate);
                    //m_arPtrThreadCreateTemplate.Add (pThreadCreateTemplate);
				    m_mapPtrThread.SetAt (pThreadCreateTemplate->m_nThreadID, pThreadCreateTemplate);
                
                    //Запрет обновления информации по шаблону
                    //pCST->m_evCreateTemplate.ResetEvent ();
                    //По хорошему освободить ШАБЛОН после ВОЗОБНОВЛЕНия ПОТОКа ??? НЕ ПОСЛЕ ВОЗОБНОВЛЕНИЯ, НО ПОСЛЕ ЗАВЕРШЕНИЯ !!!
                    //SetEvent (pCST->m_hEvtOperation);

                    iPrevSuspendCount = pThreadCreateTemplate->ResumeThread ();
                    if (iPrevSuspendCount == 0xFFFFFFFF) {
	                    iRes = GetLastError ();
	                    PostMessage (WM_USER_EXIT_CREATE_TEMPLATE, NULL, pThreadCreateTemplate->m_nThreadID);
            	    
	                    //return false;
	                    return ;
                    }
				    else {
					    ::PostThreadMessage (pThreadCreateTemplate->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);
                    }
                }
                else {
                    strLog.format (_T ("Не инициализировали RS для таблицы: %S"), _T (TABLENAME_TEMPLATE));
                    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);

                    //PostMessage (WM_USER_EXIT_CREATE_TEMPLATE, NULL, pThreadCreateTemplate->m_nThreadID);
                    delete pThreadCreateTemplate;
                }
            }
            else
                ; //ПОТОК не создан ???
            /*break;
		default:
			;
    }*/
    
    strConnectDB.free (); strTypeSource.free (); arstrExtSelectedItem.free (), strSubItem.free ();
    
#ifdef _DEBUG
    //g_memStateEnd.Checkpoint ();
    //if (g_memStateDiff.Difference (g_memStateBegin, g_memStateEnd))
    //{
    //    g_memStateDiff.DumpAllObjectsSince ();
    //    g_memStateDiff.DumpStatistics ();
    //}
    //else
    //    ;
#endif
    
    strLog.format (_T ("%S"), _T ("Выход из 'CUserKTSDlg::OperationCreate'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
}

int CUserKTSDlg::MakeProgressCtrl (void) {
    //Для динамического 'm_pProgressCtrl'
    CProgressCtrl *pProgressCtrl = NULL;
    CRect rcDlg, rcDlgClient, rcRelativeCtrl, rcProgressCtrl;
    DWORD styleProgressCtrl = 0;

    //Увеличиваю размер окна (правая (нижняя) граница)
    GetWindowRect (&rcDlg);
    GetClientRect (&rcDlgClient);
    ClientToScreen (&rcDlgClient);

    rcDlg.bottom = rcDlg.bottom + HEIGHT_PROGRESSCTRL + 1 * OFFSET_CONTROL;
    this->MoveWindow(rcDlg.left, rcDlg.top, rcDlg.Width (), rcDlg.Height (), true);
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_TIME_CURRENT);
    
    pProgressCtrl = DEBUG_NEW CProgressCtrl ();
    
    styleProgressCtrl = WS_BORDER | WS_CHILD | WS_OVERLAPPED | WS_CLIPSIBLINGS | WS_VISIBLE | PBS_SMOOTH;
    
    m_ptrStatusBar->GetWindowRect (&rcRelativeCtrl);
    rcProgressCtrl.top = rcRelativeCtrl.top - 1 * (OFFSET_CONTROL + HEIGHT_PROGRESSCTRL); rcProgressCtrl.bottom = rcProgressCtrl.top + HEIGHT_PROGRESSCTRL;
    rcProgressCtrl.left = rcDlgClient.left + OFFSET_CONTROL; rcProgressCtrl.right = rcDlgClient.right - OFFSET_CONTROL;
    ScreenToClient (&rcProgressCtrl);

    pProgressCtrl->Create (styleProgressCtrl, rcProgressCtrl, this, IDC_PROGRESS_CTRL + m_arpProgressCtrl.GetSize ());
    m_arpProgressCtrl.Add (pProgressCtrl);
    
    return pProgressCtrl->GetDlgCtrlID ();
}

afx_msg LRESULT CUserKTSDlg::OnMakeProgressCtrl (WPARAM wp, LPARAM) {
    int idProgressCtrl = MakeProgressCtrl ();

    ::PostThreadMessage (wp, WM_USER_THREAD_RESUME, (WPARAM) GetSafeHwnd (), (LPARAM) idProgressCtrl);

    return 0;
}

int CUserKTSDlg::RemoveProgressCtrl (int idProgressCtrl)
{
    int i, iRes = 0; //Success
    CProgressCtrl *pProgressCtrl = NULL;
    FILELOG_VARIABLE
    
    if (m_arpProgressCtrl.GetSize ())
    {
        //Уменьшаю размер окна (правая граница)
        //удаляю динамический 'ProgressCtrl'
        //Для динамического 'm_pProgressCtrl'        
        //НайдЁм соответствующий завершившемуся потоку
        for (i = 0; i < m_arpProgressCtrl.GetSize (); i ++)
        {
            if (m_arpProgressCtrl.GetAt (i)->GetDlgCtrlID () == idProgressCtrl)
            {
                pProgressCtrl = m_arpProgressCtrl.GetAt (i);
                break;
            }
            else
                ;
        }

        if (pProgressCtrl)
        {
            strLog.format (_T ("Найден 'ProgressBar' для завершившегося ПОТОКа IDCTRL = %i"), idProgressCtrl);
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
        
            CRect rcDlg;
            GetWindowRect (&rcDlg);
            
            rcDlg.bottom = rcDlg.bottom - HEIGHT_PROGRESSCTRL - OFFSET_CONTROL;
            MoveWindow (rcDlg.left, rcDlg.top, rcDlg.Width (), rcDlg.Height (), true);
            
            pProgressCtrl->DestroyWindow ();
            delete pProgressCtrl;
            pProgressCtrl = NULL;
            
            m_arpProgressCtrl.RemoveAt (i);
            
            strLog.format (_T ("%S"), _T ("Удалён 'ProgressBar' для завершившегося ПОТОКа ..."));
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
            
            //Репозиционирование 'StatusBar'
            RepositionBars (AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_TIME_CURRENT);
            
            CRect rcDlgClient, rcRelativeCtrl, rcProgressCtrl;
            
            GetClientRect (&rcDlgClient);
            ClientToScreen (&rcDlgClient);
            
            m_ptrStatusBar->GetWindowRect (&rcRelativeCtrl);
            
            //Репозиционирование остальных 'ProgressBar', если ЕСТь
            for (i = 0; i < m_arpProgressCtrl.GetSize (); i ++) {
                pProgressCtrl = m_arpProgressCtrl.GetAt (i);
                
                rcProgressCtrl.top = rcRelativeCtrl.top - (m_arpProgressCtrl.GetSize () - i) * (OFFSET_CONTROL + HEIGHT_PROGRESSCTRL); rcProgressCtrl.bottom = rcProgressCtrl.top + HEIGHT_PROGRESSCTRL;
                rcProgressCtrl.left = rcDlgClient.left + OFFSET_CONTROL; rcProgressCtrl.right = rcDlgClient.right - OFFSET_CONTROL;
                ScreenToClient (&rcProgressCtrl);
                
                pProgressCtrl->MoveWindow(rcProgressCtrl.left, rcProgressCtrl.top, rcProgressCtrl.Width (), rcProgressCtrl.Height (), true);
                
                strLog.format (_T ("Репозиционирование 'ProgressBar' для АКТИВного ПОТОКа с IDCTRL=%i"), pProgressCtrl->GetDlgCtrlID ());
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
            }
        }
        else
        {
            //Не найден 'ProgressCtrl', ЗАВЕРШИВШегося ПОТОКа, который об этом уведомил САМ ???
            strLog.format (_T ("%S"), _T ("НЕ найден 'ProgressCtrl' ПОТОКа ..."));
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
        }
    }
    else
    {
        //Нет ни ОДНого ПОТОКа
        strLog.format (_T ("%S"), _T ("Нет ни ОДНого 'ProgressCtrl-а' ..."));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
    }
    
    return iRes;
}

int CUserKTSDlg::CreateUIThreadsDirectoryWatched (void) {
    int iRes = 0; //Success
 
    FILELOG_VARIABLE
    strLog.format (_T ("%S"), _T ("Вход в 'CUserKTSDlg::CreateUIThreadsDirectoryWatched' без аргументОВ"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
    
    HThreadDirectoryWatched *pThreadDirWatched = NULL;
    HTREEITEM hTreeItem = m_pTreeCtrlTemplate->GetChildItem (m_pTreeCtrlTemplate->GetRootItem ());

	while (hTreeItem) {
        pThreadDirWatched = DEBUG_NEW HThreadDirectoryWatched ();
        
        if (pThreadDirWatched->CreateThread (CREATE_SUSPENDED)) {
            strLog.format (_T ("Добавляем параметрЫ для потока ID=%i"), pThreadDirWatched->m_nThreadID);
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
            strLog.free ();
            
            pThreadDirWatched->AddData (HThreadDirectoryWatched::HTREEITEM_TEMPLATE, &hTreeItem, sizeof (HTREEITEM));
            //pThreadDirWatched->AddData (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED, m_pTreeCtrlTemplate->GetItemFullPath (* ((HTREEITEM *) pThreadDirWatched->GetDataOfId (HThreadDirectoryWatched::HTREEITEM_TEMPLATE))));
            pThreadDirWatched->AddData (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED, m_pTreeCtrlTemplate->GetItemFullPath (hTreeItem));

			pThreadDirWatched->AddData (HThreadDirectoryWatched::HANDLE_EVENT_DIRECTORY_WATCHED, &APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::DIRECTORY_WATCHED), sizeof (DWORD));

			HWND hThisWnd = GetSafeHwnd ();
			pThreadDirWatched->AddData (HThreadDirectoryWatched::HWND_RESPONSE, &hThisWnd, sizeof (HANDLE));
            
            iRes = pThreadDirWatched->ResumeThread ();
            if (iRes == 0xFFFFFFFF) {
                iRes = GetLastError ();
                PostMessage (WM_USER_LEAVE_DIRECTORY_WATCHED, NULL, pThreadDirWatched->m_nThreadID);
        	    
                strLog.format (_T ("Не выполняется RUN потока ID=%i"), pThreadDirWatched->m_nThreadID);
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
                strLog.free ();
            }
            else
				::PostThreadMessage (pThreadDirWatched->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);
                
            m_arPtrThreadDirWatched.Add (pThreadDirWatched);
		
            strLog.format (_T ("Старт ПОТОКа: %i, %S"), pThreadDirWatched->m_nThreadID, WC_HSTRING (pThreadDirWatched->stringData (pThreadDirWatched->GetDataOfId (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED))));
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
            strLog.free ();
            		
		    hTreeItem = m_pTreeCtrlTemplate->GetNextItem (hTreeItem, TVGN_NEXT);
        }
        else
            ; //ПОТОК не создан ???
    } //окончание блока 'while'

    strLog.format (_T ("%S"), _T ("Выход из 'CUserKTSDlg::CreateUIThreadsDirectoryWatched'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);
    
    return iRes;
}

int CUserKTSDlg::TerminateHThreadCreateTemplate (HThreadCreateTemplate *pThreadCreateTemplate) {
	int iRes = 0; //Success	
	FILELOG_VARIABLE

	iRes = RemoveProgressCtrl (*(int *) pThreadCreateTemplate->GetDataOfId (HThreadCreateTemplate::ID_DATAITEM::NUMBER_IDPROGRESSBAR));

	HCheckStateTemplate *pCST = NULL;
	pCST = * (HCheckStateTemplate **) ((HThreadCreateTemplate *) pThreadCreateTemplate)->GetDataOfId (HThreadCreateTemplate::PTR_CHECKSTATETEPMLATE);
		
	strLog.format (_T ("'TerminateHThread' - ПОТОК 'HThreadCreateTemplate' с ID = %i, pCST->ID = "), pThreadCreateTemplate->m_nThreadID);

	if (pCST) {
		strLog.appendNumber ((DWORD) pCST->GetID ());
		strLog.append (_T ("..."));

		//Здесь можно проверить установлен ЛИ 'm_hEvtOperation'
		switch (WaitForSingleObject (pCST->m_hEvtOperation, 0x0)) {
			case WAIT_ABANDONED:
			case WAIT_TIMEOUT:
				if (SetEvent (pCST->m_hEvtOperation) == 1)
					strLog.append (_T ("СБРОШЕН, изменен"));
				else
					strLog.append (_T ("СБРОШЕН, НЕ изменен"));
				break;
			case WAIT_OBJECT_0:
				strLog.append (_T ("УСТАНОВЛЕН, оставлен"));
				break;
			default:
				;
		}
	}
	else
		strLog.append (_T ("не известен..."));

	FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);
	strLog.free ();

	return iRes;
}

void CUserKTSDlg::DestroyUIThreadsCreateTemplate (void) {
    int iRes = 0; //Success	
	FILELOG_VARIABLE
	WORD key = 0;
	HThreadCreateTemplate *pThreadCreateTemplate = NULL;

	//Вариант с 'Map'
	POSITION pos = m_mapPtrThread.GetStartPosition ();
    while (pos) {
        m_mapPtrThread.GetNextAssoc (pos, key, (void *&) pThreadCreateTemplate);

		iRes = ((HWinApp *) AfxGetApp ())->TerminateHThread (pThreadCreateTemplate);

		//if (strcmp (pThreadCreateTemplate->GetRuntimeClass ()->m_lpszClassName, "HThreadCreateTemplate") == 0)
			DELETE_HTHREAD (pThreadCreateTemplate, HThreadCreateTemplate)
		//else ;
    }
    m_mapPtrThread.RemoveAll ();

	//Вариант с 'Array'
    /*int szArrayPtrCreateTemplate = m_mapPtrThread.GetSize ();

	while (szArrayPtrCreateTemplate) {
	    pThreadCreateTemplate = m_arPtrThreadCreateTemplate.GetAt (szArrayPtrCreateTemplate - 1);
	
	    TerminateHThread (pThreadCreateTemplate, true);
        
        m_arPtrThreadCreateTemplate.RemoveAt (szArrayPtrCreateTemplate - 1);
        
        szArrayPtrCreateTemplate = m_arPtrThreadCreateTemplate.GetSize ();
	}*/
}

void CUserKTSDlg::DestroyUIThreadsDirectoryWatched (void) {
    int iRes = 0;
	HThreadDirectoryWatched *pThreadDirWatched = NULL;

    int szArrayPtrDirWatched = m_arPtrThreadDirWatched.GetSize ();

	while (szArrayPtrDirWatched) {
	    pThreadDirWatched = m_arPtrThreadDirWatched.GetAt (szArrayPtrDirWatched - 1);
	
	    iRes = ((HWinApp *) AfxGetApp ())->TerminateHThread (pThreadDirWatched);

		DELETE_HTHREAD (pThreadDirWatched, HThreadDirectoryWatched)
        
        m_arPtrThreadDirWatched.RemoveAt (szArrayPtrDirWatched - 1);
        
        szArrayPtrDirWatched = m_arPtrThreadDirWatched.GetSize ();
	}
}

#if defined _WORK_THREAD
BOOL CUserKTSDlg::CreateThreadsDirectoryWatched (void)
{
    HString strLog;
	//SECURITY_ATTRIBUTES *pSA = DEBUG_NEW SECURITY_ATTRIBUTES;
    //pSA->nLength = sizeof (SECURITY_ATTRIBUTES);
    
    int iRes;
	HThreadDescription *pstDescThread = DEBUG_NEW HThreadDescription;

	HTREEITEM hTreeItem = m_pTreeCtrlTemplate->GetChildItem (m_pTreeCtrlTemplate->GetRootItem ());

	while (hTreeItem)
	{
		pstDescThread->AddData (&hTreeItem);
		//pstDescThread->idContainer = ((ITEM_DATA *) m_pTreeCtrlTemplate->GetItemData (pstDescThread->hTreeItem))->item_id;
		//pstDescThread->strPath = ((CString) m_pTreeCtrlTemplate->GetItemPATH (pstDescThread->hTreeItem));
		pstDescThread->AddData (HThreadDescription::PATH_DIRECTORY_WATCHED, m_pTreeCtrlTemplate->GetItemFullPath (* ((HTREEITEM *) pstDescThread->GetDataOfId (HThreadDescription::HTREEITEM_TEMPLATE))));
		//pstDescThread->strPath = ((CString) m_pTreeCtrlTemplate->GetItemFullPath (pstDescThread->idContainer)); //Не ЗАКОНЧЕНа ???

		//Инициализирован в КОНСТРУКТОРе
		//pstDescThread->m_idExitCodeThread = WAIT_IO_COMPLETION;

		//pstDescThread->hThread = CreateThread (NULL, 0, fThreadDirectoryWatched, pstDescThread->strPath.GetBuffer (), 0, &pstDescThread->idThread);
		pstDescThread->hThread = CreateThread (NULL, 0, fThreadDirectoryWatched, pstDescThread, 0, &pstDescThread->idThread);
		if (pstDescThread->hThread == NULL)
		{
			iRes = GetLastError ();
			return false;
		}
		else
		    ;
		
		m_arDescThreadDirWatched.Add (pstDescThread);
		
        strLog.format (_T ("Старт ПОТОКа: %i, %S"), pstDescThread->idThread, WC_HSTRING (pstDescThread->stringData (pstDescThread->GetDataOfId (HThreadDescription::PATH_DIRECTORY_WATCHED))));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2);
        strLog.free ();
        		
		hTreeItem = m_pTreeCtrlTemplate->GetNextItem (hTreeItem, TVGN_NEXT);

		pstDescThread = DEBUG_NEW HThreadDescription;
	}

	delete pstDescThread;

	return true;
}

void CUserKTSDlg::DestroyThreadsDirectoryWatched (void)
{
	BOOL bRes = false;
	HThreadDescription *pThreadDescDirWatched = NULL;
	HString strLog;

	for (int i = 0; i < m_arDescThreadDirWatched.GetSize (); i ++)
	{
		pThreadDescDirWatched = (HThreadDescription *) m_arDescThreadDirWatched.GetAt (i);
		GetExitCodeThread (pThreadDescDirWatched->hThread, &pThreadDescDirWatched->m_idExitCodeThread);
		if (pThreadDescDirWatched->m_idExitCodeThread == STILL_ACTIVE)
		{
			bRes = TerminateThread (pThreadDescDirWatched->hThread, 0x0);
			
            //strLog.Format (_T ("Остановлен ПОТОК: %i, %s"), m_arDescThreadDirWatched.GetAt (i)->idThread, m_arDescThreadDirWatched.GetAt (i)->stringData (m_arDescThreadDirWatched.GetAt (i)->GetDataOfNumber (1)));
            strLog.format (_T ("Остановлен ПОТОК: %i, %S"), pThreadDescDirWatched->idThread, WC_HSTRING (pThreadDescDirWatched->stringData (pThreadDescDirWatched->GetDataOfId (HThreadDescription::PATH_DIRECTORY_WATCHED))));
            //((CUserKTSApp *) theApp)->g_pFileLog->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2);
		}
		else
		    ;
		    
		CLOSE_HANDLE (pThreadDescDirWatched->hThread);
		pThreadDescDirWatched->idThread = 0x0;
		pThreadDescDirWatched->m_idExitCodeThread = WAIT_IO_COMPLETION;
	}
	
	for (int i = 0; i < m_arDescThreadDirWatched.GetSize (); i ++)
	    //pThreadDescDirWatched = (HThreadDescription *) m_arDescThreadDirWatched.GetAt (i);
		delete (HThreadDescription *) m_arDescThreadDirWatched.GetAt (i);

	m_arDescThreadDirWatched.RemoveAll ();
}

#endif

//Обработчик сообщений от ТАЙМЕРа
void CUserKTSDlg::OnTimer (UINT_PTR nIDEvent) {
    // TODO: Add your message handler code here and/or call default
    FILELOG_VARIABLE
    CTime tmEntry = CTime::GetCurrentTime (), tmExit;

    strLog.format (_T ("%S"), _T ("Обработка сообщения 'WM_TIMER'..."));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);

    if (IS_RUN_REMAINING) {
		//Приложение в режиме ограничения времени рабоТы
        if ((tmEntry - theApp.GetTimeStart ()).GetTotalSeconds () > WORK_SECONDS) {
            //Закрываем приложение по времени рабоТы
			GetDlgItem (IDCANCEL)->SendMessage (BM_CLICK);

            return;
        }
        else
            ;
    }
    else
        ;

    //Проверка на выполнение ОЧИСТКи за ТЕКущие сутки
    //if (GetModeApp () & ID_MODE_APP::MODE_LOCAL)
    {//Задача м.б. запущена ТОЛЬкО в ЛОКАЛЬноМ режиме

        //Запуск задачи очистки директории с резервными копиями УДАЛяемых/ПЕРЕНОСимых файлов
        CTime tmUndoClearRun = UNDOCLEAR_TIMERUN;
        //if (m_pUndoClear->bRun && ((tmNow > m_pUndoClear->tmRun) > 0))
        if ((tmEntry - UNDOCLEAR_TIMERUN) > 0) {
            //::PostThreadMessage (GetCurrentThreadId (), WM_USER_THREADUNDOCLEAR_RUN, (WPARAM) this->GetSafeHwnd (), 0);
            ::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_USER_THREADUNDOCLEAR_RUN, (WPARAM) this->GetSafeHwnd (), 0);
        }
        else
            ;
    }
    //else
    //    ;
        
    //switch (WaitForSingleObject (m_hEvtDirectoryWatched, INFINITE))
    switch (WaitForSingleObject (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::DIRECTORY_WATCHED), 66))
    //switch (WaitForSingleObject (m_hEvtDirectoryWatched, 0x0))
	{
		case WAIT_ABANDONED:
		case WAIT_TIMEOUT:
			break;
		case WAIT_OBJECT_0:
			FILELOG_WRITE_WITHDT ("Запрет работы ВСЕМ ПОТОКам...", USER_KTS_DLG);
			ResetEvent (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::DIRECTORY_WATCHED));
			
			m_pTreeCtrlTemplate->UpdateObjectCheckStateTemplate ();

	        TimeIndicators (ID_INDICATOR_TIME_PASSED, ID_INDICATOR_TIME_CURRENT);

	        FILELOG_WRITE_WITHDT ("Разрешение работы ВСЕМ ПОТОКам...", USER_KTS_DLG);
	        SetEvent (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::DIRECTORY_WATCHED));
		default:
			;
	}
	
	tmExit = CTime::GetCurrentTime ();

	//Проверка на активность потоков
	HThreadCreateTemplate *pThreadCreateTemplate = 0x0, *pThreadCreateTemplateNew = 0x0;
	POSITION pos = m_mapPtrThread.GetStartPosition ();
	WORD key = -1;
    while (pos) {
		pThreadCreateTemplate = 0x0;
        m_mapPtrThread.GetNextAssoc (pos, key, (void *&) pThreadCreateTemplate);

		if (pThreadCreateTemplate)
			if (pThreadCreateTemplate->getDiffLastResponce ().GetTotalSeconds () > 90) {
				strLog.format (_T ("timeout ответа ПОТОКа ID = %i!"), pThreadCreateTemplate->m_nThreadID);
				FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HERROR);

				//int iRes = -1, idData = -1;

				//pThreadCreateTemplateNew = DEBUG_NEW HThreadCreateTemplate ();

				//if (pThreadCreateTemplateNew) {
				//	strLog.format (_T ("Добавляем параметрЫ для потока (ПЕРЕзапуск) ID = %i"), pThreadCreateTemplateNew->m_nThreadID);
				//	FILELOG_WRITE_WITHDT (strLog.GetBuffer (), USER_KTS_DLG);

				//	idData = HThreadCreateTemplate::PTR_TREECTRLTEMPLATE;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	idData = HThreadCreateTemplate::PTR_CHECKSTATETEPMLATE;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	idData = HThreadCreateTemplate::PTR_ARPOSSELECTEDITEM;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	idData = HThreadCreateTemplate::STRING_AREXTSELECTEDITEM;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	idData = HThreadCreateTemplate::STRING_CONNECTDB;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	idData = HThreadCreateTemplate::STRING_TYPESOURCE;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	idData = HThreadCreateTemplate::NUMBER_OPERATION;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	idData = HThreadCreateTemplate::STRING_DIRECTORYUNDO;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	
				//	//Создание своего прогресс-бара
				//	/*idData = HThreadCreateTemplate::NUMBER_IDPROGRESSBAR;
				//	int idProgressCtrl = MakeProgressCtrl ();
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, &idProgressCtrl, sizeof (idProgressCtrl));*/

				//	idData = HThreadCreateTemplate::SPAN_INTERVALDATA;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	idData = HThreadCreateTemplate::BOOL_AUTOCAHNGESOURCE;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	idData = HThreadCreateTemplate::SHORT_NUMBERBEGINSOURCE;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//	idData = HThreadCreateTemplate::NUMBER_IDTEMPLATE;
				//	(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, pThreadCreateTemplate->GetDataOfId (idData), pThreadCreateTemplate->GetLengthDataOfId (idData));
				//}
				//else {
				//	strLog.format (_T ("Ошибка при СОЗДании обЪекта ПЕРЕзапускАЕМОГо ПОТОКа для ПОТОКА с ID = %i!"), pThreadCreateTemplate->m_nThreadID);
				//	FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
				//}

				//TerminateHThread (pThreadCreateTemplate, true);
				((HWinApp *) AfxGetApp ())->TerminateHThread ((HThreadCreateTemplate *) pThreadCreateTemplate);
				TerminateHThreadCreateTemplate ((HThreadCreateTemplate *) pThreadCreateTemplate);
				DELETE_HTHREAD (pThreadCreateTemplate, HThreadCreateTemplate)

				m_mapPtrThread.RemoveKey (key);

				//if (pThreadCreateTemplateNew) {
				//	if (pThreadCreateTemplateNew->CreateThread (CREATE_SUSPENDED)) {
				//		HCheckStateTemplate *pCST = * (HCheckStateTemplate **) ((HThreadCreateTemplate *) pThreadCreateTemplateNew)->GetDataOfId (HThreadCreateTemplate::PTR_CHECKSTATETEPMLATE);

				//		ResetEvent (pCST->m_hEvtOperation);
				//		//Создание своего прогресс-бара
				//		idData = HThreadCreateTemplate::NUMBER_IDPROGRESSBAR;
				//		int idProgressCtrl = MakeProgressCtrl ();
				//		(DYNAMIC_DOWNCAST (HWinThread, pThreadCreateTemplateNew))->AddData (idData, &idProgressCtrl, sizeof (idProgressCtrl));

				//		m_mapPtrThread.SetAt (pThreadCreateTemplate->m_nThreadID, pThreadCreateTemplate);

				//		if (pThreadCreateTemplate->ResumeThread () == 0xFFFFFFFF)
				//		{
				//			iRes = GetLastError ();
				//			PostMessage (WM_USER_EXIT_CREATE_TEMPLATE, NULL, pThreadCreateTemplateNew->m_nThreadID);
				//		}
				//		else
				//			::PostThreadMessage (pThreadCreateTemplateNew->m_nThreadID, WM_USER_THREAD_RESUME_CREATE_TEMPLATE, 0, 0);

				//		/*break;*/
				//	}
				//	else {
				//		strLog.format (_T ("Ошибка при СОЗДании ПЕРЕзапускАЕМОГо ПОТОКа!"));
				//		FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
				//		
				//		delete pThreadCreateTemplateNew;
				//	}
				//}
				//else
				//	; //Объект ПОТОКа не был создан - уже обработали
			}
			else
				; //ПОТОК отвечает своевременно
		else
			;
    }
	
	if ((tmExit - tmEntry).GetTotalSeconds () > 1) {
        strLog.format (_T ("Время выполнения 'CUserKTSDlg::OnTimer' составило %i секунд(ы)!"), (tmExit - tmEntry).GetTotalSeconds ());
        FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), USER_KTS_DLG);
        //MessageBox (strLog, _T("Внимание!"), MB_ICONINFORMATION);
    }
    else
        ;
    
    //if (! ((CUserKTSApp *) AfxGetApp ())->m_pIsUse ())
    //    //Освобождение БД
    //    PTR_DB->Disconnect ();

    //HDlgKTSFiles::OnTimer (nIDEvent);
}

void CUserKTSDlg::OnClose ()
{
    // TODO: Add your message handler code here and/or call default
    HDlgKTSFiles::OnClose();
}

void CUserKTSDlg::OnDestroy () {
    //Как узнать текущее состояние окна ???

	//int iCountMenuItem = GetMenu ()->GetMenuItemCount ();
	CMenu *ptrSubMenu = 0x0;
	while (GetMenu ()->GetMenuItemCount ()) {
		ptrSubMenu = GetMenu ()->GetSubMenu (0);
		ptrSubMenu->DestroyMenu ();
		GetMenu ()->DeleteMenu (0, MF_STRING | MF_BYPOSITION);
		delete ptrSubMenu; ptrSubMenu = 0x0;
	}

	delete GetMenu ();

    // TODO: Add your message handler code here
    if (m_iIdTimerUpdate > 0) {
	    try { KillTimer (m_iIdTimerUpdate); }
	    catch (CException *err) {
			throw (err);
	    }
    }
	else
		;

    delete GetDlgItem (IDC_COMMAND_DIR_LIST);
    delete GetDlgItem (IDC_STATIC_FULLPATH);
	
	HDlgKTSFiles::OnDestroy ();
}

#define re_cast(type, obj) reinterpret_cast <#type> (obj)

void CUserKTSDlg::OnBnClickedCancel () {
    // TODO: Add your control notification handler code here
#if defined _WORK_THREAD
    DestroyThreadsDirectoryWatched ();
#else
    DestroyUIThreadsDirectoryWatched ();
    DestroyUIThreadsCreateTemplate ();
#endif
    
    //DestroyWindow ();
    EndDialog (0x0);
}

CMenu *CUserKTSDlg::CreateMainMenu (void) {
	int iRes = 0x0;
	CMenu *ptrMainMenu = 0x0, *ptrSubMenu = 0x0;

	ptrMainMenu = DEBUG_NEW CMenu ();
	ptrMainMenu->CreateMenu ();
	
	ptrSubMenu = DEBUG_NEW CMenu ();
	ptrSubMenu->CreatePopupMenu ();
	ptrSubMenu->AppendMenu (MF_BYCOMMAND | MF_STRING, IDM_TEMPLATE_ADD, _T ("Добавить"));
	ptrSubMenu->AppendMenu (MF_BYCOMMAND | MF_STRING | MF_DISABLED | MF_GRAYED, IDM_TEMPLATE_CHANGE, _T ("Изменить"));
	ptrSubMenu->AppendMenu (MF_SEPARATOR, 0x0, _T ("-"));
	ptrSubMenu->AppendMenu (MF_BYCOMMAND | MF_STRING, IDM_TEMPLATE_DEL, _T ("Удалить"));
	
	iRes = ptrMainMenu->AppendMenu (MF_POPUP | MF_STRING, (INT_PTR) ptrSubMenu->GetSafeHmenu (), _T ("Шаблон"));
	ptrSubMenu = 0x0;
	ptrSubMenu = ptrMainMenu->GetSubMenu (0);

	ptrSubMenu = DEBUG_NEW CMenu ();
	ptrSubMenu->CreatePopupMenu ();
	ptrSubMenu->AppendMenu (MF_BYCOMMAND | MF_STRING, IDM_OPERATION_UPDATE, _T ("Обновить информацию"));
	
	iRes = ptrMainMenu->AppendMenu (MF_POPUP | MF_STRING, (INT_PTR) ptrSubMenu->GetSafeHmenu (), _T ("Действие"));
	ptrSubMenu = 0x0;
	ptrSubMenu = ptrMainMenu->GetSubMenu (1);

	ptrSubMenu = DEBUG_NEW CMenu ();
	ptrSubMenu->CreatePopupMenu ();
	ptrSubMenu->AppendMenu (MF_BYCOMMAND | MF_STRING | MF_DISABLED | MF_GRAYED, IDM_HELP_CONTEXT, _T ("Контекст"));
	ptrSubMenu->AppendMenu (MF_SEPARATOR, 0x0, _T ("-"));
	ptrSubMenu->AppendMenu (MF_BYCOMMAND | MF_STRING, IDM_HELP_ABOUT, _T ("О программе"));
	
	iRes = ptrMainMenu->AppendMenu (MF_POPUP | MF_STRING, (INT_PTR) ptrSubMenu->GetSafeHmenu (), _T ("Помощь"));
	ptrSubMenu = 0x0;
	ptrSubMenu = ptrMainMenu->GetSubMenu (2);

	return ptrMainMenu;
}
// PseudoKTS.cpp : Определяет поведение классов для приложения.
//

#include "stdafx.h"

#include "PseudoKTS.h"

//При объявлении этого значения будет создано окно для 'App'
//#define _MAINWINDOOW_SDI

#if defined _MAINWINDOOW_SDI
#include "MainFrm.h"
#include "PseudoKTSDoc.h"
#include "PseudoKTSView.h"
#else
#endif

#include "HThreadGUIFilesDB.h"
#include "hthreadGUIcontrol.h"
#include "HThreadGetData.h"
#include "HThreadWriteData.h"
#include "HThreadTimer.h"
#include  <HThreadDirectoryWatched.h>

// CPseudoKTSApp

BEGIN_MESSAGE_MAP(CPseudoKTSApp, HWinAppKTS)
	ON_COMMAND(ID_APP_ABOUT, &CPseudoKTSApp::OnAppAbout)
	// Стандартные команды по работе с файлами документов
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Стандартная команда настройки печати
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
    ON_THREAD_MESSAGE (WM_USER_THREAD_GUI_CLOSE, &CPseudoKTSApp::OnThreadGuiFilesDBClose)
    ON_THREAD_MESSAGE (WM_USER_THREAD_TIMER, &CPseudoKTSApp::OnThreadTimer)
    ON_THREAD_MESSAGE (WM_USER_ENTER_DIRECTORY_WATCHED, &OnThreadEnterDirectoryWatched)
    ON_THREAD_MESSAGE (WM_USER_LEAVE_DIRECTORY_WATCHED, &OnThreadLeaveDirectoryWatched)
    ON_THREAD_MESSAGE (IDM_RESTORE, &OnRestoreDlg)
    ON_THREAD_MESSAGE (WM_USER_SOURCEDB_SELECHANGED, &OnSourceDBselectedChanged)
    ON_THREAD_MESSAGE (WM_USER_QUERY_THREAD_ID, &OnQueryThreadId)
    ON_THREAD_MESSAGE (WM_USER_QUERY_NUMBER_PTIC_CONTROL, &OnQueryNumberPTICControl)
END_MESSAGE_MAP()

// создание CPseudoKTSApp

// Единственный объект CPseudoKTSApp

CPseudoKTSApp theApp;

EXT_DECLARE_HTHREAD_FILELOG
EXT_DECLARE_WSA
//DECLARE_HDATETIMEINTERVAL

//#import "C:\temp\energ6\getdata.exe"
//using namespace getdata;

#ifdef _DEBUG
extern CMemoryState g_memStateDiff,
                    g_memStateBegin,
                    g_memStateEnd;
#endif

CPseudoKTSApp::CPseudoKTSApp () : m_bDestroyUIThreads (FALSE) {
	// поддержка диспетчера перезагрузки
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART_ASPECTS;
#ifdef _MANAGED
	// Если приложение построено с поддержкой среды Common Language Runtime (/clr):
	//     1) Этот дополнительный параметр требуется для правильной поддержки работы диспетчера перезагрузки.
	//     2) В своем проекте для построения необходимо добавить ссылку на System.Windows.Forms.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: замените ниже строку идентификатора приложения строкой уникального идентификатора; рекомендуемый
	// формат для строки: ИмяКомпании.ИмяПродукта.СубПродукт.СведенияОВерсии
	SetAppID (_T ("PseudoKTS.AppID.NoVersion"));

	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
    InitializeCriticalSection (&m_CriticalSectionPtrThread);

    m_hEnterDirectoryWatched = CreateEvent (NULL, TRUE, FALSE, NULL);
}

// CPseudoKTSApp DEnstruction
CPseudoKTSApp::~CPseudoKTSApp (void) {
	// TODO: add DEnstruction code here,
	// Place all significant initialization in InitInstance
    DeleteCriticalSection (&m_CriticalSectionPtrThread);

    CloseHandle (m_hEnterDirectoryWatched);
}

#define CREATE_HTHREAD(type, className) { \
                                            className *ptrThread = 0x0; \
                                            ptrThread = DEBUG_NEW className (); \
                                            DELETE_HTHREAD (type, className) \
                                        }

// инициализация CPseudoKTSApp
int CPseudoKTSApp::InitInstance () {
    FILELOG_VARIABLE

    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	BOOL bRes = HWinAppKTS::InitInstance ();

#if defined _MAINWINDOOW_SDI
    // Зарегистрируйте шаблоны документов приложения. Шаблоны документов
	//  выступают в роли посредника между документами, окнами рамок и представлениями
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS (CPseudoKTSDoc),
		RUNTIME_CLASS (CMainFrame),       // основное окно рамки SDI
		RUNTIME_CLASS (CPseudoKTSView));
	if (! pDocTemplate)
		return FALSE;
	AddDocTemplate (pDocTemplate);

	// Включить открытие выполнения DDE
	EnableShellOpen ();
	RegisterShellFileTypes (TRUE);

	// Команды диспетчеризации, указанные в командной строке. Значение FALSE будет возвращено, если
	// приложение было запущено с параметром /RegServer, /Register, /Unregserver или /Unregister.
	if ( !ProcessShellCommand (cmdInfo))
		return FALSE;

	// Одно и только одно окно было инициализировано, поэтому отобразите и обновите его
	//m_pMainWnd->ShowWindow (SW_SHOW);
    //m_pMainWnd->ShowWindow (SW_SHOWNOACTIVATE);
    //m_pMainWnd->ShowWindow (SW_SHOWMINNOACTIVE);
    //m_pMainWnd->ShowWindow (SW_FORCEMINIMIZE);
    m_pMainWnd->ShowWindow (SW_HIDE);
	m_pMainWnd->UpdateWindow ();
	// вызов DragAcceptFiles только при наличии суффикса
	// В приложении SDI это должно произойти после ProcessShellCommand
	// Включить открытие перетаскивания
	m_pMainWnd->DragAcceptFiles ();
#else
#endif

    //CREATE_HTHREAD (THREAD_DIRECTORYWATCHED, HThreadDirectoryWatched);

	if (bRes)
		//Запуск ПОТОКа интерфейса с ПОЛЬЗОВАТЕЛем
		if (CreateThreadGUIFilesDB ())
            //Запуск ПОТОКа контроля ТЕКУЩих данных
		    if (CreateThreadGUIControl ())
			    //Запуск ПОТОКа получения данных
			    if (CreateThreadGetData ())
				    //Запуск ПОТОКа записи данных
				    if (CreateThreadWriteData ())
					    //Запуск ПОТОКа ТАМЕРа
					    if (CreateThreadTimer ())
						    //Запуск ПОТОКа проверки состояния файлов БД
						    /*ptrThread = DEBUG_NEW HThreadCheckStateTemplate ();
						    ptrThread->CreateThread (CREATE_SUSPENDED);

						    ptrThread->ResumeThread ();

						    m_mapPtrThread.SetAt (THREAD_CHECKSTATETEMPLATE, ptrThread);
						     = ptrThread->m_nThreadID;
						    ::PostThreadMessage (ptrThread->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);*/
							    //Запуск ПОТОКа слежения за директорией
							    if (CreateThreadDirectoryWatched ())
								    ;
							    else bRes = FALSE; //слежениЕ за директорией
					    else bRes = FALSE; //ТАМЕР
				    else bRes = FALSE; //записЬ данных
			    else bRes = FALSE; //получениЕ данных
            else ; //контролЬ ТЕКУЩих данных
		else bRes = FALSE; //контролЬ файлов БД
	else ;

    if (bRes) {
		m_ptrMenuTray = DEBUG_NEW CMenu ();
		bRes = m_ptrMenuTray->CreatePopupMenu ();

		if (bRes) {
			m_ptrMenuTray->AppendMenu (MF_BYPOSITION | MF_STRING, IDM_RESTORE_FILES_DB, _T ("Файлы БД"));
            m_ptrMenuTray->AppendMenu (MF_BYPOSITION | MF_STRING, IDM_RESTORE_CONTROL, _T ("Контроль"));
			m_ptrMenuTray->AppendMenu(MF_SEPARATOR);
			m_ptrMenuTray->AppendMenu (MF_BYPOSITION | MF_STRING, IDM_EXIT, _T ("Закрыть"));
		}
		else
			;
	}
	else
		;

	return bRes;
}

int CPseudoKTSApp::ExitInstance () {
	//TODO: обработайте дополнительные ресурсы, которые могли быть добавлены
	DestroyUIThreads ();

	return HWinAppKTS::ExitInstance ();
}

// обработчики сообщений CPseudoKTSApp
afx_msg void CPseudoKTSApp::OnThreadGuiFilesDBClose (WPARAM wp, LPARAM ) {
	PostThreadMessage (WM_QUIT, wp, 0);
}

afx_msg void CPseudoKTSApp::OnThreadTimer (WPARAM nameInterval, LPARAM secondsInterval) {
    if (! m_bDestroyUIThreads) {
        SYSTEMTIME stmNow;; GetLocalTime (&stmNow);
        HString strNameInterval ((TCHAR *) nameInterval);
        TRACE ("Системное время: %i ч %i мин %i сек %i мс (%s)\n", stmNow.wHour, stmNow.wMinute, stmNow.wSecond, stmNow.wMilliseconds, MB_HSTRING (strNameInterval));

        //Вариантт №1
        /*if (strNameInterval.compareNoCase (_T ("SECOND_15")) == 0) {
		    ::PostThreadMessage (((HWinThread *) m_mapPtrThread [THREAD_GUI_FILESDB])->m_nThreadID, WM_USER_TIMER, secondsInterval, 0);
		    ::PostThreadMessage (((HWinThread *) m_mapPtrThread [THREAD_GETDATA])->m_nThreadID, WM_USER_TIMER, secondsInterval, 0);
        }
        else
            if (strNameInterval.compareNoCase (_T ("SECOND_30")) == 0) {
            }
            else
                if (strNameInterval.compareNoCase (_T ("MINUTE_3")) == 0) {
                }
                else
                    if (strNameInterval.compareNoCase (_T ("MINUTE_05")) == 0) {
					    ::PostThreadMessage (((HWinThread *) m_mapPtrThread [THREAD_GETDATA])->m_nThreadID, WM_USER_TIMER, secondsInterval, 0);
                    }
                    else
                        if (strNameInterval.compareNoCase (_T ("MINUTE_30")) == 0) {
					    }
					    else
						    ; //Неизвестный интервал*/

        //Вариантт №2
        /*switch (secondsInterval) {
            case 15:
                ::PostThreadMessage (((HWinThread *) m_mapPtrThread [THREAD_GUI_FILESDB])->m_nThreadID, WM_USER_TIMER, secondsInterval, 0);
			    ::PostThreadMessage (((HWinThread *) m_mapPtrThread [THREAD_GETDATA])->m_nThreadID, WM_USER_TIMER, secondsInterval, 0);
                break;
            case 30:
                break;
            case 3 * 60:
                break;
		    case 5 * 60:
			    ::PostThreadMessage (((HWinThread *) m_mapPtrThread [THREAD_GETDATA])->m_nThreadID, WM_USER_TIMER, secondsInterval, 0);
                break;
            case 30 * 60:
                break;
            default:
                ;
        }*/

	    //Вариант №3
	    //Для 'THREAD_GUI_FILESDB'
	    if (strNameInterval.compareNoCase (_T ("SECOND_15")) == 0)
		    ::PostThreadMessage (((HWinThread *) m_mapPtrThread [THREAD_GUI_FILESDB])->m_nThreadID, WM_USER_TIMER, secondsInterval, 0);
	    else
		    ;

        //Для 'THREAD_GUI_CONTROL'
	    if (strNameInterval.compareNoCase (_T ("SECOND_15")) == 0)
		    ::PostThreadMessage (((HWinThread *) m_mapPtrThread [THREAD_GUI_CONTROL])->m_nThreadID, WM_USER_TIMER, secondsInterval, 0);
	    else
		    ;

	    //Для 'THREAD_GETDATA'
        if ((strNameInterval.compareNoCase (_T ("SECOND_05")) == 0) ||
	        (strNameInterval.compareNoCase (_T ("SECOND_30")) == 0) /*||
            (strNameInterval.compareNoCase (_T ("SECOND_15")) == 0) ||
            (strNameInterval.compareNoCase (_T ("MINUTE_01")) == 0) /*||
		    (strNameInterval.compareNoCase (_T ("MINUTE_05")) == 0) ||
		    (strNameInterval.compareNoCase (_T ("MINUTE_30")) == 0)*/)
		    ::PostThreadMessage (((HWinThread *) m_mapPtrThread [THREAD_GETDATA])->m_nThreadID, WM_USER_TIMER, secondsInterval, 0);
	    else
		    ;

	    WORD key = 0;
	    HWinThread *pThread = 0x0,
                    *ptrThreadSender = 0x0;

        int mode = 0;

        //Автовосстановление работоспособности
        if (strNameInterval.compareNoCase (_T ("SECOND_05")) == 0) {
            POSITION pos = 0x0;
            while (pos) {
                pos = m_mapPtrThread.GetStartPosition ();
                m_mapPtrThread.GetNextAssoc (pos, key, (void *&) pThread);
		        if (pThread->IsLive () == FALSE) {
			        TerminateHThread (pThread);

			        switch (key) {
				        case THREAD_GUI_FILESDB:
					        DELETE_HTHREAD (pThread, HThreadGUIFilesDB)
					        break;
                        case THREAD_GUI_CONTROL:
					        DELETE_HTHREAD (pThread, HThreadGUIControl)
                            ptrThreadSender = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GETDATA);
                            if (! (ptrThreadSender == 0x0))
                                ptrThreadSender->PostThreadMessage (WM_USER_RESET_THREAD_RESPONCE, 0, 0);
                            else
                                ;
					        break;
				        case THREAD_GETDATA:
					        DELETE_HTHREAD (pThread, HThreadGetData)
                            ptrThreadSender = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GUI_CONTROL);
                            if (! (ptrThreadSender == 0x0))
                                ptrThreadSender->PostThreadMessage (WM_USER_RESET_THREAD_RESPONCE, 0, 0);
                            else
                                ;
					        break;
				        case THREAD_WRITEDATA:
					        DELETE_HTHREAD (pThread, HThreadWriteData)
					        break;
				        case THREAD_TIMER:
					        DELETE_HTHREAD (pThread, HThreadTimer)
					        break;
				        case THREAD_DIRECTORYWATCHED:
					        DELETE_HTHREAD (pThread, HThreadDirectoryWatched)
					        break;
				        default:
					        DELETE_HTHREAD (pThread, HWinThread);
                            //Удалить элемент из 'm_mapPtrThread' ???
			        }
		        }
		        else {
                    pThread->PostThreadMessage (WM_USER_THREAD_LIVE, 0, 0);

                    switch (key) {
				        case THREAD_GUI_FILESDB:
					        break;
                        case THREAD_GUI_CONTROL:
					        break;
				        case THREAD_GETDATA:
					        break;
				        case THREAD_WRITEDATA:
					        break;
				        case THREAD_TIMER:
					        break;
				        case THREAD_DIRECTORYWATCHED:
					        break;
				        default: ;
			        }
                }
	        }

            //Создать все отсутствующие потоки (запомнить какие отсутствовали)
            for (key = THREAD_GUI_FILESDB; key < COUNT_TYPE_THREAD; key ++) {
                if (m_mapPtrThread.Lookup (key, (void *&) pThread) == true) {
                }
                else {
                    switch (key) {
				        case THREAD_GUI_FILESDB:
					        CreateThreadGUIFilesDB ();
					        SetEvent (m_arHandleEvent [DIRECTORY_WATCHED]);
					        break;
                        case THREAD_GUI_CONTROL:
					        CreateThreadGUIControl ();
					        break;
				        case THREAD_GETDATA:
					        CreateThreadGetData ();
					        break;
				        case THREAD_WRITEDATA:
					        CreateThreadWriteData ();
					        break;
				        case THREAD_TIMER:
					        CreateThreadTimer ();
					        break;
				        case THREAD_DIRECTORYWATCHED:
					        CreateThreadDirectoryWatched ();
					        break;
				        default:
					        DELETE_HTHREAD (pThread, HWinThread);
                            //Удалить элемент из 'm_mapPtrThread' ???
			        }

                    mode |= 1 << key;
                }
            }

            for (key = THREAD_GUI_FILESDB; key < COUNT_TYPE_THREAD; key ++) {
                if (m_mapPtrThread.Lookup (key, (void *&) pThread) == true) {
                    if ((mode & (1 << key)) == (1 << key)) {
                        switch (key) {
                            case THREAD_GUI_FILESDB:
					            break;
                            case THREAD_GUI_CONTROL:
					            break;
				            case THREAD_GETDATA:
					            break;
				            case THREAD_WRITEDATA:
					            break;
				            case THREAD_TIMER:
					            break;
				            case THREAD_DIRECTORYWATCHED:
                                pThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GUI_FILESDB);
                                if (! (pThread == 0x0)) {
                                    pThread->PostThreadMessage (WM_USER_INIT_COMBOBOX_DIRWATCH, 0, 0);

                                    ::PostThreadMessage (m_nThreadID, WM_USER_SOURCEDB_SELECHANGED, 0x0, 0x0);
                                }
                                else
                                    ;
					            break;
				            default:
                                ;
                        }
                    }
                    else
                        ;
                } else { }
            }

            if (FALSE) ; else ; //PostThreadMessage (WM_QUIT, -1, 0);
        }
        else
            ;
    }
    else
        ; //DestroyUIThreads
}

afx_msg void CPseudoKTSApp::OnThreadEnterDirectoryWatched (WPARAM wp, LPARAM ) {
    SetEvent (m_hEnterDirectoryWatched);
}

afx_msg void CPseudoKTSApp::OnThreadLeaveDirectoryWatched (WPARAM wp, LPARAM ) {
    TerminateHThread ((CObject *&) m_mapPtrThread [THREAD_DIRECTORYWATCHED]);
    DELETE_HTHREAD (m_mapPtrThread [THREAD_DIRECTORYWATCHED], HThreadDirectoryWatched)
    m_mapPtrThread.RemoveKey (THREAD_DIRECTORYWATCHED);

    ResetEvent (m_hEnterDirectoryWatched);
}

void CPseudoKTSApp::DestroyUIThreads (void) {
    FILELOG_VARIABLE

    strLog.format ("CPseudoKTSApp::DestroyUIThreads...");
    FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HCOMMON);

    m_bDestroyUIThreads = TRUE;

	WORD key = 0;
	HWinThread *pThread = NULL;

	//Вариант с 'Map'
	POSITION pos = m_mapPtrThread.GetStartPosition ();
    while (pos) {
        m_mapPtrThread.GetNextAssoc (pos, key, (void *&) pThread);

		TerminateHThread (pThread);

		switch (key) {
			case THREAD_GUI_FILESDB:
				DELETE_HTHREAD (pThread, HThreadGUIFilesDB)
				break;
            case THREAD_GUI_CONTROL:
				DELETE_HTHREAD (pThread, HThreadGUIFilesDB)
				break;
            case THREAD_GETDATA:
				DELETE_HTHREAD (pThread, HThreadGetData)
				break;
			case THREAD_WRITEDATA:
				DELETE_HTHREAD (pThread, HThreadGetData)
				break;
            case THREAD_TIMER:
				DELETE_HTHREAD (pThread, HThreadTimer)
				break;
            case THREAD_DIRECTORYWATCHED:
				DELETE_HTHREAD (pThread, HThreadDirectoryWatched)
				break;
			default:
                DELETE_HTHREAD (pThread, HWinThread);
		}

        m_mapPtrThread.RemoveKey (key);
    }

    m_mapPtrThread.RemoveAll ();
}

// обработчики сообщений CPseudoKTSApp
afx_msg void CPseudoKTSApp::OnAppAbout () {
	HWinThread *pThreadGUI = 0x0;
	m_mapPtrThread.Lookup (THREAD_GUI_FILESDB, (void *&)pThreadGUI);
	if (pThreadGUI)
		pThreadGUI->m_pMainWnd->SendMessage (WM_SYSCOMMAND, IDM_ABOUTBOX);
	else
		;
}

int CPseudoKTSApp::SetOption () {
    int iRes = 0;

    return iRes;
}

DWORD CPseudoKTSApp::CreateThreadGUIFilesDB (void) {
	HThreadGUIFilesDB *ptrThread = DEBUG_NEW HThreadGUIFilesDB ();
	ptrThread->CreateThread (CREATE_SUSPENDED);

	ptrThread->setMaxDiffLastResponce (60);

	ptrThread->ResumeThread ();

	EnterCriticalSection (&m_CriticalSectionPtrThread);
    m_mapPtrThread.SetAt (THREAD_GUI_FILESDB, ptrThread);
    LeaveCriticalSection (&m_CriticalSectionPtrThread);

    ::PostThreadMessage (ptrThread->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);

    GetExitCodeThread (ptrThread->m_hThread, &ptrThread->m_idExitCodeThread);

    return ptrThread->m_nThreadID;
}

DWORD CPseudoKTSApp::CreateThreadGUIControl (void) {
	HThreadGUIControl *ptrThread = DEBUG_NEW HThreadGUIControl ();
	ptrThread->CreateThread (CREATE_SUSPENDED);

	ptrThread->setMaxDiffLastResponce (60);

	ptrThread->ResumeThread ();

	EnterCriticalSection (&m_CriticalSectionPtrThread);
    m_mapPtrThread.SetAt (THREAD_GUI_CONTROL, ptrThread);
    LeaveCriticalSection (&m_CriticalSectionPtrThread);

    ::PostThreadMessage (ptrThread->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);

    GetExitCodeThread (ptrThread->m_hThread, &ptrThread->m_idExitCodeThread);

    return ptrThread->m_nThreadID;
}

DWORD CPseudoKTSApp::CreateThreadGetData (void) {
	HThreadGetData *ptrThread = DEBUG_NEW HThreadGetData ();
	ptrThread->CreateThread (CREATE_SUSPENDED);

	//ptrThread->setMaxDiffLastResponce (60);

    //ptrThread->AddData (HThreadGetData::STR_CONNECT_DB, PTR_SETTINGS->CompileConnectDB (PTR_SETTINGS->GetOption (_T ("ИД_ИСТОЧНИКДАННЫХ_ТЕПЛО")).number (), HDBSettings::SQL_DB));
    
    /*int iIdSource = PTR_SETTINGS->GetOption (_T ("ИД_ИСТОЧНИКДАННЫХ_ТЕПЛО")).number ();
    ptrThread->AddData (HThreadGetData::INT_SOURCE_ID, &iIdSource, sizeof (int));*/

    ptrThread->ResumeThread ();

	EnterCriticalSection (&m_CriticalSectionPtrThread);
    m_mapPtrThread.SetAt (THREAD_GETDATA, ptrThread);
    LeaveCriticalSection (&m_CriticalSectionPtrThread);

    ::PostThreadMessage (ptrThread->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);

	return ptrThread->m_nThreadID;
}

DWORD CPseudoKTSApp::CreateThreadWriteData (void) {
	HThreadWriteData *ptrThread = DEBUG_NEW HThreadWriteData ();
	ptrThread->CreateThread (CREATE_SUSPENDED);

	//ptrThread->setMaxDiffLastResponce (60);

    ptrThread->ResumeThread ();

	EnterCriticalSection (&m_CriticalSectionPtrThread);
    m_mapPtrThread.SetAt (THREAD_WRITEDATA, ptrThread);
    LeaveCriticalSection (&m_CriticalSectionPtrThread);

    ::PostThreadMessage (ptrThread->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);

	return ptrThread->m_nThreadID;
}

DWORD CPseudoKTSApp::CreateThreadTimer (void) {
	HThreadTimer *ptrThread = DEBUG_NEW HThreadTimer ();
	ptrThread->CreateThread (CREATE_SUSPENDED);

	//ptrThread->setMaxDiffLastResponce (60);

    ptrThread->ResumeThread ();

	EnterCriticalSection (&m_CriticalSectionPtrThread);
    m_mapPtrThread.SetAt (THREAD_TIMER, ptrThread);
    LeaveCriticalSection (&m_CriticalSectionPtrThread);

    ::PostThreadMessage (ptrThread->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);

	return ptrThread->m_nThreadID;
}

DWORD CPseudoKTSApp::CreateThreadDirectoryWatched () {
	DWORD idThreadResponseDirectoryWatched = GetPtrThread (THREAD_GUI_FILESDB)->m_nThreadID;
	HString strDirectoryWatch;
    HRecordset *pRS = PTR_DB->GetRecordset (this, HString (_T (TABLENAME_TEMPLATE)));
    if (pRS->SeekRSTemplateToItem (1)) {
        pRS->GetFieldValue (_T ("ITEM_PATH"), strDirectoryWatch);
        strDirectoryWatch.insert (0, _T ("\\"));
        if (MODE_APP & HWinAppKTS::MODE_LOCAL)
            strDirectoryWatch.insert (0, PTR_SETTINGS->GetOption (_T ("NetSharedSource")));
        else
            strDirectoryWatch.insert (0, PTR_SETTINGS->GetOption (_T ("NetSharedName")));
    }
    else
        ;

    PTR_DB->ReleaseRS (this, pRS);

	HThreadDirectoryWatched *ptrThread = DEBUG_NEW HThreadDirectoryWatched ();
	ptrThread->CreateThread (CREATE_SUSPENDED);

	//ptrThread->setMaxDiffLastResponce (60);
    
    ptrThread->AddData (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED, strDirectoryWatch);

	ptrThread->AddData (HThreadDirectoryWatched::HANDLE_EVENT_DIRECTORY_WATCHED, &APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::DIRECTORY_WATCHED), sizeof (DWORD));
	ptrThread->AddData (HThreadDirectoryWatched::INT_IDTHREAD_RESPONSE, &idThreadResponseDirectoryWatched, sizeof (DWORD));

    ptrThread->ResumeThread ();

	EnterCriticalSection (&m_CriticalSectionPtrThread);
    m_mapPtrThread.SetAt (THREAD_DIRECTORYWATCHED, ptrThread);
    LeaveCriticalSection (&m_CriticalSectionPtrThread);

    ::PostThreadMessage (ptrThread->m_nThreadID, WM_USER_THREAD_RESUME, 0, 0);
	
	return ptrThread->m_nThreadID;
}

afx_msg void CPseudoKTSApp::OnRestoreDlg (WPARAM wp, LPARAM ) {
    DWORD id_type_thread = -1;
    switch ((int) wp) {
        case IDM_RESTORE_FILES_DB:
            id_type_thread = THREAD_GUI_FILESDB;
            break;
        case IDM_RESTORE_CONTROL:
            id_type_thread = THREAD_GUI_CONTROL;
            break;
        default: ;
    };

    ::ShowWindow (((HWinThread *) m_mapPtrThread [id_type_thread])->m_pMainWnd->GetSafeHwnd (), SW_RESTORE);
}

afx_msg void CPseudoKTSApp::OnSourceDBselectedChanged (WPARAM , LPARAM ) {
    BOOL bRes = true;
    WORD key = -1;
    POSITION pos = m_mapPtrThread.GetStartPosition ();
    HWinThread *pThread = 0x0;
    /*while (pos) {
        m_mapPtrThread.GetNextAssoc (pos, key, (void *&) pThread);

		switch (key) {
            case THREAD_GUI_CONTROL:
                TerminateHThread (pThread);
				DELETE_HTHREAD (pThread, HThreadGUIFilesDB)
				break;
            case THREAD_GETDATA:
                TerminateHThread (pThread);
				DELETE_HTHREAD (pThread, HThreadGetData)
				break;
			default:
                ;
		}
    }

    if (CreateThreadGUIControl ())
        if (CreateThreadGetData ())
            ;
        else
            bRes = false;
    else
        bRes = false;*/

    for (key = CPseudoKTSApp::THREAD_GUI_FILESDB; key < COUNT_TYPE_THREAD; key ++) {
        switch (key) {
            case THREAD_GUI_CONTROL:
            case THREAD_GETDATA:
                if (m_mapPtrThread.Lookup (key, (void* &) pThread) == TRUE) {
                    bRes = pThread->PostThreadMessage (WM_USER_CONNECT_TO_SERVER, 0x0, 0x0);
                } else { }
				break;
			default:
                ;
		}

        if (bRes == FALSE) break; else ;
    }

    if (bRes == false)
        ::PostThreadMessage (m_nThreadID, WM_QUIT, 0, 0);
    else
        ;
}

afx_msg void CPseudoKTSApp::OnQueryThreadId (WPARAM wp, LPARAM lp) {
    //wp - кому? ID [DWORD]
    //lp - чей?
    HWinThread *pThread = 0x0;
    if (m_mapPtrThread.Lookup ((WORD)lp, (void * &)pThread) == TRUE) {
        ::PostThreadMessage (pThread->m_nThreadID, WM_USER_QUERY_THREAD_ID, wp, 0);
    }
    else { }
}

afx_msg void CPseudoKTSApp::OnQueryNumberPTICControl (WPARAM wp, LPARAM ) {
    //wp - кому? ID [DWORD]
    //lp - чей?
    HWinThread *pThread = 0x0;
    if (m_mapPtrThread.Lookup ((WORD)THREAD_GUI_CONTROL, (void * &)pThread) == TRUE) {
        ::PostThreadMessage (pThread->m_nThreadID, WM_USER_QUERY_NUMBER_PTIC_CONTROL, wp, 0);
    }
    else { }
}

HWinThread *CPseudoKTSApp::GetPtrThread (WORD type) {
    HWinThread *ptrRes = 0x0;
    
    EnterCriticalSection (&m_CriticalSectionPtrThread);
    m_mapPtrThread.Lookup (type, (void *&) ptrRes);
    LeaveCriticalSection (&m_CriticalSectionPtrThread);
    
    return ptrRes;
}
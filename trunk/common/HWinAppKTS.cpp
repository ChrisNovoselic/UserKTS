// HWinApp.cpp: ���� ����������
//

#include "stdafx.h"
#include <tlhelp32.h>

// HWinAppKTS
EXT_DECLARE_HTHREAD_FILELOG
EXT_DECLARE_WSA

DECLARE_HDATETIMEINTERVAL

//IMPLEMENT_DYNCREATE(HWinAppKTS, HWinApp)

extern BOOL FreeMemoryDBObject(CObject *&);

HString ReFindFile (HString &, HString &);

//extern DWORD findTask (char *);
extern DWORD findIdProcess (CString, int);
extern DWORD findIdThreadAppOfProcess (DWORD);

extern BOOL CALLBACK fWndEnumProcCloseErr (HWND , LPARAM );

BEGIN_MESSAGE_MAP(HWinAppKTS, HWinApp)
END_MESSAGE_MAP()

HWinAppKTS::HWinAppKTS () {
	int i = -1;
	
	//TCHAR str [] = _T ("ierfiejhgvkldnvet534");
	//int ar [666], i;
	//i = lenof (str);
	//i = lenof (ar);
	//i = 0;
	
	//_� � � � � � �_
	//������ ���� ���������
	//HANDLE hSnapshot; // = NULL;
	//HWND hCurrentWindow = FindWindow (NULL, _T ("")); // ������� ����;
	//TCHAR ptrNameWindow [4096];

	//do {
	//	if (IsWindow (hCurrentWindow)) {//�������� ���������� �����
	//		if (IsWindowVisible (hCurrentWindow)) {
	//			if (GetWindowText (hCurrentWindow, ptrNameWindow, sizeof (ptrNameWindow)) != NULL) {
	//				/*�������� ����� �������� ����� ����,
	//				���� �� NULL �� ������ ��� �������*/
	//				if (GetWindow (hCurrentWindow, GW_OWNER) == NULL) {
 //                      //������� �������� ����
	//					*(ptrNameWindow + wcslen (ptrNameWindow)) = '\n';
	//					*(ptrNameWindow + wcslen (ptrNameWindow)) = 0x0;
	//					TRACE (ptrNameWindow);
	//				}
	//				else
	//					;
	//			}
	//			else
	//				;
	//		}
	//		else
	//			;
	//	}
	//	else
	//		;

	//	hCurrentWindow = GetNextWindow (hCurrentWindow, GW_HWNDNEXT);
 
	//}
	//while (hCurrentWindow != NULL);
	//
	//if (! (hSnapshot == INVALID_HANDLE_VALUE)) {
	//	try { CloseHandle (hSnapshot); }
	//	catch (CException *err) {
	//		TRACE ("CloseHandle (hSnapshot) = except");		
	//	}
	//}
	//else
	//	;

	//TRY { CloseHandle (hCurrentWindow); }
	//CATCH_ALL (err) {
	//	TRACE ("CloseHandle (hCurrentWindow) = except");
	//	THROW ();
	//}
	//END_CATCH_ALL

	//��������� ��������
	i = 1;
	
	//������������ ������
	/*i = -1, val = 666;
	char *pch = DEBUG_NEW char [sizeof (int) + 1];
	memset (pch, 0x0, sizeof (int) + 1);
	
	i = sizeof (int);
	while (i > 0)
	    *(pch + (i -- - 1)) = *(((char *) &val) + (i - 1));
	
	i = sizeof (int);
	val = -1;
	while (i > 0)
	    *(((char *) &val) + (i -- - 1)) = *(pch + (i - 1));
	
	delete pch;*/
	
	//HTemp *pTmp1 = NULL, *pTmp2 = NULL, *pTmp3 = NULL;
	//pTmp1 = DEBUG_NEW HTemp ();
	//pTmp2 = DEBUG_NEW HTemp (666);
	//pTmp3 = DEBUG_NEW HTemp (-666.666);
	
	//BOOL bRes = false, bVal = false;
	//LPSTR nameSignal;
	//int iRes = -1;	
	//
	//CMap <LPSTR, LPSTR, BOOL, BOOL> mapSignalAvg;
	//    
 //   mapSignalAvg.SetAt ("LBA10CP001", true); //���� ������� ���� �� ������-�
 //   mapSignalAvg.SetAt ("LBA20CP001", false); //���� ������� ���� �� ������-�
 //   mapSignalAvg.SetAt ("LAY50CP001", true); //�������� ����������� ���� � ������
    //    
 //   nameSignal = "LBA20CP001";
 //   bRes = mapSignalAvg.Lookup (nameSignal, bVal);

	FILELOG_VARIABLE

    m_iModeApp = 0;
    m_tmSpanUpdate = 0;

    m_ptrMenuTray = 0x0;

    if (WSA_IS_INIT) {
		int iRes = -1;

        for (ID_HANDLE_EVENT idEvt = DIRECTORY_WATCHED; idEvt < ID_HANDLE_EVENT::COUNT_HANDLE_EVENT; idEvt++) {
			boolean bState = true;
			switch (idEvt) {
				case DIRECTORY_WATCHED:
					break;
				/*case GETDATA_05_MIN:
				case GETDATA_30_MIN:
					bState = false;
					break;*/
                case WHOIS:
                    bState = false;
					break;
				default: ;
			}
			m_arHandleEvent [idEvt] = CreateEvent (NULL, bState, true, NULL);
		}

        //Thread32First\Next
		//lpCreationTime �� GetThreadTimes

		//����������� ����������� ���������� ���������
        HANDLE hProcess = 0x0;
		DWORD idProcess = 0x0, idThreadApp, idCurrentProcess = GetCurrentProcessId ();
		idProcess = findIdProcess (CString (m_strAppName.GetBuffer ()), idCurrentProcess);
		if ((! (idProcess == -1)) && (! (idProcess == idCurrentProcess))) {
            /*idThreadApp = findIdThreadAppOfProcess (idProcess);

            ::PostThreadMessage (idThreadApp, WM_USER_WHOIS, m_nThreadID, 0);*/

            /*switch (WaitForSingleObject (APP_HANDLE_EVENT (ID_HANDLE_EVENT::WHOIS), 66)) {
                case WAIT_ABANDONED:
		        case WAIT_TIMEOUT:*/
                    hProcess = NULL;
			        //try { hProcess = OpenProcess (PROCESS_TERMINATE, true, idProcess); }
			        try { hProcess = OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, true, idProcess); }
			        catch (CException *err)	{
				        //err->
				        //cout << err->ReportError (err->GetErrorMessage ()) << endl;
				        TRACE (CString ("������!").GetBuffer ());
                        /*::MessageBoxA (NULL, "������� �� ������!", "���������", MB_ICONHAND)*/;
			        }

			        if (hProcess) {
				        TRY { iRes = TerminateProcess (hProcess, 0x0); }
				        CATCH_ALL (err) {
					        TRACE (CString ("������ 'TerminateProcess'!").GetBuffer ());
				        }
				        END_CATCH_ALL

				        int idError =-1;
                        if (! (iRes == 0)) {
					        idError = GetLastError ();
                            //iRes = 0;
                        }
                        else	        
				            /*::MessageBoxA (NULL, "������� ��������", "���������", MB_ICONHAND)*/;

				        CLOSE_HANDLE (hProcess);
	            
				        //if ()
			        }
			        else
				        /*::MessageBoxA (NULL, "HANDLE �������� = 0x0!", "���������", MB_ICONHAND)*/; //Error: �� ������� ������� �������
			        /*break;
                case WAIT_OBJECT_0:
                    break;
                default: ;
            }*/
		}
		else
			/*::MessageBoxA (NULL, "������� �� ������!", "���������", MB_ICONHAND)*/; //OK: �� ���ĸ� �������

		if (iRes) {
            Sleep (666);

            IMPLEMENT_HTHREAD_FILELOG
        }
        else ;
	}
	else {
		//TRACE ("WSAStartup �� ����� ���� ��������. ��� ��������: %d\n", iRes);
	}

    //_� � � � � � �_
	//HFileInfo *phfi;
	//HFILEINFO (phfi, "C:\\temp.txt");
}

HWinAppKTS::~HWinAppKTS () {
    WNDENUMPROC fWndProc = NULL;
	LPARAM lpWndProc = 0x0;

	//::GetWindowThreadProcessId (

	//fWndProc = fWndEnumProcCloseErr;
	//lpWndProc = (LPARAM) GetAppName ().GetBuffer ();
	//EnumThreadWindows (GetCurrentThreadId (), fWndProc, lpWndProc);

	/*Process32First (
	Thread32First*/

	fWndProc = fWndEnumProcCloseErr;
	lpWndProc = (LPARAM) m_strAppName.GetBuffer ();
	EnumWindows (fWndProc, lpWndProc);

    delete PTR_DATETIMEINTERVAL;
	
	FILELOG_VARIABLE
	
	strLog.format (_T ("���������� ������� ���������� %S!"), _T ("HWinAppKTS"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HCOMMON);
}

BOOL HWinAppKTS::InitInstance (void) {
	// TODO: ��������� ����� �������������� ������������� �������
    FILELOG_VARIABLE
    int iRes = -1;

    if (HWinApp::InitInstance ()) {
        if (IS_INIT_HTHREAD_FILELOG) {
            FILELOG_WRITE_WITHDT (_T ("������� ������������� ��!"), HDATABASE);

            iRes = InitializeDB ();
            if (iRes == 0) {
                iRes = SetParametrLogFile ();

                InitializeIntervals ();
            
                iRes = SetModeApp ();
            }
            else {
                //MessageBox (NULL, _T ("������ ������������� ��!"), _T("��������!"), MB_ICONINFORMATION);
                FILELOG_WRITE_WITHDT (_T ("������ ������������� ��!"), HDATABASE);
            }
        }
        else
            ;
    }
    else
		//��� ���������� ������ ������� �� ��������� (��������������) ������
        ; 
		
	if (iRes == 0)
        return TRUE;
    else
        return FALSE;
}

int HWinAppKTS::ExitInstance (void) {
	// TODO: ��������� ����� ����� �������������� ������� �������
    delete m_ptrDBSettings;

    FreeMemoryDBObject ((CObject *&) m_pDB);

	for (ID_HANDLE_EVENT idEvt = DIRECTORY_WATCHED; idEvt < ID_HANDLE_EVENT::COUNT_HANDLE_EVENT; idEvt++) {
		CLOSE_HANDLE (m_arHandleEvent [idEvt])
	}

    if (m_ptrMenuTray && m_ptrMenuTray->m_hMenu != INVALID_HANDLE_VALUE)
        while (m_ptrMenuTray->GetMenuItemCount ()) {
            m_ptrMenuTray->RemoveMenu (0, MF_BYPOSITION);
        }
    else ;

	delete m_ptrMenuTray;

    int iRes = HWinApp::ExitInstance ();
	return iRes;
}

// ����������� ��������� HWinAppKTS
int HWinAppKTS::InitializeDB (void) {
    int i = -1, iRes = 0; //Success

    HString strPathApp, strFileNameApp, strPathDB, strFileNameDB, strWildcard;
    FILELOG_VARIABLE

	strLog.format ("���� � 'InitializeDB'");
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATABASE);

	//����� � ��������� ������ ���������� ���� �� ����� � ��
	CCommandLineInfo cmdLineInfo;
	ParseCommandLine (cmdLineInfo);
	//cmdLineInfo.ParseParam ("FileNameDB", false, false);

	//�� ��������� ���� � �� � ����� �������� � '*.exe'
	strPathApp = GetAppDirectory ();
	strFileNameApp = GetAppName ();

    cmdLineInfo.m_strFileName = parseParam ("db_cfg").GetBuffer ();

	if (! cmdLineInfo.m_strFileName.IsEmpty ())	{
	    strFileNameDB = HString (cmdLineInfo.m_strFileName.GetBuffer ());

        strWildcard = strFileNameDB.left (strFileNameDB.reverseFind ('\\') + 1); //������ � '\\'
        strFileNameDB = strFileNameDB.right (strFileNameDB.length () - (strFileNameDB.reverseFind ('\\') + 1));
	}
	else {
	    //strFileNameDB = strFileNameApp;
	    //int iRes = strFileNameDB.replace (_T (".exe"), _T (".mdb"));

        strWildcard = strPathApp;
	    strWildcard.appendChar ('\\');

        strFileNameDB = _T ("UserKTS.mdb");
    }

	//����� ����� � ��

    strWildcard.append (strFileNameDB);

	CFileFind finder;
    // start working for files
    finder.Close ();
    BOOL bWorking = finder.FindFile (strWildcard.GetBuffer ());
	finder.Close();

	strLog.format (_T ("InitializeDB: �������� ������: %S"), WC_HSTRING (strWildcard));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HCOMMON);

    if (bWorking == 0) {
		//� ������� ���������� ��� ����� ��
		//���������� ������������� � ������� ����������
		strLog.format ("� ������� ���������� ��� ����� � ��.");
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HCOMMON);

		strPathDB = ReFindFile (strPathApp, strFileNameDB);
#if defined _DEBUG
		while (strPathDB.isUses () == 0) {
			//������� �� ������� �����
			//� ���������� �������������
			i = strPathApp.reverseFind ('\\');
			if (i < 1) {
				strPathApp = _T ("");
				break;
			}
			strPathApp = strPathApp.left (i);
			
			if (strPathApp.isUses () == 0)
			    break;
            else
                ;

			strPathDB = ReFindFile (strPathApp, strFileNameDB);
		}
#else
#endif
	}
	else {
	    strPathDB = strWildcard.left (strWildcard.reverseFind ('\\') + 0); //��� '\\'
	}

	if (strPathDB.isUses () == 1) {		
		m_strFullPathDB = strPathDB;
		m_strFullPathDB.appendChar ('\\');
		m_strFullPathDB.append (strFileNameDB);

	    strLog.format (_T ("������ ���� � �� %S."), WC_HSTRING (m_strFullPathDB));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HCOMMON);
		
        m_ptrDBSettings = DEBUG_NEW HDBSettings ();
		iRes = SetConnectDB ();
	}
	else {
	    iRes = -1;
	}

	strPathApp.free (); strFileNameApp.free (); strPathDB.free (); strFileNameDB.free (); strLog.free (); strWildcard.free ();

	return iRes;
}

int HWinAppKTS::SetParametrLogFile (void) {
    int iRes = 0, iOptionValue = -1;
    if (PTR_DB) {
        iOptionValue = PTR_SETTINGS->GetOption (HString (_T ("�����_�_�������_����������"))).number ();
        //PTR_FILELOG->SetMaxCountRecord (iOptionValue);
        //bRes = PTR_HTHREAD_FILELOG->PostThreadMessage (WM_USER_MAXCOUNTRECORD, (WPARAM) &iOptionValue, 0);
        ::PostThreadMessage (PTR_HTHREAD_FILELOG->m_nThreadID, WM_APP_MAXCOUNTRECORD, (WPARAM) &iOptionValue, 0);
        iOptionValue = PTR_SETTINGS->GetOption (HString (_T ("�����_�_�������_����������"))).number ();
        //PTR_FILELOG->SetLevelOutputInfo (iOptionValue);
        //bRes = PTR_HTHREAD_FILELOG->PostThreadMessage (WM_USER_LEVELOUTPUTINFO, (WPARAM) &iOptionValue, 0);
        ::PostThreadMessage (PTR_HTHREAD_FILELOG->m_nThreadID, WM_APP_LEVELOUTPUTINFO, (WPARAM) &iOptionValue, 0);
    }
    else
        ;

    return iRes;
}

int HWinAppKTS::InitializeIntervals (void) {
    int iRes = 0; //Success
    FILELOG_VARIABLE
    HString strTmp;
    
    strTmp.format ("���� � 'InitializeIntervals...");
    FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), HCOMMON);
    
    //�������� ������ �������� ������� �� ������ ����. 'Template'
    if (PTR_DB) {
        HRecordset *pRSTime = NULL;
        pRSTime = PTR_DB->GetRecordset ( this, HString (_T (TABLENAME_TIME)));

        if (pRSTime) {
            if (pRSTime->CanScroll ())
                pRSTime->MoveFirst ();
            else
                ;
            
            while (! pRSTime->IsEOF ()) {
                HString strIdName;
                iRes = pRSTime->GetFieldValue (_T ("ID_NAME"), strIdName);
                if (iRes) iRes = 0; else ;
                PTR_DATETIMEINTERVAL->AddInterval ((int ) pRSTime->GetFieldValue (_T ("ID")).m_lVal, strIdName, (int ) pRSTime->GetFieldValue (_T ("INTERVAL_SECONDS")).m_lVal);
                
                pRSTime->MoveNext ();
            }
        }
        else
            ;

        PTR_DB->ReleaseRS (this, HRECORDSET (pRSTime), true, true);
    }
    
    strTmp.format ("����� �� 'InitializeIntervals... � ����� %i", iRes);
    FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), HCOMMON);
    
    return iRes;
}

//int HWinAppKTS::SetConnectDB (HString &pathDB, HDatabase::DRIVER_DB typeDB) {
int HWinAppKTS::SetConnectDB (void) {
    int iRes = -1;
    
    if (m_pDB == NULL)
        m_pDB = DEBUG_NEW HDatabase ();
	else
		;

	//�������� ���������� � ��
	iRes = m_pDB->Connect (m_strFullPathDB, HDatabase::MS_ACCESS);

	if (iRes == 0)
		;
	else {
		FreeMemoryDBObject ((CObject *&) m_pDB);

        Sleep (666);

        m_pDB = DEBUG_NEW HDatabase ();
    }

	return iRes;
}

void HWinAppKTS::DisconnectDB (void) {
    if (! (m_pDB == NULL))
        m_pDB->Disconnect ();
	else
		;
}

int HWinAppKTS::SetModeApp (void) {
    FILELOG_VARIABLE

    int iRes = 0; //Success

    //�������������, ������������
    switch (m_ptrDBSettings->GetOption (HString (_T ("�����_������"))).number ()) {
        case 0:
            m_iModeApp |= HWinAppKTS::ID_MODE_APP::MODE_VIEW;
            break;
        case 1:
            m_iModeApp |= HWinAppKTS::ID_MODE_APP::MODE_ADMIN;
            break;
        default:
            ;
    }

    //��������, ��������
    if (GetHostName ().compareNoCase (m_ptrDBSettings->GetOption (HString (_T ("HostNameTarget")))) == 0)
        m_iModeApp |= HWinAppKTS::ID_MODE_APP::MODE_LOCAL;
    else
        m_iModeApp |= HWinAppKTS::ID_MODE_APP::MODE_REMOTE; //ID_MODE_APP::MODE_LOCAL - � �������� ������� �� ��

    //����� �� ��������� �� ���./������
    HString strModesApp = parseParam ("modeapp")
        , strMode;
    int pos = -1;
    strMode = strModesApp.tokenize (",", pos);

    while (! (pos < 0)) {
        if (strMode.compareNoCase ("disabled_autocreate") == 0)
            m_iModeApp |= HWinAppKTS::ID_MODE_APP::MODE_DISABLED_AUTOCREATE;
        else
            ;

        strMode = strModesApp.tokenize (",", pos);
    }

    return iRes;
}

int HWinAppKTS::ManagerIconTray (HWND hWnd, int action, HICON hIcon) {
    int iRes = 0;

    CString strToolTipIconTray;

    NOTIFYICONDATA nid;
    nid.cbSize = sizeof (NOTIFYICONDATA);
    nid.hWnd = hWnd;

    nid.uID = 1;

    switch (action) {
        case NIM_ADD:
            if (hIcon == 0x0) {
                //��������� HICON ������� �1
                //nid.hIcon = m_hIcon;
                //��������� HICON ������� �2
                //nid.hIcon = (HICON) GetClassLong (hWnd, GCL_HICON);
                //��������� HICON ������� �3
                /*TCHAR szClassName [MAX_PATH];
                GetClassName (hWnd, szClassName, MAX_PATH);
                WNDCLASS wc;
                GetClassInfo (m_hInstance, szClassName, &wc);
                nid.hIcon = wc.hIcon;*/


                //DWORD ProcessID = GetWindowThreadProcessId (hWnd, &ProcessID);
            }
            else
                nid.hIcon = hIcon;

            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid.uCallbackMessage = WM_USER_MESSAGE_ICON_TRAY;
            
            memset (nid.szTip, 0x0, 128 * sizeof (WCHAR));
            memset (nid.szInfo, 0x0, 256 * sizeof (WCHAR));
            memset (nid.szInfoTitle, 0x0, 64 * sizeof (WCHAR));
            strToolTipIconTray = CString (WC_HSTRING (HWinApp::ResourceVersion (_T ("FileDescription"))));
            memcpy (nid.szTip, strToolTipIconTray.GetBuffer (), strToolTipIconTray.GetLength () * sizeof (WCHAR));
            break;
        case NIM_MODIFY:
            break;
        case NIM_DELETE:
            break;
        default:
            ;
    }
    
    Shell_NotifyIcon (action, &nid);

    return iRes;
}
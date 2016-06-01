// HThreadDirWatched.cpp : implementation file
//

#include "stdafx.h"
#include "UserKTS.h"
#include "HThreadCreateTemplate.h"
#include "HCheckStateTemplate.h"

#if defined PTR_DB
extern CUserKTSApp
#else
extern CWinApp
#endif
theApp;

EXT_DECLARE_HTHREAD_FILELOG

#ifdef _DEBUG
extern CMemoryState g_memStateDiff,
                    g_memStateBegin,
                    g_memStateEnd;
#endif

extern int randRange (int, int );

extern HString tokenizeTypeSource (int, HString &);
extern HString tokenizeConnectDB (int, HString &);

IMPLEMENT_DYNCREATE(HThreadCreateTemplate, HWinThread)

HThreadCreateTemplate::HThreadCreateTemplate () : HWinThread () {    
    //HDEBUG_NEW (m_pContentTemplateTmp, HContentTemplate, 1);
	//m_pContentTemplateTmp = DEBUG_NEW HContentTemplate ();
    m_pContentTemplateRun = NULL;
    
    /*m_idExitCodeThread = WAIT_IO_COMPLETION;*/
    
    FILELOG_VARIABLE
    FILELOG_IF_WRITE (THREAD_CREATE_TEMPLATE)
    strLog.format (_T ("����������� ������ 'HThreadCreateTemplate'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), THREAD_CREATE_TEMPLATE);
    FILELOG_ENDIF
}

HThreadCreateTemplate::~HThreadCreateTemplate () {
    FILELOG_VARIABLE

    HCheckStateTemplate *pCST = * (HCheckStateTemplate **) GetDataOfId (PTR_CHECKSTATETEPMLATE);
    //�� �������� ���������� ������ ����� ������������� ������ ??? �� ����� �������������, �� ����� ����������, �� ���� ������ !!!
    if (pCST) {
        strLog.format (_T ("~HThreadCreateTemplate () � ID = %i, pCST->ID = "), m_nThreadID);
        strLog.appendNumber ((DWORD) pCST->GetID ());
		strLog.append (_T ("..."));

        switch (WaitForSingleObject (pCST->m_hEvtOperation, 0x0)) {
			case WAIT_ABANDONED:
			case WAIT_TIMEOUT:
                if (SetEvent (pCST->m_hEvtOperation) == 1)
					strLog.append (_T ("�������, �������"));
				else
					strLog.append (_T ("�������, �� �������"));
				break;
			case WAIT_OBJECT_0:
				strLog.append (_T ("����������, ��������"));
				break;
			default:
				;
		}

        FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);
	    strLog.free ();
    }
    else {
        FILELOG_IF_WRITE (THREAD_CREATE_TEMPLATE)
        strLog.format ("������ ������� �� ������� (ID = %i)", m_nThreadID);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), THREAD_CREATE_TEMPLATE);
        FILELOG_ENDIF
    }

    FILELOG_IF_WRITE (HDEBUG)
    strLog.format (_T ("%S '%S'"), _T ("���������� ������"), WC_HSTRING (HString (GetRuntimeClass ()->m_lpszClassName)));
    //FILELOG_WRITE_WITHDT (strLog.GetBuffer (), THREAD_CREATE_TEMPLATE);
	FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);
    FILELOG_ENDIF
}

BOOL HThreadCreateTemplate::InitInstance () {
	// TODO:  perform and per-thread initialization here	
	return TRUE;
}

int HThreadCreateTemplate::ExitInstance () {
	// TODO:  perform any per-thread cleanup here
    FILELOG_VARIABLE

	return HWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(HThreadCreateTemplate, HWinThread)
	ON_THREAD_MESSAGE (WM_USER_THREAD_RESUME, &HThreadCreateTemplate::OnResumeRun)
	/*ON_THREAD_MESSAGE (WM_USER_THREAD_CONTENT_CREATE_TEMPLATE, OnResumeContent)
	ON_THREAD_MESSAGE (WM_USER_THREAD_CALCULATE_CREATE_TEMPLATE, OnResumeCalculate)
	ON_THREAD_MESSAGE (WM_USER_THREAD_GENERATE_CREATE_TEMPLATE, OnResumeGenerate)
	ON_THREAD_MESSAGE (WM_USER_THREAD_WRITE_CREATE_TEMPLATE, OnResumeWrite)*/
END_MESSAGE_MAP()

int HThreadCreateTemplate::AddData (HTREEITEM *phItem) {
    return HWinThread::AddData (HTREEITEM_TEMPLATE, (BYTE *) phItem, sizeof (HTREEITEM));
}

void HThreadCreateTemplate::GetContentTeplate (CPtrArray &ar) {
    m_pContentTemplateTmp->CopyContentTemplate (ar);
}

void HThreadCreateTemplate::FreeContentTemplate (void) {
    delete m_pContentTemplateTmp;
}

// HThreadDirWatched message handlers
//int HThreadCreateTemplate::Run ()
afx_msg void HThreadCreateTemplate::OnResumeRun (WPARAM , LPARAM ) {
    DWORD idExitCodeThread = m_idExitCodeThread;
    //CWinApp *theApp = ::AfxGetApp ();
    
    FILELOG_VARIABLE

//#ifdef _DEBUG    
//    CMemoryState memState;
//#endif
    
    //����� ����� 'link.exe', ������� ���������� error - ::Run () must return a value
    //�� ���������� ��������� ����� ������� 'AfxEndThread'
    //� ���� ����� ����� ����: warning - not all paths return a value
    if (! (idExitCodeThread == WAIT_IO_COMPLETION)) {
        idExitCodeThread = -1;
        //return idExitCodeThread;
    }
    else
        ;
        
    idExitCodeThread = STILL_ACTIVE;

    int iRes = 0,
        i = 0,
        iNumAlternative = 0,
        iPosExtSelectedItem = 0, iExt = -1,
        *piNumberOperation = ((int *) GetDataOfId (NUMBER_OPERATION)), iNumberOperation = *piNumberOperation,
        //*piIdProgressCtrl = ((int *) GetDataOfId (NUMBER_IDPROGRESSBAR)), iIdProgressCtrl = *piIdProgressCtrl,
        *arPosSelectedItem = (int *) GetDataOfId (PTR_ARPOSSELECTEDITEM);
        
    short shNumBeginSource = *(short *) GetDataOfId (SHORT_NUMBERBEGINSOURCE);
        
    CTimeSpan tmSpanData (*((LONGLONG *) GetDataOfId (SPAN_INTERVALDATA)));
    
    BOOL bAutoChangeSource (*((BOOL *) GetDataOfId (BOOL_AUTOCAHNGESOURCE)));
   
    CFile file;
    CFileFind filefind;
    
    //HTreeCtrlTemplate *pTreeCtrlTemplate = * (HTreeCtrlTemplate **) GetDataOfId (PTR_TREECTRLTEMPLATE);
    HCheckStateTemplate *pCST = * (HCheckStateTemplate **) GetDataOfId (PTR_CHECKSTATETEPMLATE);
    /*CString strTypeSource,
            strConnectDB,*/
    HString strExtSelectedItem,
            strDirectoryUndo = stringData (GetDataOfId (STRING_DIRECTORYUNDO)),
            arstrExtSelectedItem = stringData (GetDataOfId (STRING_AREXTSELECTEDITEM)),
            strFullPath, strFullPathUndo, strFileUndo, strTmp;
                
    if (! (strDirectoryUndo.reverseFind ('\\') == strDirectoryUndo.length () - 1))
        strDirectoryUndo.append ("\\");
	else
		;
	
    m_pContentTemplateRun = DEBUG_NEW HContentTemplate (pCST->GetID (), pCST->GetHTreeItem (), pCST->GetFullPathTemplate (), pCST->GetSpanModify (), pCST->GetSpanTask (), pCST->GetSpanWork (), pCST->GetIdInterval (), pCST->GetMinExt (), pCST->GetMaxExt (), true);
    
    if (m_pContentTemplateRun) {
        m_pContentTemplateRun->SetSpanData (tmSpanData);

		FILELOG_IF_WRITE (HDEBUG)
		strLog.format ("New 'HContentTemplate'...�����", m_nThreadID);
		FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
		FILELOG_ENDIF

        //m_pContentTemplateRun->CopyContentTemplate (m_m_pContentTemplateRunTmp->CopyContentTemplate ());
        int iRes = m_pContentTemplateRun->GetContentTemplate ();

        //���� 'HContentTemplate' ��� �� ����� ???
        //������ � 'ExitInstance' ���� ���� ����� ???
        //�� ���� 'ExitInstance' �� ���������� ???
        //FreeContentTemplate ();

        strTmp.free ();

		if (iRes == 0) {
			FILELOG_IF_WRITE (HDEBUG) //THREAD_CREATE_TEMPLATE
			strLog.format ("��������� �������� ���� �� ������...%i �� ��������� ������ � ��...�����...", m_nThreadID);
			FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG); //THREAD_CREATE_TEMPLATE
			FILELOG_ENDIF

			PostMessage (theApp.m_pMainWnd->m_hWnd, WM_USER_ENDWORKBD_CREATE_TEMPLATE, (WPARAM) m_nThreadID, NULL);

			FILELOG_IF_WRITE (HDEBUG) //THREAD_CREATE_TEMPLATE
			strTmp.format ("��������� �������� ���� �� ������...%i �� ������������� ������� ProgressBar's...", m_nThreadID);
			FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), HDEBUG); //THREAD_CREATE_TEMPLATE
			FILELOG_ENDIF
			strTmp.free ();

			PostMessage (::AfxGetMainWnd ()->m_hWnd, WM_USER_PROGRESS_RULES, (WPARAM) (DYNAMIC_DOWNCAST (HThreadCreateTemplate, ::AfxGetThread ()))->m_nThreadID, (GetLengthDataOfId (PTR_ARPOSSELECTEDITEM) - sizeof (long)) / sizeof (int) * (m_pContentTemplateRun->GetCountLine () - 3)); //3 - ����� ����� ������������ ����� ��������� � 'HContentTemplate::Calculate'

            FILELOG_IF_WRITE (HDEBUG) //THREAD_CREATE_TEMPLATE
			strTmp.format ("���� � ���� 'while' (���-�� ���������=%i) �� ��������� ��������� � ������...", sizeof (arPosSelectedItem));
			FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), HDEBUG); //THREAD_CREATE_TEMPLATE
			FILELOG_ENDIF
			strTmp.free ();

			while (arPosSelectedItem [i]) {
                FILELOG_IF_WRITE (HDEBUG) //THREAD_CREATE_TEMPLATE
			    strTmp.format ("��������� %i-�� �������� ������ (������� = )...", i, arPosSelectedItem [i]);
			    FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), HDEBUG); //THREAD_CREATE_TEMPLATE
			    FILELOG_ENDIF
			    strTmp.free ();

				//pTemplate->SetSpanData (stmBegin.wHour * 60 * 60 + stmBegin.wMinute * 60 + stmBegin.wSecond);

				CTime dtModifyInfoSample = pCST->GetDTModifyInfoSample (int (arPosSelectedItem [i] - 1));
				m_pContentTemplateRun->NewTarget (m_pContentTemplateRun->GetDTTarget (dtModifyInfoSample - pCST->GetIntervalVariable (dtModifyInfoSample, HDateTimeInterval::DIRECTION_DATETIME::PAST)));

				//������� �������� ������ � ������ ��������� ���������� ������ (�.�.�. � ��������������)
				iRes = -1;
				iNumAlternative = shNumBeginSource;
				do {
					//strConnectDB = ParseConnectDB (iNumAlternative, stringData (GetDataOfId (STRING_CONNECTDB)));
					//strTypeSource = ParseTypeSource (iNumAlternative, stringData (GetDataOfId (STRING_TYPESOURCE)));

					//if ((iNumAlternative > shNumBeginSource) && (iRes == ERR_NULL_VALUE))
					if (iNumAlternative > shNumBeginSource)
						PostMessage (::AfxGetMainWnd ()->m_hWnd, WM_USER_PROGRESS_RULES, (WPARAM) (DYNAMIC_DOWNCAST (HThreadCreateTemplate, ::AfxGetThread ()))->m_nThreadID, -1 * (m_pContentTemplateRun->GetCountLine () - 3)); //3 - ����� ����� ������������ ����� ��������� � 'HContentTemplate::Calculate'
					else
						;
	            
					//������� ������ ��� ���������� � ��, � ��� ��������� ����������
					iRes = m_pContentTemplateRun->CalculateTemplate (tokenizeConnectDB (iNumAlternative, stringData (GetDataOfId (STRING_CONNECTDB))), tokenizeTypeSource (iNumAlternative, stringData (GetDataOfId (STRING_TYPESOURCE))));

					iNumAlternative ++;

					//������� �1
					//if (iRes == ERR_BREAK)
					//    //����� ������ ����� (���������� �����) - ����������� ����������� !!!
					//    bAutoChangeSource = 0;
					//else
					//    ;
				}            
				//������� �1
				//while ( (! (iRes == ERR_BREAK)) &&
				//        (! (iRes == ERR_NULL_VALUE)) &&
				//        (! (iRes == ERR_PARSE_NULL)) &&
				//        (! (iRes == ERR_ARGS)) &&
				//        (! (iRes == 0)) &&
				//        bAutoChangeSource); //����� ��������� ???
				//������� �2
				while ( (! (iRes == ERR_BREAK)) &&
						(! (iRes == ERR_PARSE_NULL)) &&
						(! (iRes == 0)) &&
						bAutoChangeSource); //����� ��������� ???

				//if ((! strTypeSource.IsEmpty ()) && (! (iRes == 0)))
				if (! (iRes == 0)) {
					; //break; //���������� ������ ??? ��� ����� ???
					; //������ �������� � ������� ??? - ���������� ���� ������� ������ ???
					switch (iRes)
					{
						case ERR_BREAK:
							while (arPosSelectedItem [i])
								i ++;

							i --;
							break;
						default:
							;
					}
				}
				else
				{
					//������� ������ � ����� ��������� ���������� ��� ��� �� ����� ???
					iRes = m_pContentTemplateRun->GenerateContentTemplate ();
					if (iRes == 0)	{
						iExt = -1;
						iPosExtSelectedItem = 0;
						while ((iExt < i) && (! (iPosExtSelectedItem < 0))) {
							strExtSelectedItem = arstrExtSelectedItem.tokenize (_T (";"), iPosExtSelectedItem);
							iExt ++;
						}

						/*FILELOG_IF_WRITE (HDEBUG) //THREAD_CREATE_TEMPLATE
						strLog.format ("iPosExtSelectedItem = %i ��� 'arstrExtSelectedItem.Tokenize... %s'", iPosExtSelectedItem, MB_HSTRING (arstrExtSelectedItem));
						FILELOG_WRITE (strLog.GetBuffer (), HDEBUG); //THREAD_CREATE_TEMPLATE
						FILELOG_ENDIF*/

						//strFullPath = pTreeCtrlTemplate->GetItemFullPath (pCST->GetHTreeItem ());
						strFullPath = m_pContentTemplateRun->GetFullPathTemplate ();
						strFullPath.replace ("*", strExtSelectedItem.GetBuffer ());

						//DEBUG - ������� ���� ������ �� ����
						filefind.Close ();
						if (filefind.FindFile (strFullPath.GetBuffer ())) {
							CTime tmCurrent = CTime::GetCurrentTime ();

							strTmp.free ();
							strTmp.format ("%i", iNumberOperation - (PREFIX_IDM_OPERATION + 1));
							while (strTmp.length () < 2)
								strTmp.insert (0, "0");
							strFileUndo.append (strTmp);

							strFileUndo.appendChar ('_');

							strTmp.free ();
							strTmp.format ("%i", pCST->GetID ());
							while (strTmp.length () < 4)
								strTmp.insert (0, "0");

							strFileUndo.append (strTmp);

							//����� ��������/����������� �. �����...
							//strFileUndo.AppendChar ('_');

							//strTmp.Empty ();
							//strTmp.Format (_T("%s"), tmCurrent.Format (_T("%H%M%S")));
							//while (strTmp.GetLength () < 6) strTmp.Insert (0, '0');
							//    strFileUndo.Append (strTmp);

							strFileUndo.appendChar ('_');

							//������ ������� �. ������������ 'rand'
							strTmp.free ();
							strTmp.format ("%i", randRange (0, RAND_MAX));
							while (strTmp.length () < 6)
								strTmp.insert (0, "0");
							strFileUndo.append (strTmp);

							FILELOG_IF_WRITE (HDEBUG) //THREAD_CREATE_TEMPLATE
							strLog.format ("%s", MB_HSTRING (strFileUndo));
							FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG); //THREAD_CREATE_TEMPLATE
							FILELOG_ENDIF

							strFileUndo.appendChar ('.');

							strTmp.free();
							strTmp.format ("%s", MB_HSTRING (strExtSelectedItem));
							strFileUndo.append (strTmp);
	                        
							// ???
							if (strDirectoryUndo.length () == 0) {
								idExitCodeThread = -1;
								//ExitThread (((HThreadDescription *) lpParam)->idExitCodeThread);
								//return idExitCodeThread;
								//AfxEndThread (idExitCodeThread, m_bAutoDelete);
								AfxEndThread (idExitCodeThread, true);
							}
							else
								;

							//strFullPathUndo = strDirectoryUndo + strFileUndo;
							//strFullPathUndo.Append (strDirectoryUndo);
							//strFullPathUndo.Append (strFileUndo);
							strFullPathUndo.format ("%S%S", WC_HSTRING (strDirectoryUndo), WC_HSTRING (strFileUndo));
	                        
							//CFile::Remove (strFullPath);
							try { CFile::Rename (WC_HSTRING (strFullPath), WC_HSTRING (strFullPathUndo)); }
							catch (CFileException *err) {
								FILELOG_IF_WRITE (HERROR)
								strLog.format ("������ 'CFile::Rename' ����� �� %s � %s (%s)", MB_HSTRING (strFullPath), MB_HSTRING (strFullPathUndo), MB_HSTRING (HString (err->m_strFileName.GetBuffer ())));
								FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
								FILELOG_ENDIF
		                        
								//����� ��� ��������� ������
								strFullPath.free ();
							}
	                        
							if (strFullPath.isUses () == 1) {
								FILELOG_IF_WRITE (HDEBUG) //THREAD_CREATE_TEMPLATE
								//strTmp.Format (_T ("%s ��������� ��� %s%s"), strFullPath.GetBuffer (), strFullPathUndo.GetBuffer ());
								strLog.format ("%s ��������� ��� %s", MB_HSTRING (strFullPath), MB_HSTRING (strFullPathUndo));
								FILELOG_WRITE (strLog.GetBuffer (), HDEBUG); //THREAD_CREATE_TEMPLATE
								FILELOG_ENDIF
	                            
								//���������� ����������
								strFullPath.free ();
							}
							else
								;
	                        
							//������������ ������ ��. �����
							strFileUndo.free ();
							strFullPathUndo.free ();
						}
	                        
						if (! (m_pContentTemplateRun->WriteContentTemplate ()))
							; //������ ����� ��������� � ������� ???
						else
							; //������ ����� ��������� �������
					}
					else
						; //������������� ���������� ��������� � ������� ???
				}
	                
				i ++;
			}
		}
		else {
			FILELOG_IF_WRITE (HERROR)
			strLog.format ("��������� �������� ���� �� ������...%i �� ��������� ������ � ��...������...", m_nThreadID);
			FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
			FILELOG_ENDIF
		}

        FILELOG_IF_WRITE (HDEBUG) //THREAD_CREATE_TEMPLATE
        strLog.format ("������������ ������ ...����� ����������� ������...%i", m_nThreadID);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG); //THREAD_CREATE_TEMPLATE
        FILELOG_ENDIF

        delete m_pContentTemplateRun;
        m_pContentTemplateRun = NULL;
    }
	else {//�� ������ ������-������
		FILELOG_IF_WRITE (HDEBUG)
		strLog.format ("New 'HContentTemplate'...������", m_nThreadID);
		FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
		FILELOG_ENDIF
	}

    /*strConnectDB.Empty ();
    strTypeSource.Empty ();*/
    strExtSelectedItem.free ();
    arstrExtSelectedItem.free ();
    strDirectoryUndo.free ();
    
    FILELOG_IF_WRITE (HCOMMON)
    strLog.format ("��������� �������� ���� (HANDLE = %i) � ����������� ������...%i, ���-� = %i", m_pMainWnd->m_hWnd, m_nThreadID, iRes);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HCOMMON);
    FILELOG_ENDIF

    PostMessage (m_pMainWnd->m_hWnd, WM_USER_EXIT_CREATE_TEMPLATE, (WPARAM) m_nThreadID, NULL);
    
#ifdef _DEBUG
    g_memStateEnd.Checkpoint ();
    if (g_memStateDiff.Difference (g_memStateBegin, g_memStateEnd))
    {
        g_memStateDiff.DumpAllObjectsSince ();
        g_memStateDiff.DumpStatistics ();
    }
    else
        ;
#endif
    
	idExitCodeThread = 0x0;

	//������� �-3
	//ExitThread (((HThreadDescription *) lpParam)->idExitCodeThread);
	//������� �-2
	//return ((HThreadDescription *) lpParam)->idExitCodeThread;
	
	//������� �-1
    //PostQuitMessage (idExitCodeThread);

	//������� �1
	//???return idExitCodeThread;
    //������� �2
	//AfxEndThread (idExitCodeThread, m_bAutoDelete);
}
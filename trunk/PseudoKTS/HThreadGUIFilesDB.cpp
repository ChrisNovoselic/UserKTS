#include "StdAfx.h"
#include "HThreadGUIFilesDB.h"

//Для theApp ('GetPtrThread')
#include "PseudoKTS.h"

//enumm TYPE_DATAID
#include <HThreadDirectoryWatched.h>

EXT_DECLARE_HTHREAD_FILELOG

#ifdef _DEBUG
extern CMemoryState g_memStateDiff,
                    g_memStateBegin,
                    g_memStateEnd;
#endif

IMPLEMENT_DYNCREATE (HThreadGUIFilesDB, HWinThread)

BEGIN_MESSAGE_MAP(HThreadGUIFilesDB, HWinThread)
	ON_THREAD_MESSAGE (WM_USER_THREAD_RESUME, &HThreadGUIFilesDB::OnResumeRun)
    //ON_THREAD_MESSAGE (WM_USER_CONNECT_TO_SERVER, &OnConnectToServer)
    ON_THREAD_MESSAGE (WM_USER_QUERY_CONNECT_TO_SERVER, &OnMessageQueryDataConnectToServer)
    ON_THREAD_MESSAGE (WM_USER_MESSAGE_DIR_WATCH, &OnMessageDirectoryWatched)
    ON_THREAD_MESSAGE (WM_USER_TIMER, &OnMessageTimer)
    ON_THREAD_MESSAGE (WM_USER_INIT_COMBOBOX_DIRWATCH, &OnMessageInitComboboxDirWatch)
END_MESSAGE_MAP()

HThreadGUIFilesDB::HThreadGUIFilesDB (void) : HWinThread ()/*, m_pDlgGUI (0x0)*/ {
}

HThreadGUIFilesDB::~HThreadGUIFilesDB (void) {
    //За этим следит САМ поток 'm_pMainWnd' ???
    if (dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)) {
        dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->DestroyWindow ();

        delete dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd);
    }
    else
        ;
}

int HThreadGUIFilesDB::InitInstance (void) {
    FILELOG_VARIABLE

	HWinThread::InitInstance ();

    strLog.format ("HThreadGUIFilesDB::InitInstance...");
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG/*COMMON_KTS_APP*/)
    strLog.free ();

    m_idExitCodeThread = STILL_ACTIVE;

	return TRUE;
}

int HThreadGUIFilesDB::ExitInstance (void) {
    if (dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->IsModal ())
        dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->EndDialog (m_idExitCodeThread);
    else
        dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->SendMessage (WM_DESTROY, m_idExitCodeThread);

	return HWinThread::ExitInstance ();
}

afx_msg void HThreadGUIFilesDB::OnResumeRun (WPARAM, LPARAM ) {
    CRuntimeClass* pDlgClass = RUNTIME_CLASS (HDlgGUIFilesDB);
    m_pMainWnd = (CWnd *) pDlgClass->CreateObject ();
	ASSERT (m_pMainWnd);
	if (m_pMainWnd->IsKindOf(pDlgClass))
		dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->PreCreate (CRect (0, 0, 360, 180));
	else
		;
    
    if (dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->IsModal ()) {
        /*INT_PTR nResponse = -1;
        nResponse = */dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->DoModal ();

        /*if (nResponse == IDOK) {
		    // TODO: Place code here to handle when the dialog is dismissed with OK
	    }
	    else if (nResponse == IDCANCEL)	{
		    // TODO: Place code here to handle when the dialog is dismissed with Cancel
	    }*/
    }
    else {
        dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->ShowWindow (SW_SHOW);
    }

    //m_pMainWnd = m_pDlgGUI; //???
}

afx_msg void HThreadGUIFilesDB::OnMessageDirectoryWatched (WPARAM ptrId, LPARAM bytes) {
    FILELOG_VARIABLE

#if defined _DEBUG
    /*CMemoryState memStateBegin, memStateEnd, memStateDiff;
    memStateBegin.Checkpoint ();*/
#endif

    if (bytes > 0) {
        //BOOL bWorking;
	    DWORD bytesDiff = bytes;
	    PFILE_NOTIFY_INFORMATION pFNI = NULL;
        //BYTE *pFNITotal = 0x0;

        FILELOG_VARIABLE
	    //CString strFullPath, strNameTemplate, strExt;
	    HString pwchDirWatched;
	    LPWSTR pwchNameTemplateOrigin = NULL, pwchExt = NULL, pwchFullPath = NULL;
	    enum {LEN_NAME, LEN_EXT};
        DWORD arLengthTemplate [2]; arLengthTemplate [LEN_NAME] = 0; arLengthTemplate [LEN_EXT] = 3;

	    int i, iItem = -1, idItem = 0, iCountRec = 0;

	    HCheckStateTemplate *pCST = NULL;

	    SYSTEMTIME sTime;

	    CFileFind filefind;
	    CFileStatus fStatus;

	    NMHDR hdr;

	    LVITEM lvItem;
	    lvItem.mask = LVIF_STATE;
        lvItem.stateMask = LVIS_SELECTED;
    	
	    //Информация о ПОТОКе
	    HThreadDirectoryWatched *pThreadDirWatched = (HThreadDirectoryWatched *) ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_DIRECTORYWATCHED);

	    //if (indxThreadDirWatched > -1)
	    //if (indxThreadDirWatched < m_arDescThreadDirWatched.GetSize ())
	    if (pThreadDirWatched) {
            /*strLog.format ("'::OnCommandThreadDirWatch...поток с id = %i, получено байт: %i", pThreadDirWatched->m_hThread, bytes);
            FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
            strLog.free ();*/
            
            pFNI = (FILE_NOTIFY_INFORMATION *) pThreadDirWatched->getFNI ();
            /*pFNITotal = DEBUG_NEW BYTE [bytes + 1];
            memset (pFNITotal, 0x0, bytes + 1);
            memcpy (pFNITotal, pThreadDirWatched->getFNI (), bytes);
            pFNI = (FILE_NOTIFY_INFORMATION *) pFNITotal;*/
            
            pwchDirWatched = pThreadDirWatched->stringDataOfId (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED);
            
            /*strLog.format (_T ("'::OnCommandThreadDirWatch...целевой каталог = %S"), WC_HSTRING (pwchDirWatched));
            FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), THREAD_WATCH_DIRECTORY);
            strLog.free ();*/

            do {
                arLengthTemplate [LEN_NAME] = pFNI->FileNameLength;
                
                //memcpy (&arLengthTemplate [LEN_NAME], ((DWORD *) pFNI) + 2, sizeof (DWORD));

                /*i = 0;
                while (i < sizeof (DWORD)) {
                    memcpy (((BYTE *) &arLengthTemplate [LEN_NAME]) + i, ((BYTE *) (((DWORD *) pFNI) + 2)) + i, 1);
                    i ++;
                }*/

                if (! (arLengthTemplate [LEN_NAME] == pFNI->FileNameLength)) {
                    strLog.format ("'::OnCommandThreadDirWatch...ТУПая ОШИБКа (%i == %i)", arLengthTemplate [LEN_NAME], pFNI->FileNameLength);
					FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
					strLog.free ();
                }
                else
                    ;

                arLengthTemplate [LEN_NAME] /= sizeof (WCHAR);

                if ((arLengthTemplate [LEN_NAME] > bytes) || (! (arLengthTemplate [LEN_NAME] > 0))) {
                    strLog.format ("'::OnCommandThreadDirWatch...BREAK: shLenNameTemplate [LEN_NAME] = %i", arLengthTemplate [LEN_NAME]);
					FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
					strLog.free ();

					break;
				}
                else
                    ;

                //pFNI->FileName [pFNI->FileNameLength / 2] = 0x0;
                //Имя файла у нас дается относительно папки
                //т.е.если изменится файл 'strCurDir'\File\test.dat, то FileName
                //будет равно File\test.dat, где 'strCurDir' - исследуемый каталог

                //strNameTemplate = CString (pFNI->FileName);

                pwchNameTemplateOrigin = DEBUG_NEW WCHAR [arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1) + 1];
                pwchExt = DEBUG_NEW WCHAR [arLengthTemplate [LEN_EXT] + 1];
                i = arLengthTemplate [LEN_NAME];
                while (i) {   
                    if (i < (arLengthTemplate [LEN_EXT] + 1)) {
                        pwchExt [arLengthTemplate [LEN_EXT] - i] = *(((WCHAR *) pFNI->FileName) + (arLengthTemplate [LEN_NAME] - i));
                        //if (i == shLenExt)
                        //    pwchNameTemplateOrigin [shLenNameTemplate [0] - i] = '*';
                        //else
                        //    ;
                    }
                    else {
                        //strNameTemplate.appendChar (*(((WCHAR *) pFNI->FileName) + (pFNI->FileNameLength - shLenNameTemplate [0])));
                        if ((! (i == arLengthTemplate [LEN_EXT])) && ((! (i == (arLengthTemplate [LEN_EXT] + 1)))))
                            pwchNameTemplateOrigin [arLengthTemplate [LEN_NAME] - i] = *(((WCHAR *) pFNI->FileName) + (arLengthTemplate [LEN_NAME] - i));
                        else
                            ;
                    }
                    
                    i --;
                }

                pwchNameTemplateOrigin [arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1)] = 0x0; _wcsupr (pwchNameTemplateOrigin);
                pwchExt [arLengthTemplate [LEN_EXT]] = 0x0; _wcsupr (pwchExt);

                if (_wcsicmp (pwchExt, _T ("ENG")) == 0) {
		            FREE_PTR (pwchFullPath)                    
		            pwchFullPath = _wcsadd (WC_HSTRING (pwchDirWatched), _T ("\\"));

                    iItem = 0;
                    i = 0;
                    //if ((shLenNameTemplate [0] - (shLenExt + 1)) > 6) {
                    if (wcsstr (pwchNameTemplateOrigin, _T ("PRO"))) {
                        //Только если длина имени файла > 6 (SutPro*.eng, KanPro*.eng) , НО НЕ 'OKey30.eng'
                        while (IsNumber (pwchNameTemplateOrigin [arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1) - (i + 1)])) {
                            iItem += (pwchNameTemplateOrigin [arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1) - (i + 1)] - 48 ) * pow (10.0, i);
                            i ++;
                        }
                    }
                    else
                        ;

                    LPWSTR pwchNameTemplate = 0x0;
                    if (i > 0) {
                        pwchNameTemplate = DEBUG_NEW WCHAR [arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1) + 1 - i + 1]; //1-я '+1' для 'NULL terminate', 2-я '+1' для '*'
                        memset (pwchNameTemplate, 0x0, (arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1) + 1 - i + 1) * sizeof (WCHAR));
                        memcpy (pwchNameTemplate, pwchNameTemplateOrigin, (arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1) + 0 - i) * sizeof (WCHAR));
                        //pwchNameTemplate [shLenNameTemplate [0] - (shLenExt + 1) + 1 - i] = 0;
                        pwchNameTemplate [arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1) + 1 - i - 1] = '*';
                    }
                    else {
                        pwchNameTemplate = DEBUG_NEW WCHAR [arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1) + 1];
                        memset (pwchNameTemplate, 0x0, (arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1) + 1) * sizeof (WCHAR));
                        memcpy (pwchNameTemplate, pwchNameTemplateOrigin, (arLengthTemplate [LEN_NAME] - (arLengthTemplate [LEN_EXT] + 1)) * sizeof (WCHAR));
                    }

		            pwchFullPath = _wcsadd (pwchFullPath, pwchNameTemplate, true);
		            pwchFullPath = _wcsadd (pwchFullPath, _T (".eng"), true);

		            FREE_PTR (pwchNameTemplate)

#if defined _DEBUG
    //MEMORY_LEAKED (_T ("Set pwchFullPath"))
#endif

			        //pCST = ((CTreeCtrl *) m_pDlgGUI->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetObjectCheckStateTemplate (m_arDescThreadDirWatched.GetAt (indxThreadDirWatched)->stringData (m_arDescThreadDirWatched.GetAt (indxThreadDirWatched)->GetDataOfNumber (1)) + CString (_T("\\")) + strNameTemplate);
			        pCST = ((HTreeCtrlKTS *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetObjectCheckStateTemplate (pwchFullPath);
			    
#if defined _DEBUG
    //MEMORY_LEAKED (_T ("Get 'HObjectCheckStateTemplate'"))
#endif

			        if (pCST) {
				        //По 'ID' найдём ~ об'ект 'HCheckStateTemplate'
				        //pCST = ((CTreeCtrl *) m_pDlgGUI->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetObjectCheckStateTemplate (idItem);
				        //В 'pCST' есть 1) 'HTREEITEM'; 2) 'ID_OWNER'
				        //А где проверка 'pCST' на NULL ??? - ВЫШе !!!
				        if (((CTreeCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetSelectedItem () == pCST->GetHTreeItem ()) {
							//ШАБЛОН в котором произошли изменения ОТОБРАЖАЕТСЯ - ПРОБЛЕМЫ !!!
				            //if (pCST->IsUpdate ())
				            //{
					            //strFullPath.Replace (_T ("*"), CString (pwchExt));
					            FREE_PTR (pwchFullPath)
					            pwchFullPath = _wcsadd (WC_HSTRING (pwchDirWatched), _T ("\\"));
		                        pwchFullPath = _wcsadd (pwchFullPath, pwchNameTemplateOrigin, true);
		                        pwchFullPath = _wcsadd (pwchFullPath, _T ("."), true);
		                        pwchFullPath = _wcsadd (pwchFullPath, pwchExt, true);
		                    
#if defined _DEBUG
    //MEMORY_LEAKED (_T ("Set pwchFullPath"))
#endif
					        
					            //Определим МЕСТО искомого файла в 'm_ListItemFiles'
                                //'iItem' определяю выше по ТЕМ же цифрам в ИМЕНи файла
					            /*iItem = 0;
                                if (pCST->GetMaxExt () > 0)
                                    //Определить номер по цифрам в ИМЕНи файла
                                    ;
                                else ;*/

#if defined _DEBUG
    //MEMORY_LEAKED (_T ("pCST->GetPosItemOfExt"))
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
            				                        
										            dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->SetListBoxItemFilesText (pCST, iItem, false);
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
            				                
								            dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->SetListBoxItemFilesText (pCST, iItem, false);
								            break;
							            case FILE_ACTION_RENAMED_OLD_NAME: //Файл был переименован и в имени файла - предыдущее имя (4)
								            //Определим номер РАСШИРЕНИя
								            //iNumExtPrev = _wtoi ((strExt.GetBuffer ()));

								            pCST->ClearDTInfoTemplate (iItem);
								            pCST->SetState (iItem);
            		                        
								            dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->SetListBoxItemFilesText (pCST, iItem, false);

								            lvItem.state = 0;
								            ((CListCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_LIST_ITEM_FILES))->SetItemState (iItem, &lvItem);
								            break;
							            case FILE_ACTION_RENAMED_NEW_NAME: //Новое имя после переименования (5)
								            filefind.Close ();
								            if (filefind.FindFile (pwchFullPath)) {
									            if (CFile::GetStatus (pwchFullPath, fStatus)) {
										            //iNumExtDiff = _wtoi ((strExt.GetBuffer ())) - iNumExtPrev;
										            pCST->SetDTInfoTemplate (iItem, fStatus.m_mtime);
										            pCST->SetState (iItem);
            				                        
										            dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->SetListBoxItemFilesText (pCST, iItem, false);

										            lvItem.state = LVIS_SELECTED;
										            ((CListCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_LIST_ITEM_FILES))->SetItemState (iItem, &lvItem);
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
    //MEMORY_LEAKED (_T ("pFNI->Action"))
#endif

						            //Время, когда произошло событие
						            GetLocalTime (&sTime);
					            }
					            else
						            ; //Не НАЙДЕН в списке 'm_ListItemFiles'
				        }
				        else {
							//ШАБЛОН в котором произошли изменения НЕ отображается
					        //Тупо (принудительно) обновляем инфрмацию о ШАБЛОНе
					        pCST->ClearTimeUpdate ();
					        pCST->Update ();
					    
#if defined _DEBUG
    //MEMORY_LEAKED (_T ("pCST->Update () - container"))
#endif

					        //Структура для посылки сообщения
					        hdr.code = TVN_SELCHANGED;
					        hdr.hwndFrom = ((CTreeCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->m_hWnd;
					        hdr.idFrom = ((CTreeCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetDlgCtrlID ();

					        /*
					        //Так коротко, но НЕ ПОНЯТНО
					        if ((((CTreeCtrl *) m_pDlgGUI->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetSelectedItem () == ((CTreeCtrl *) m_pDlgGUI->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetParentItem (pCST->GetHTreeItem ()) ||
						        (((CTreeCtrl *) m_pDlgGUI->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetSelectedItem () == ((CTreeCtrl *) m_pDlgGUI->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetRootItem ()))
							        this->SendMessage(WM_NOTIFY, (WPARAM) ((CTreeCtrl *) m_pDlgGUI->GetDlgItem (IDC_TREECTRL_TEMPLATE))->m_hWnd, (LPARAM) &hdr);
					        */

					        //Так ДЛИННЕЙ, но ПОНЯТНО
					        if (((CTreeCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetSelectedItem () == ((CTreeCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetParentItem (pCST->GetHTreeItem ())) {
								//КОНТЕЙНЕР для ШАБЛОНа в котором произошли изменения ОТОБРАЖАЕТСЯ
						        //Отсылаем сообщение на ПЕРЕСЕЛЕКТ КОНТЕЙНЕРа для ШАБЛОНа
						        dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->SendMessage(WM_NOTIFY, (WPARAM) ((CTreeCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->m_hWnd, (LPARAM) &hdr);
					        }
					        else {
								//КОНТЕЙНЕР для ШАБЛОНа в котором произошли изменения НЕ отображается
						        if (((CTreeCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetSelectedItem () == ((CTreeCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->GetRootItem ()) {
									//ОТОБРАЖАЕТСЯ КОРНЕВАой элемент дерева
							        //Отсылаем сообщение на ПЕРЕСЕЛЕКТ КОРНЕВого элемента дерева
							        dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->SendMessage(WM_NOTIFY, (WPARAM) ((CTreeCtrl *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->m_hWnd, (LPARAM) &hdr);
						        }
						        else {
									//Вообще НЕТ НЕОБХОДИМОСТИ что-то ДЕЛАТЬ
						        }
					        }
					    
#if defined _DEBUG
    //MEMORY_LEAKED (_T ("SendMessage"))
#endif
				        }
			        }
					else {
						strLog.format ("'::OnCommandThreadDirWatch...pCST НЕТ для: %S", pwchFullPath);
						FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
						strLog.free ();
					}
		        }
		        else {
					//ИзменниЯ НЕ для этоГО приложения
					strLog.format ("'::OnCommandThreadDirWatch...расширение: %S", pwchExt);
					FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
					strLog.free ();
				}
    	
    	        FREE_PTR (pwchNameTemplateOrigin)
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
            }
            //КЛАССИКА
	        //while (bWorking);
	        //НЕ КЛАССИКА
	        while (bytesDiff);

			/*strLog.format ("'::OnCommandThreadDirWatch...обработано байт: %i", bytes - bytesDiff);
            FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
            strLog.free ();*/
    	    
	        //FREE_PTR (pwchDirWatched)
	        pwchDirWatched.free ();
        }
        else
            ; //НЕТ информации о ПОТОКе ???

	    pFNI = NULL;

	    strLog.free ();
    }
    else
        ; //Число полученных данных НЕ КОРРЕКТНО

#if defined _DEBUG
	//MEMORY_LEAKED (_T ("Выход"))    
#endif

	setLastResponce ();

	SetEvent (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::DIRECTORY_WATCHED));
}

afx_msg void HThreadGUIFilesDB::OnMessageTimer (WPARAM , LPARAM ) {
    //m_pDlgGUI->TimeIndicators ();

    FILELOG_VARIABLE
    CTime tmEntry = CTime::GetCurrentTime (), tmExit;

    strLog.format (_T ("%S"), _T ("Обработка сообщения 'WM_TIMER'..."));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), PSEUDO_KTS_DLG);

    if (IS_RUN_REMAINING) {
		//Приложение в режиме ограничения времени рабоТы
        if ((tmEntry - ((HWinApp *) AfxGetApp ())->GetTimeStart ()).GetTotalSeconds () > WORK_SECONDS) {
            //Закрываем приложение по времени рабоТы
			dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDCANCEL)->SendMessage (BM_CLICK);

            return;
        }
        else
            ;
    }
    else
        ;

    switch (WaitForSingleObject (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::DIRECTORY_WATCHED), 66)) {
		case WAIT_ABANDONED:
		case WAIT_TIMEOUT:
			break;
		case WAIT_OBJECT_0:
			FILELOG_WRITE_WITHDT ("Запрет работы ВСЕМ ПОТОКам...", PSEUDO_KTS_DLG);
			ResetEvent (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::DIRECTORY_WATCHED));
			
			((HTreeCtrlKTS *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_TEMPLATE))->UpdateObjectCheckStateTemplate ();

	        dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->TimeIndicators (HDlgKTSFiles::ID_INDICATOR_TIME_PASSED, HDlgKTSFiles::ID_INDICATOR_TIME_CURRENT);

	        FILELOG_WRITE_WITHDT ("Разрешение работы ВСЕМ ПОТОКам...", PSEUDO_KTS_DLG);
	        SetEvent (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::DIRECTORY_WATCHED));
		default:
			;
	}

	tmExit = CTime::GetCurrentTime ();

	if ((tmExit - tmEntry).GetTotalSeconds () > 1) {
        strLog.format (_T ("Время выполнения 'CUserKTSDlg::OnTimer' составило %i секунд(ы)!"), (tmExit - tmEntry).GetTotalSeconds ());
        FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), PSEUDO_KTS_DLG);
        //MessageBox (strLog, _T("Внимание!"), MB_ICONINFORMATION);
    }
    else
        ;

	setLastResponce ();
}

afx_msg void HThreadGUIFilesDB::OnMessageInitComboboxDirWatch (WPARAM , LPARAM ) {
    dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->InitComboBoxDirWatch ();
}

//Сохраняем 'INT_SOURCE_ID', 'INT_NUMBER_SELECTED_SOURCE' для использования другими потоками
//afx_msg void HThreadGUIFilesDB::OnConnectToServer (WPARAM wp, LPARAM lp) {
//    int idSourceData = wp, iNumberSelectedSource = lp;
//
//    AddData (INT_SOURCE_ID, &idSourceData, sizeof (int));
//    AddData (INT_NUMBER_SELECTED_SOURCE, &iNumberSelectedSource, sizeof (int));
//}

//Прием запроса о необходимости оповестить адресата о параметрах соединения
afx_msg void HThreadGUIFilesDB::OnMessageQueryDataConnectToServer (WPARAM wp, LPARAM ) {
    FILELOG_VARIABLE

    DWORD idThread = (DWORD) wp;
    CComboBox *pComboBox = ((CComboBox *) dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->GetDlgItem (IDC_COMBO_SOURCEDB));
    if ((idThread > 0) && (! (pComboBox == 0x0))) {
        ::PostThreadMessage (idThread, WM_USER_CONNECT_TO_SERVER, dynamic_cast <HDlgGUIFilesDB *> (m_pMainWnd)->m_iIdSource, pComboBox->GetCurSel ());

        strLog.format (_T ("HThreadGUIFilesDB::OnMessageQueryDataConnectToServer () - получено сообщение от %i"), idThread);
    }
    else
        strLog.format (_T ("HThreadGUIFilesDB::OnMessageQueryDataConnectToServer () - получено сообщение от потока с аргументом (id потока)="), -1);

    
    FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);
}
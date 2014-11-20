#include "stdafx.h"

#include "pseudokts.h"

#include "hthreadguicontrol.h"
#include "HThreadGetData.h"

#include <string>

EXT_DECLARE_HTHREAD_FILELOG

extern HString tokenizeTypeSource (int, HString &);
extern HString tokenizeConnectDB (int, HString &);

HThreadGUIControl::HThreadGUIControl () : HWinThread () {
}

HThreadGUIControl::~HThreadGUIControl () {
    //За этим следит САМ поток 'm_pMainWnd' ???
    if (m_pMainWnd) {
        dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->DestroyWindow ();

        delete dynamic_cast <HDlgGUIControl *> (m_pMainWnd);
    }
    else
        ;
}

BEGIN_MESSAGE_MAP (HThreadGUIControl, HWinThread)
    ON_THREAD_MESSAGE (WM_USER_THREAD_RESUME, &OnResumeRun)
    ON_THREAD_MESSAGE (WM_USER_CONNECT_TO_SERVER, &OnConnectToServer)
    ON_THREAD_MESSAGE (WM_USER_TIMER, &OnMessageTimer)
    ON_THREAD_MESSAGE (WM_USER_SEC_15_THREADGETDATA, &OnSec15ThreadGetData)
    ON_THREAD_MESSAGE (WM_USER_ENG_ORIGIN_THREADGETDATA, &OnEngOriginThreadGetData)
    ON_THREAD_MESSAGE (WM_USER_ENG_PSEUDO_THREADGETDATA, &OnEngPseudoThreadGetData)
    ON_THREAD_MESSAGE (WM_USER_QUERY_NUMBER_PTIC_CONTROL, &OnQueryUserNumberPTICControl)
END_MESSAGE_MAP ()

int HThreadGUIControl::InitInstance (void) {
	int iRes = -1;
	//ПЕРВый вызов метода БАЗового класса
	iRes = HWinThread::InitInstance ();

    FILELOG_VARIABLE

    strLog.format ("HThreadGUIControl::InitInstance...");
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG/*COMMON_KTS_APP*/)
    strLog.free ();

    m_idExitCodeThread = STILL_ACTIVE;

    if (iRes == 1) {
	}
	else ; //Инициализация - ОШИБКА

	return iRes;
}

int HThreadGUIControl::ExitInstance (void) {
    if (dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->IsModal ())
        dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->EndDialog (m_idExitCodeThread);
    else
        dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->SendMessage (WM_DESTROY, m_idExitCodeThread);

	//КРАЙний вызов метода БАЗового класса
	return HWinThread::ExitInstance ();
}

afx_msg void HThreadGUIControl::OnResumeRun (WPARAM, LPARAM ) {
    FILELOG_VARIABLE

    strLog.format ("HThreadGUIControl::OnResumeRun...");
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG/*COMMON_KTS_APP*/)
    strLog.free ();

    HWinThread *ptrThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GUI_FILESDB);
    ::PostThreadMessage (ptrThread->m_nThreadID, WM_USER_QUERY_CONNECT_TO_SERVER, m_nThreadID, 0);
    
    CRuntimeClass* pDlgClass = RUNTIME_CLASS (HDlgGUIControl);
    m_pMainWnd = (CWnd *) pDlgClass->CreateObject ();
	ASSERT (m_pMainWnd);
	if (m_pMainWnd->IsKindOf(pDlgClass))
		dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->PreCreate (CRect (0, 0, 360, 180));
	else
		;

    if (dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->IsModal ()) {
        /*INT_PTR nResponse = -1;
        nResponse = */dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->DoModal ();

        /*if (nResponse == IDOK) {
		    // TODO: Place code here to handle when the dialog is dismissed with OK
	    }
	    else if (nResponse == IDCANCEL)	{
		    // TODO: Place code here to handle when the dialog is dismissed with Cancel
	    }*/
    }
    else {
        dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->ShowWindow (SW_HIDE);
    }

    //m_pMainWnd = 0x0/*m_pDlgGUI*/; //???

    strLog.format ("HThreadGUIControl::OnResumeRun... - Выход");
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG/*COMMON_KTS_APP*/)
    strLog.free ();
}

afx_msg void HThreadGUIControl::OnMessageTimer (WPARAM , LPARAM ) {
    CTreeCtrl *pTreeCtrl = 0x0;
    if (m_pMainWnd) {
        pTreeCtrl = dynamic_cast <CTreeCtrl *> (dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->GetDlgItem (IDC_TREECTRL_CONTROL));
        pTreeCtrl->GetCount ();
        dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->TimeIndicators (HDlgGUIControl::ID_INDICATOR_TIME_PASSED, HDlgGUIControl::ID_INDICATOR_TIME_CURRENT);
    }
    else
        ;
}

void setItemText (CListCtrl *ptrListCtrl, int iItem, int iSubItem, DWORD val) {
    HString strItem;
    string stdstrItem = to_string ((_ULonglong) val);
    strItem = HString (stdstrItem.c_str ());
    BOOL bRes = ptrListCtrl->SetItemText (iItem, iSubItem, WC_HSTRING (strItem));
}

void setItemText (CListCtrl *ptrListCtrl, int iItem, int iSubItem, double val) {
    HString strItem;
    string stdstrItem = to_string ((long double) val);
    strItem = HString (stdstrItem.c_str ());
    BOOL bRes = ptrListCtrl->SetItemText (iItem, iSubItem, WC_HSTRING (strItem));
}

afx_msg void HThreadGUIControl::OnSec15ThreadGetData (WPARAM wp, LPARAM ) {
    FILELOG_VARIABLE

    HThreadGetData::DATA_RESPONSE dataResponse;
    memcpy (&dataResponse, (HThreadGetData::DATA_RESPONSE *) wp, sizeof (HThreadGetData::DATA_RESPONSE));
    CTime tmQuery;

    strLog.format (_T ("Получено WM_USER_SEC_15_THREADGETDATA от 'HThreadGetData' за время: %i"), dataResponse.tm64tQuery);
    FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);

    if (dataResponse.tm64tQuery > 0) {
        tmQuery = CTime (dataResponse.tm64tQuery + PTR_DATETIMEINTERVAL->tm64tBias ());
    
        dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->m_ptrStatusBar->SetPaneText (HDlgGUIControl::ID_INDICATOR_TIME_QUERY, tmQuery.Format (_T ("Опрос: %d.%m.%Y %H:%M:%S")));

        INDEX_TIME t_max = DAY_01;
        CListCtrl *ptrListCtrl = 0x0;
        //IDC_LIST_ITEM_IMPULSES
        ptrListCtrl = dynamic_cast <CListCtrl *> (dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->GetDlgItem (IDC_LIST_ITEM_IMPULSES));
        for (INDEX_TIME t = SECOND_15; t < t_max; t ++) {
            setItemText (ptrListCtrl, t, 1, (DWORD) dataResponse.arWordImpulses [t] [HThreadGetData::PREVIOUS]);
            setItemText (ptrListCtrl, t, 2, (DWORD) dataResponse.arWordImpulses [t] [HThreadGetData::CURRENT]);
        }

        //IDC_LIST_ITEM_VALUES
        ptrListCtrl = dynamic_cast <CListCtrl *> (dynamic_cast <HDlgGUIControl *> (m_pMainWnd)->GetDlgItem (IDC_LIST_ITEM_VALUES));
        for (INDEX_TIME t = SECOND_15; t < t_max; t ++) {
            setItemText (ptrListCtrl, t, 1, (double) dataResponse.arDblValues [t] [HThreadGetData::PREVIOUS]);
            setItemText (ptrListCtrl, t, 2, (double) dataResponse.arDblValues [t] [HThreadGetData::CURRENT]);
        }

        setLastResponce ();
    }
    else
        ;
}

afx_msg void HThreadGUIControl::OnQueryUserNumberPTICControl (WPARAM wp, LPARAM ) {
    HTREEITEM hItem = 0x0;
    hItem = (((CTreeCtrl *) m_pMainWnd->GetDlgItem (IDC_TREECTRL_CONTROL)))->GetSelectedItem ();
    if (hItem) ::PostThreadMessage (wp, WM_USER_NUMBER_PTIC_CONTROL, (int) (((CTreeCtrl *) m_pMainWnd->GetDlgItem (IDC_TREECTRL_CONTROL)))->GetItemData (hItem), 0); else ;
}

afx_msg void HThreadGUIControl::OnConnectToServer (WPARAM wp, LPARAM lp) {
    FILELOG_VARIABLE

    strLog.format (_T ("Получено сообщение от 'HthreadFilesDB', параметры: {wp=%i, lp=%i}"), wp, lp);
    FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);

    HDatabaseSQL *ptrDBSQL = DEBUG_NEW HDatabaseSQL ();
    HString strConnectDB, strarServerNames;
    int iRes = -1, iError = 0;

    int idSourceData = -1, iNumberSelectedSource = -1;
    //Вариант №1
    /*HWinThread *ptrThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GUI_FILESDB);
    idSourceData = *((int *) ptrThread->GetDataOfId (HThreadGUIFilesDB::INT_SOURCE_ID));
    iNumberSelectedSource = *((int *) ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GUI_FILESDB)->GetDataOfId (HThreadGUIFilesDB::INT_NUMBER_SELECTED_SOURCE));*/
    //Вариант №2
    idSourceData = wp, iNumberSelectedSource = lp;

    //strConnectDB = stringData (GetDataOfId (STR_CONNECT_DB));
    strarServerNames = PTR_SETTINGS->CompileSourceField (idSourceData, HString (_T ("SERVER_NAME")), HDBSettings::SQL_DB);

    int iPos = 0, iNumSource = 0;
    if (strarServerNames.isUses ()) {
		strConnectDB = PTR_SETTINGS->CompileConnectDB (idSourceData, HDBSettings::SQL_DB);

        strLog.format (_T ("CompileConnectDB вернула: %S"), WC_HSTRING (strConnectDB));
        FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);

		iRes = ptrDBSQL->Connect (MB_HSTRING (tokenizeConnectDB (iNumberSelectedSource, strConnectDB)));
		if (ptrDBSQL->IsConnectDB ()) {
            iRes = 0; //Success

            CTreeCtrl *pTreeCtrl = dynamic_cast <CTreeCtrl *> (m_pMainWnd->GetDlgItem (IDC_TREECTRL_CONTROL));
            HTREEITEM hItemUSD = 0x0, hItemChnl = 0x0, hItemChnlSelect = 0x0;

            TVINSERTSTRUCT tvInsert;
            tvInsert.hInsertAfter = TVI_LAST;
            tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;

            CRecordset *pRSUSD = ptrDBSQL->GetRecordset (), *pRSChnl = ptrDBSQL->GetRecordset ();
            std::wstring strQueryUSD, strQueryChnl, strNameField;
            CDBVariant dbData;

//CHANNEL
//'SELECT pl.PTICUserNumber, pl.idPTIC, pl.idTK, pl.PTICName, pl.PTICShrName ' +
//'FROM e6pr.PR_TIChnlList ticl, e6pr.PR_PTICList pl, e6pr.PR_USDList ul ' +
//'WHERE ticl.idTIChnl=pl.idTIChnl and ul.idUSD=ticl.idUSD and ' +
//'ul.idUSDType='

//Электричество str = str + '137'
//Тепло str = str + '275'

//Электричество strNumberUSD = GetElektroNumberUSDOfPlace (FormSostChnl.SelectPlace.value)
//Тепло strNumberUSD = GetTeploNumberUSDOfPlace (FormSostChnl.SelectPlace.value)

//str += ' and ul.USDUserNumber IN (' + strNumberUSD + ')'
//str += ' ORDER BY pl.PTICUserNumber'

//USD
//'SELECT USDNameShr, idUSD, USDUserNumber ' +
//'FROM e6pr.PR_USDList ' +
//'WHERE idUSDType='
      
//Электричество
//strNumberUSD = GetElektroNumberUSDOfPlace (numPlace)
//str = str + '137'

//Тепло
//strNumberUSD = GetTeploNumberUSDOfPlace (numPlace)
//str = str + '275'

//str += ' AND USDUserNumber IN (' + strNumberUSD + ')'

            //for (int i = 0; i < 32; i ++) {
                strQueryUSD = L"SELECT USDNameShr, idUSD, USDUserNumber \
                                FROM e6pr.PR_USDList \
                                WHERE idUSDType = ";
                strQueryUSD += std::to_wstring ((_ULonglong) 275);
                //strQueryUSD += L" and ul.USDUserNumber = ";
                //strQueryUSD += std::to_wstring ((_ULonglong) (i + 1));

                try { pRSUSD->Open (CRecordset::dynaset, strQueryUSD.c_str (), CRecordset::useBookmarks | CRecordset::noDirtyFieldCheck); }
                catch (CDBException *err) {
                    FILELOG_IF_WRITE (HERROR)
                    strLog.format ("Не выполнен запрос: %s.\n%s", MB_HSTRING (HString (strQueryChnl.c_str ())), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
                    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
                    FILELOG_ENDIF

                    iRes = -1;

		            err->Delete ();
                }

                if (iRes == 0) {
                    if (! pRSUSD->IsEOF ()) {
                        tvInsert.hParent = 0x0;

                        tvInsert.item.lParam = 0x0;
                        tvInsert.item.pszText = L"УСД по симплексу (Тепло)";

                        pTreeCtrl->InsertItem (&tvInsert);

                        while (! pRSUSD->IsEOF ()) {
                            tvInsert.hParent = pTreeCtrl->GetRootItem ();

                            strNameField = L"USDUserNumber";
                            try { pRSUSD->GetFieldValue (strNameField.c_str (), dbData); }
                            catch (CException *err)
                            { iRes = -1; err->Delete (); break; }
                            tvInsert.item.lParam = (DWORD_PTR) dbData.m_iVal;

                            strNameField = L"USDNameShr";
                            try { pRSUSD->GetFieldValue (strNameField.c_str (), dbData); }
                            catch (CException *err)
                            { iRes = -1; err->Delete (); break; }
                            HString strShrName;
                            strShrName.format (L"%s (УСД. №%i)", dbData.m_pstringA->GetBuffer (), tvInsert.item.lParam);
                            tvInsert.item.pszText = WC_HSTRING (strShrName);

                            hItemUSD = pTreeCtrl->InsertItem (&tvInsert);

                            strQueryChnl = /*L"SELECT pl.PTICUserNumber, pl.idPTIC, pl.idTK, pl.PTICName, pl.PTICShrName \*/L"SELECT pl.PTICUserNumber, pl.PTICShrName \
                                        FROM e6pr.PR_TIChnlList ticl, e6pr.PR_PTICList pl, e6pr.PR_USDList ul \
                                        WHERE ticl.idTIChnl=pl.idTIChnl and ul.idUSD=ticl.idUSD \
                                        and ul.idUSDType = ";
                            strQueryChnl += std::to_wstring ((_ULonglong) 275);
                            strQueryChnl += L" and ul.USDUserNumber = ";
                            strQueryChnl += std::to_wstring ((_ULonglong) (tvInsert.item.lParam/* + 1*/));

                            try { pRSChnl->Open (CRecordset::dynaset, strQueryChnl.c_str (), CRecordset::useBookmarks | CRecordset::noDirtyFieldCheck); }
                            catch (CDBException *err) {
                                FILELOG_IF_WRITE (HERROR)
                                strLog.format ("Не выполнен запрос: %s.\n%s", MB_HSTRING (HString (strQueryChnl.c_str ())), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
                                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
                                FILELOG_ENDIF

                                iRes = -1;

		                        err->Delete ();

                                break;
                            }

                            if (iRes == 0) {
                                if (! pRSChnl->IsEOF ()) {
                                    tvInsert.hParent = hItemUSD;
                                    while (! pRSChnl->IsEOF ()) {
                                        strNameField = L"PTICUserNumber";
                                        try { pRSChnl->GetFieldValue (strNameField.c_str (), dbData); }
                                        catch (CException *err)
                                        { iRes = -1; err->Delete (); break; }
                                        tvInsert.item.lParam = (DWORD_PTR) dbData.m_iVal;

                                        strNameField = L"PTICShrName";
                                        pRSChnl->GetFieldValue (strNameField.c_str (), dbData);
                                        HString strShrName;
                                        strShrName.format (L"%s (кан. №%i)", dbData.m_pstringA->GetBuffer (), tvInsert.item.lParam);
                                        tvInsert.item.pszText = WC_HSTRING (strShrName);

                                        hItemChnl = pTreeCtrl->InsertItem (&tvInsert);
                                        if (tvInsert.item.lParam == 9) hItemChnlSelect = hItemChnl; else ;

                                        try { pRSChnl->MoveNext (); }
                                        catch (CException *err)
                                        { iRes = -1; err->Delete (); break; }
                                    }
                                }
                                else
                                    ;
                            }
                            else ;

                            pRSChnl->Close ();

                            pRSUSD->MoveNext ();
                        } //'while' по USD

                        pTreeCtrl->SelectItem (hItemChnlSelect);
                    }
                    else
                        ; //RS 'USD' НЕТ ни однОЙ записИ
                }
                else
                    ; //Не выполнен запрос для 'USD'
            //} //Цикл по 32 USD

            FreeMemoryDBObject ((CObject *&) pRSChnl);
            FreeMemoryDBObject ((CObject *&) pRSUSD);
		}
		else {
			//Не удалось создать соединение с SQLServer-ом
			strLog.format ("Не удалось создать соединение с SQLServer-ом: %s", MB_HSTRING (strConnectDB));

			FreeMemoryDBObject ((CObject *&) ptrDBSQL);
		}
	}
    else
		//Нет НИ ОДНого имени СДД для подключения
        strLog.format (_T ("%S"), _T ("Нет НИ ОДНого имени СЕРВЕРА для подключения"));

	if ((ptrDBSQL == 0x0) || (! (iRes == 0))) {
		strLog.insert (0, _T ("HDlgGUIControl::OnConnectToServer - "));
		FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);

		//Сообщить контролирующему потоку о завершении
        //::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_USER_THREAD_GUI_CLOSE, iRes, 0);
		//Завершить поток
        //::PostThreadMessage (idCurrentThread, WM_QUIT, iRes, 0);
	}
	else
		;

    FreeMemoryDBObject ((CObject *&) ptrDBSQL);
}

void HThreadGUIControl::setItemTextOfGetData (int item, int iSubItem, WPARAM imp, LPARAM val) {
     CListCtrl *ptrListCtrl = 0x0;
    //IDC_LIST_ITEM_IMPULSES
    ptrListCtrl = dynamic_cast <CListCtrl *> (m_pMainWnd->GetDlgItem (IDC_LIST_ITEM_IMPULSES));
    setItemText (ptrListCtrl, item, iSubItem, (DWORD) *(WORD *) imp);

    //IDC_LIST_ITEM_VALUES
    ptrListCtrl = dynamic_cast <CListCtrl *> (m_pMainWnd->GetDlgItem (IDC_LIST_ITEM_VALUES));
    setItemText (ptrListCtrl, item, iSubItem, (double) *(double *) val);

    setLastResponce ();
}

afx_msg void HThreadGUIControl::OnEngPseudoThreadGetData (WPARAM wp, LPARAM lp) {
    FILELOG_VARIABLE

    strLog.format ("HThreadGUIControl::OnEngPseudoThreadGetData ()...");
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);

    setItemTextOfGetData (wp, 3, (WPARAM) (*((WORD **) lp + 0)), (LPARAM) (*((double **) lp + 1)));

    delete *((WORD **) lp + 0);
    delete *((double **) lp + 1);

    delete (void *) lp;
}

afx_msg void HThreadGUIControl::OnEngOriginThreadGetData (WPARAM wp, LPARAM lp) {
    FILELOG_VARIABLE

    strLog.format ("HThreadGUIControl::OnEngOriginThreadGetData ()...");
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);

    switch (wp) {
        case MINUTE_05:
            break;
        case MINUTE_30:
            break;
        default: ;
    }
   setItemTextOfGetData (wp, 4, (WPARAM) (*((WORD **) lp + 0)), (LPARAM) (*((double **) lp + 1)));

   delete *((WORD **) lp + 0);
   delete *((double **) lp + 1);

   delete (void *) lp;
}
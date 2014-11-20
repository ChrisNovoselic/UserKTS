#include "stdafx.h"
#include "HDlgGUIFilesDB.h"

#include "PseudoKTS.h" //PseudoKTS::GetPtrThread

#include <HThreadDirectoryWatched.h> //HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED
#include "HThreadGetData.h" //HThreadGetData::INT_SOURCE_ID
#include "HThreadWriteData.h"

IMPLEMENT_DYNCREATE (HDlgGUIFilesDB, HDlgKTSFiles)

EXT_DECLARE_HTHREAD_FILELOG

HDlgGUIFilesDB::HDlgGUIFilesDB (CWnd* pParent) : HDlgKTSFiles (pParent),
                                                m_pTreeCtrlTemplate (0x0),
                                                m_iIdSource (-1) {
}

HDlgGUIFilesDB::~HDlgGUIFilesDB (void) {
}

void HDlgGUIFilesDB::DoDataExchange (CDataExchange* pDX) {
    HDlgKTSFiles::DoDataExchange (pDX);
}

BEGIN_MESSAGE_MAP(HDlgGUIFilesDB, HDlgKTSFiles)
    ON_MESSAGE (WM_DESTROY, &OnDestroy)
    //ON_COMMAND (IDM_RESTORE_FILES_DB, &HDlgKTSFiles::OnCommandRestoreDlg)
    ON_BN_CLICKED (IDCANCEL, &OnBnClickedCancel)
    ON_COMMAND (IDM_EXIT, &OnBnClickedCancel)
    //ON_NOTIFY (TVN_SELCHANGED, IDC_TREECTRL_TEMPLATE, &OnTvnSelchangedTemplate)
    ON_CBN_SELCHANGE (IDC_COMBO_SOURCEDB, &OnSourceDBSelectedChanged)
END_MESSAGE_MAP()

// HDlgChoiceDirectory message handlers

BOOL HDlgGUIFilesDB::OnInitDialog () {
    FILELOG_VARIABLE

    int iRes = -1;
    iRes = HDlgKTSFiles::OnInitDialog ();

    SetWindowText (_T ("Эмулятор ядра БПО Энергия+ ver.5.x...: Файлы БД"));

    CButton *pButton = 0x0;

    CRect rcThis, rcCtrl,
            rcStatusBar;
    GetClientRect (&rcThis);

    CreateStatusBar (this);
    m_ptrStatusBar->GetClientRect (&rcStatusBar);
    
    CRect rcTreeCtrl;
    m_pTreeCtrlTemplate = DEBUG_NEW HTreeCtrlPseudo ();
    if (m_pTreeCtrlTemplate->InitializeRS (_T (TABLENAME_TEMPLATE)) == 0) {
        rcTreeCtrl.left = OFFSET_CONTROL; rcTreeCtrl.right = 0.35 * rcThis.Width ();
        rcTreeCtrl.top = OFFSET_CONTROL; rcTreeCtrl.bottom = rcThis.bottom - rcStatusBar.Height () - OFFSET_CONTROL;
	    m_pTreeCtrlTemplate->Create (WS_VISIBLE | WS_BORDER | WS_OVERLAPPED | WS_CHILD | WS_HSCROLL  | WS_VSCROLL | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS /*| TVS_CHECKBOXES | TVS_SINGLEEXPAND*/, rcTreeCtrl, this, IDC_TREECTRL_TEMPLATE);

        if (m_pTreeCtrlTemplate->InitializeImageList () == 0)
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
    
    if (iRes == 0) {
        iRes = CreateCommonCtrl (this);

        CComboBox *ptrComboBoxPathDirWatch = 0x0
            , *ptrComboBoxDataSrc = 0x0;
        //Создание 'ComboBox' директория назначения
        ptrComboBoxPathDirWatch = DEBUG_NEW CComboBox ();
        rcCtrl.left = rcTreeCtrl.right + OFFSET_CONTROL; rcCtrl.right = rcCtrl.left + ((rcThis.Width () - rcTreeCtrl.Width () - 4 * OFFSET_CONTROL) / 2);
        rcCtrl.top = rcTreeCtrl.top; rcCtrl.bottom = rcCtrl.top + HEIGHT_CONTROL;
        ptrComboBoxPathDirWatch->Create (WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP, rcCtrl, this, IDC_COMBO_DIRWATCH);
        SendDlgItemMessage (IDC_COMBO_DIRWATCH, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);
        DDX_Control(m_ptrDataExchange, IDC_COMBO_DIRWATCH, *ptrComboBoxPathDirWatch);

        //Создание 'ComboBox' источник данных
        ptrComboBoxDataSrc = DEBUG_NEW CComboBox ();
        rcCtrl.left = rcCtrl.right + OFFSET_CONTROL; rcCtrl.right = rcThis.Width () - OFFSET_CONTROL;
        rcCtrl.top = rcTreeCtrl.top; rcCtrl.bottom = rcCtrl.top + HEIGHT_CONTROL;
        ptrComboBoxDataSrc->Create (WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP, rcCtrl, this, IDC_COMBO_SOURCEDB);
        SendDlgItemMessage (IDC_COMBO_SOURCEDB, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);
        DDX_Control(m_ptrDataExchange, IDC_COMBO_SOURCEDB, *ptrComboBoxDataSrc);

        iRes = InitComboBoxDirWatch ();

        //ptrThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GETDATA);
        HString strConnectDB, strarServerNames, strServerName;
        //strConnectDB = ptrThread->stringDataOfId (HThreadGetData::STR_CONNECT_DB);

        m_iIdSource = PTR_SETTINGS->GetOption (_T ("ИД_ИСТОЧНИКДАННЫХ_ТЕПЛО")).number ();
        //strarServerNames = PTR_SETTINGS->CompileSourceField (*(int *) ptrThread->GetDataOfId (HThreadGetData::INT_SOURCE_ID), HString (_T ("SERVER_NAME")), HDBSettings::SQL_DB);
        strarServerNames = PTR_SETTINGS->CompileSourceField (m_iIdSource, HString (_T ("SERVER_NAME")), HDBSettings::SQL_DB);

        int iPos = 0;
        if (strarServerNames.isUses ())
            while (iPos > -1) {
                strServerName = strarServerNames.tokenize (_T (","), iPos);
                if (strServerName.isUses ())
                    ptrComboBoxDataSrc->AddString (WC_HSTRING (strServerName));
                else
                    ;
            }
        else
            ;

        ptrComboBoxDataSrc->SetCurSel (0);

        iRes = InitializeOperation ();

        m_pTreeCtrlTemplate->SelectItem (m_pTreeCtrlTemplate->GetRootItem ());
        m_pTreeCtrlTemplate->Expand (m_pTreeCtrlTemplate->GetRootItem (), TVE_EXPAND);

        if (MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_LOCAL)
            PostMessage (WM_USER_TOSYSTEMTRAY);
        else
            ;
    }
    else
        ;

    if (iRes == 0)
        return TRUE;
    else
        return FALSE;
}

int HDlgGUIFilesDB::InitComboBoxDirWatch () {
    int iRes = 0;
    CComboBox *ptrComboBoxPathDirWatch = dynamic_cast <CComboBox *> (GetDlgItem (IDC_COMBO_DIRWATCH));

    //ptrComboBox->AddString (_T ("<Изменить...>"));
    HWinThread *ptrThread = 0x0; int suspense = 0;
    DWORD dwWaitRes = WaitForSingleObject (((CPseudoKTSApp *) AfxGetApp ())->m_hEnterDirectoryWatched, 666L);
    /*while ((ptrThread == 0x0) && (suspense < 666)) {
        ptrThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_DIRECTORYWATCHED);
        suspense += 66;
        Sleep (66);
    }*/
    if (dwWaitRes == 0) {
        ptrThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_DIRECTORYWATCHED);

        if (!(ptrThread == 0x0)) {
            ptrComboBoxPathDirWatch->AddString (WC_HSTRING (ptrThread->stringDataOfId (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED)));
            ptrComboBoxPathDirWatch->SetCurSel (ptrComboBoxPathDirWatch->GetCount () - 1);
        }
        else
            iRes = -1;
    }
    else
        iRes = -1;

    return iRes;
}

afx_msg LRESULT HDlgGUIFilesDB::OnDestroy (WPARAM wp, LPARAM lp) {
    //GetDlgItem (IDC_COMBO_DIRWATCH)->DestroyWindow ();
    delete GetDlgItem (IDC_COMBO_DIRWATCH);
    //GetDlgItem (IDC_COMBO_SOURCEDB)->DestroyWindow ();
    delete GetDlgItem (IDC_COMBO_SOURCEDB);

    HDlgKTSFiles::OnDestroy ();

    return wp;
}

afx_msg void HDlgGUIFilesDB::OnBnClickedCancel () {
    int iCodeExit = 0x0;
    
    CWinThread *ptrThreadTheApp = AfxGetApp ();
    DWORD idThreadTheApp = ptrThreadTheApp->m_nThreadID;
	//ptrThreadTheApp->PostThreadMessage (WM_USER_THREAD_GUI_CLOSE, nResponse, 0);
    //ptrThreadTheApp->PostThreadMessage (WM_QUIT, nResponse, 0);
    ::PostThreadMessage (idThreadTheApp, WM_USER_THREAD_GUI_CLOSE, iCodeExit, 0);
}

void HDlgGUIFilesDB::OperationCreate (HCheckStateTemplate *pCST, short shNumBeginSource) {
	HThreadWriteData *ptrThreadWriteData = 0x0;
	//ptrThreadWriteData = (HThreadWriteData *) ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_WRITEDATA);
	ptrThreadWriteData = (dynamic_cast <HThreadWriteData *> ((dynamic_cast <CPseudoKTSApp *> (AfxGetApp ()))->GetPtrThread (CPseudoKTSApp::THREAD_WRITEDATA)));

	if (m_arPosSelectedItem.GetSize () == 0) {
		//Значит - операция инициирована АВТОматически

		int *pPseudoPosSelectedItem = 0x0, i, iCountItem = -1;

		//Это, возможно, ЛИШНяя проверка
		if (pCST->GetMaxExt () == 0) {
			//Этот строка выполняется ТОЛЬКо, если вызов произошел из 'OperationCreate'
			m_arPosSelectedItem.Add (POSITION (1));

			if (((ITEM_DATA *) TREECTRL_KTS->GetItemData (pCST->GetHTreeItem ()))->is_pseudo) {
				OperationModifyDT (pCST);
				SetEvent (pCST->m_hEvtOperation);
				//ptrThreadWriteData->PostThreadMessage (ID_OPERATION_CHANGE_DATETIME, (WPARAM) pCST, (LPARAM) pPseudoPosSelectedItem);
			}
			else {
			}
		}
		else {
			pPseudoPosSelectedItem = pCST->FillArrayPseudoPosSelectedItem (iCountItem);

			HString strWildcard = pCST->GetFullPathTemplate (), strNameTek, strNamePro0;
			strWildcard.upr (); strNameTek = strWildcard.dup (); strNamePro0 = strWildcard.dup ();
			strNameTek.replace ("PRO*", "TEK"); strNamePro0.replace ("PRO*", "PRO0");
			HFileInfo *phfiTek = 0x0, *phfiPro0 = 0x0;
			HFILEINFO (phfiTek, WC_HSTRING (strNameTek));
            if (! (phfiTek == 0x0)) {
			    CTime tmTek = phfiTek->tmLastWritten (), tmPro0;
                BOOL bParse = TRUE;
			    if ((tmTek.GetHour () == 0) && (tmTek.GetMinute () == 0)) {
                    if (iCountItem == (pCST->GetMaxExt () + 1)) {
                        HFILEINFO (phfiPro0, WC_HSTRING (strNamePro0));
				        tmPro0 = phfiPro0->tmLastWritten ();

                        if ((tmTek - tmPro0).GetTotalSeconds () < 64800)
                            bParse = FALSE;
				        else ;
                    }
                    else {
                        bParse = FALSE;
                    }

				    if (bParse) {
                        for (i = 0; i < iCountItem; i ++) {
					        if (! (pPseudoPosSelectedItem [i] == 1))
						        m_arPosSelectedItem.Add (POSITION (pPseudoPosSelectedItem [i]));
					        else ;
				        }

                        OperationCopyNext (pCST);

				        //Копирование 'Tek' -> 'Pro0'
				        CopyFileToFile (strNameTek, strNamePro0, phfiTek->tmLastWritten ());
                    }
                    else {
                        //Реальное создание файлов
                    }
				
			    }
			    else ;

			    delete phfiTek; delete phfiPro0;

			    SetEvent (pCST->m_hEvtOperation);
            }
            else
                ; //phfiTek == 0x0
		}

		switch (WaitForSingleObject (pCST->m_hEvtOperation, 0)) {
			case WAIT_ABANDONED:
			case WAIT_TIMEOUT:
				//pPseudoPosSelectedItem = pCST->FillArrayPseudoPosSelectedItem (iCountItem);
				ptrThreadWriteData->PostThreadMessage (ID_OPERATION_CREATE, (WPARAM) pCST, (LPARAM) pPseudoPosSelectedItem);
				break;
			case WAIT_OBJECT_0:
				delete [] pPseudoPosSelectedItem;
				break;
			default: ;
		}
	}
	else
		;

	m_arPosSelectedItem.RemoveAll ();
}

//void HDlgGUIFilesDB::OnSourceDBSelectedChanged (NMHDR *, LRESULT *) {
afx_msg void HDlgGUIFilesDB::OnSourceDBSelectedChanged () {
    AfxGetApp ()->PostThreadMessage (WM_USER_SOURCEDB_SELECHANGED, 0x0, 0x0);
}

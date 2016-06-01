#include "stdafx.h"
#include "resource.h" //Идентификаторы 'ICON...'
#include <HDlgKTSfiles.h>

IMPLEMENT_DYNAMIC(HDlgKTSFiles, HDlgKTS)

EXT_DECLARE_HTHREAD_FILELOG

HDlgKTSFiles::HDlgKTSFiles (CWnd *pParent) : HDlgKTS (pParent),
                                    m_ptrComboBoxOperation (0x0),
                                    m_ptrListItemFiles (0x0),
                                    m_ptrCheckBoxAutoUpdate (0x0) {
}

HDlgKTSFiles::~HDlgKTSFiles (void) {
    int i;

    for (i = 0; i < m_arDescOperation.GetSize (); i ++) {
	    m_arDescOperation.GetAt (i)->Name.free ();
	    delete m_arDescOperation.GetAt (i);
	}
	m_arDescOperation.RemoveAll ();
}

BEGIN_MESSAGE_MAP(HDlgKTSFiles, HDlgKTS)
	ON_WM_DESTROY()
    ON_NOTIFY (NM_CLICK, IDC_TREECTRL_TEMPLATE, &OnNotifyNMClick)
    //ON_COMMAND (IDM_RESTORE, &HDlgKTSFiles::OnCommandRestoreDlg)
    ON_BN_CLICKED (IDC_CHECK_AUTO_UPDATE, &OnBnClickedCheckAutoUpdate)
    ON_BN_CLICKED (IDC_COMMAND_OPERATION, &OnBnClickedCommandOperation)
    ON_MESSAGE (WM_CHANGE_STAFF_OPERATION, &OnChangeStaffMenu)
    ON_MESSAGE (WM_USER_AUTO_CREATE, &OnAutoCreate)
    ON_NOTIFY (TVN_SELCHANGED, IDC_TREECTRL_TEMPLATE, &HDlgKTSFiles::OnTvnSelchangedTemplate)
    ON_NOTIFY (NM_DBLCLK, IDC_LIST_ITEM_FILES, &OnNmItemdblclickListItemFiles)
END_MESSAGE_MAP ()

void HDlgKTSFiles::DoDataExchange(CDataExchange* pDX) {
	HDlgKTS::DoDataExchange (pDX);
}

BOOL HDlgKTSFiles::OnInitDialog (void) {
	BOOL bRes = HDlgKTS::OnInitDialog ();

	return bRes;
}

int HDlgKTSFiles::CreateStatusBar (CWnd *pParent) {
    int iRes = 0;
    CRect rcThis;
    GetClientRect (&rcThis);

    int szSeparators = 2;
    m_ptrStatusBar = DEBUG_NEW CStatusBar ();
	m_ptrStatusBar->Create (pParent);
    m_arIdIndicators = DEBUG_NEW UINT [COUNT_INDICATORS];
    for (ID_INDICATORS id = ID_INDICATOR_TEMPLATE; id < COUNT_INDICATORS; id ++) { m_arIdIndicators [id] = WM_USER + 967 + id; }
    m_ptrStatusBar->SetIndicators (m_arIdIndicators, COUNT_INDICATORS);

    //Size the four panes
    m_ptrStatusBar->SetPaneInfo(ID_INDICATOR_TEMPLATE, m_arIdIndicators [ID_INDICATOR_TEMPLATE], SBPS_NORMAL, rcThis.Width () / (COUNT_INDICATORS + 1));
    m_ptrStatusBar->SetPaneInfo(ID_INDICATOR_FILE, m_arIdIndicators [ID_INDICATOR_FILE], SBPS_NORMAL, rcThis.Width () / (COUNT_INDICATORS + 1));
    m_ptrStatusBar->SetPaneInfo(ID_INDICATOR_ERROR, m_arIdIndicators [ID_INDICATOR_ERROR], SBPS_NORMAL, rcThis.Width () / (COUNT_INDICATORS + 1));
    m_ptrStatusBar->SetPaneInfo(ID_INDICATOR_TIME_PASSED, m_arIdIndicators [ID_INDICATOR_TIME_PASSED], SBPS_NORMAL, (rcThis.Width () - 3 * (rcThis.Width () / (COUNT_INDICATORS + 1)) - 4 * (COUNT_INDICATORS - 1) * szSeparators) / 2);
    m_ptrStatusBar->SetPaneInfo(ID_INDICATOR_TIME_CURRENT, m_arIdIndicators [ID_INDICATOR_TIME_CURRENT], SBPS_STRETCH, 0);

    RepositionBars (AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_TIME_CURRENT);

    return iRes;
}

int HDlgKTSFiles::CreateCommonCtrl (CWnd *pParent) {
    int iRes = -1;
    BOOL bRes = FALSE;
    CRect rcThis, rcCtrl,
        rcTreeCtrl, rcStatusBar;
    GetClientRect (&rcThis);

    CButton *pButton = 0x0;

    m_ptrStatusBar->GetClientRect (&rcStatusBar);
    GetDlgItem (IDC_TREECTRL_TEMPLATE)->GetClientRect (&rcTreeCtrl);

    pButton = static_cast <CButton *> (GetDlgItem (IDCANCEL));
    if (! (pButton == 0x0)) delete pButton; else ;
    pButton = DEBUG_NEW CButton ();
    rcCtrl.left = rcThis.Width () - 70 - OFFSET_CONTROL; rcCtrl.right = rcThis.Width () - OFFSET_CONTROL;
    rcCtrl.top = rcThis.Height () - HEIGHT_CONTROL - OFFSET_CONTROL - rcStatusBar.Height (); rcCtrl.bottom = rcThis.Height () - OFFSET_CONTROL - rcStatusBar.Height ();
    bRes = pButton->Create (_T ("Закрыть"), WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | WS_TABSTOP, rcCtrl, pParent, IDCANCEL);
    ::SendDlgItemMessage (m_hWnd, IDCANCEL, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);

    pButton = static_cast <CButton *> (GetDlgItem (IDC_COMMAND_OPERATION));
    if (! (pButton == 0x0)) delete pButton; else ;
    pButton = DEBUG_NEW CButton ();
    rcCtrl.left = rcThis.Width () - 100 - 2 * OFFSET_CONTROL; rcCtrl.right = rcThis.Width () - 70 - 2 * OFFSET_CONTROL;
    rcCtrl.top = rcThis.Height () - HEIGHT_CONTROL - OFFSET_CONTROL - rcStatusBar.Height (); rcCtrl.bottom = rcThis.Height () - OFFSET_CONTROL - rcStatusBar.Height ();
    bRes = pButton->Create (_T ("->"), WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | WS_TABSTOP, rcCtrl, pParent, IDC_COMMAND_OPERATION);

    //IDC_COMBO_OPERATION,151,171,170,104,CBS_DROPDOWNLIST | WS_TABSTOP
    m_ptrComboBoxOperation = static_cast <CComboBox *> (GetDlgItem (IDC_COMBO_OPERATION));
    if (! (m_ptrComboBoxOperation == 0x0)) delete m_ptrComboBoxOperation; else ;
    m_ptrComboBoxOperation = DEBUG_NEW CComboBox ();
    rcCtrl.left = rcTreeCtrl.right + 2 * OFFSET_CONTROL; rcCtrl.right = rcThis.Width () - 100 - 3 * OFFSET_CONTROL;
    rcCtrl.top = rcThis.bottom - rcStatusBar.Height () - OFFSET_CONTROL - HEIGHT_CONTROL; rcCtrl.bottom = rcCtrl.top + HEIGHT_CONTROL;
    iRes = m_ptrComboBoxOperation->Create (WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP, rcCtrl, pParent, IDC_COMBO_OPERATION);
    SendDlgItemMessage (IDC_COMBO_OPERATION, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);
    //m_ptrComboBoxOperation->ShowDropDown (TRUE);
    DDX_Control(m_ptrDataExchange, IDC_COMBO_OPERATION, *m_ptrComboBoxOperation);
    //m_ptrComboBoxOperation = &m_ComboBoxOperation;

    m_ptrCheckBoxAutoUpdate = static_cast <CButton *> (GetDlgItem (IDC_CHECK_AUTO_UPDATE));
    if (! (m_ptrCheckBoxAutoUpdate == 0x0)) delete m_ptrCheckBoxAutoUpdate; else ;
    m_ptrCheckBoxAutoUpdate = DEBUG_NEW CButton ();
    rcCtrl.left = rcTreeCtrl.right + 2 * OFFSET_CONTROL; rcCtrl.right = rcThis.Width () - OFFSET_CONTROL;
    rcCtrl.top = rcThis.Height () - 2 * HEIGHT_CONTROL - 2 * OFFSET_CONTROL - rcStatusBar.Height (); rcCtrl.bottom = rcCtrl.top + HEIGHT_CONTROL;
    iRes = m_ptrCheckBoxAutoUpdate->Create (_T ("Автоматическое обновление списка состояния"), WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | BS_3STATE | WS_TABSTOP, rcCtrl, pParent, IDC_CHECK_AUTO_UPDATE);
    ::SendDlgItemMessage (m_hWnd, IDC_CHECK_AUTO_UPDATE, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);
    DDX_Control (m_ptrDataExchange, IDC_CHECK_AUTO_UPDATE, *m_ptrCheckBoxAutoUpdate);

    m_ptrListItemFiles = static_cast <CListCtrl *> (GetDlgItem (IDC_LIST_ITEM_FILES));
    if (! (m_ptrListItemFiles == 0x0)) delete m_ptrListItemFiles; else ;
    m_ptrListItemFiles = DEBUG_NEW CListCtrl ();
    rcCtrl.left = rcTreeCtrl.right + 2 * OFFSET_CONTROL; rcCtrl.right = rcThis.Width () - OFFSET_CONTROL;
    rcCtrl.top = rcTreeCtrl.top + 2 * OFFSET_CONTROL + HEIGHT_CONTROL; rcCtrl.bottom = rcThis.Height () - 2 * HEIGHT_CONTROL - 3 * OFFSET_CONTROL - rcStatusBar.Height ();
    iRes = m_ptrListItemFiles->Create (WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP, rcCtrl, pParent, IDC_LIST_ITEM_FILES);
    DDX_Control (m_ptrDataExchange, IDC_LIST_ITEM_FILES, *m_ptrListItemFiles);
    m_ptrListItemFiles->SetExtendedStyle (m_ptrListItemFiles->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    return 0;
}

BOOL HDlgKTSFiles::PreTranslateMessage (MSG* pMsg) {
    return HDlgKTS::PreTranslateMessage (pMsg);
 }

void HDlgKTSFiles::OnDestroy (void) {
	delete GetDlgItem (IDC_TREECTRL_TEMPLATE);
	delete GetDlgItem (IDC_COMMAND_OPERATION);
    delete GetDlgItem (IDC_LIST_ITEM_FILES);
    delete GetDlgItem (IDC_COMBO_OPERATION);
    delete GetDlgItem (IDC_CHECK_AUTO_UPDATE);

	HDlgKTS::OnDestroy ();
}

void HDlgKTSFiles::OnNotifyNMClick (NMHDR *pNMHDR, LRESULT *pResult) {
    int iRes = 0; //Success
    BOOL bRes = false;

    if (MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_ADMIN) {
        //LPNMHDR lpnmh = (LPNMHDR) lParam;
        TVHITTESTINFO ht = {0};

        DWORD dwpos = GetMessagePos();

        // include <windowsx.h> and <windows.h> header files
        ht.pt.x = GET_X_LPARAM(dwpos);
        ht.pt.y = GET_Y_LPARAM(dwpos);

        //MapWindowPoints (HWND_DESKTOP, pNMHDR->hwndFrom, &ht.pt, 1);
        //MapWindowPoints (GetWindow (0), &ht.pt, 1);
        //MapWindowPoints (GetParent (), &ht.pt, 1);
        GetDlgItem (IDC_TREECTRL_TEMPLATE)->ScreenToClient (&ht.pt);

        TreeView_HitTest (pNMHDR->hwndFrom, &ht.pt);
        //TreeView_HitTest (m_hWnd, &ht.pt);
        //TreeView_HitTest (GetSafeHwnd (), &ht.pt);

        //if (TVHT_ONITEM & ht.flags)
        //if (TVHT_ONITEMSTATEICON & ht.flags)
        if (TVHT_ONITEMICON & ht.flags) {
            TREECTRL_KTS->ChangeItemImage (ht);
        
            //Вариант №1
            //TREECTRL_KTS->SelectItem (ht.hItem);
            
            //Вариант №2
            //Структура для посылки сообщения
            /*NMHDR hdr;
            hdr.code = TVN_SELCHANGED;
            hdr.hwndFrom = TREECTRL_KTS->m_hWnd;
            hdr.idFrom = GetDlgCtrlID ();*/
            
            //Сообщаем об изменении
            //GetParent ()->PostMessage (WM_NOTIFY, (WPARAM) m_hWnd, (LPARAM) &hdr);
            //PostMessage(hWnd, UM_CHECKSTATECHANGE, 0, (LPARAM) ht.hItem);
            //PostMessage (WM_NOTIFY, (WPARAM) TREECTRL_KTS->m_hWnd, (LPARAM) &hdr);
        }
        else
            ;
    }
    else
        ; // Нет ДОСТУПа к изменению
}

void HDlgKTSFiles::OnNmItemdblclickListItemFiles (NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMITEMACTIVATE phdr = reinterpret_cast <LPNMITEMACTIVATE> (pNMHDR);
    //LPNMITEMACTIVATE phdr = re_cast ("LPNMITEMACTIVATE", pNMHDR);
    
    // TODO: Add your control notification handler code here
    int iIdSElectedItem = TREECTRL_KTS->GetSelectedItemID ();
    //m_ptrListItemFiles->GetItemText (phdr->iItem, phdr->iSubItem);
    CString strItemText = m_ptrListItemFiles->GetItemText (phdr->iItem, 1);
    HTREEITEM hItem = (HTREEITEM) m_ptrListItemFiles->GetItemData (phdr->iItem);
    
    /*
    //Сообщением активировать элемент деревА НЕЛЬЗя ???
    NMHDR hdr;    
    //hdr.code = TVN_SELCHANGED;
    //hdr.hwndFrom = TREECTRL_KTS->m_hWnd;
    //hdr.idFrom = TREECTRL_KTS->GetDlgCtrlID ();
    
    hdr.code = TVN_SELCHANGED;
    hdr.hwndFrom = TREECTRL_KTS->GetSafeHwnd ();
    hdr.idFrom =  TREECTRL_KTS->GetDlgCtrlID ();
        
    int iCtrlID = TREECTRL_KTS->GetDlgCtrlID ();
    ::SendMessage (m_hWnd, WM_NOTIFY, (WPARAM) 0, (LPARAM) &hdr);
    */
    
    if (hItem)
        TREECTRL_KTS->SelectItem (hItem);
    else
        ;
    
    *pResult = 0;
}

void HDlgKTSFiles::OnBnClickedCheckAutoUpdate () {
    // TODO: Add your control notification handler code here
    int state = m_ptrCheckBoxAutoUpdate->GetCheck ();
    
    switch (state) {
        case BST_CHECKED:
            m_ptrCheckBoxAutoUpdate->SetCheck (BST_UNCHECKED);
            break;
        case BST_UNCHECKED:
            m_ptrCheckBoxAutoUpdate->SetCheck (BST_CHECKED);
            break;
        default:
            ;
    }
}

int HDlgKTSFiles::InitializeOperation (void) {
//m_pRSOperation В ОПЕРАТИВНУЮ ПАМЯТЬ
    int iRes = 0;
    DESC_OPERATION *pstDO = 0x0;
    
    if (PTR_DB) {
        HRecordset *pRSOperation = PTR_DB->GetRecordset (this, HString (_T (TABLENAME_OPERATION)));
        //m_pmapDBRS.Lookup (_T (TABLENAME_OPERATION), (void *&) pRSOperation);
    
        if (pRSOperation) {
            if (pRSOperation->IsOpen ()) {    
                /*if (pRSOperation->CanScroll ())
                    pRSOperation->MoveFirst ();
                else
                    ;*/
                
                while (! pRSOperation->IsEOF ()) {
                    pstDO = DEBUG_NEW DESC_OPERATION;
                
                    pRSOperation->GetFieldValue (_T("ACCESS_LEVEL"), pstDO->AccessLevel);
                    pRSOperation->GetFieldValue (_T("USE"), pstDO->bUse);
                    pRSOperation->GetFieldValue (_T("REQUIREMENT_TEMPLATE"), pstDO->bRequirementTemplate);
                    pRSOperation->GetFieldValue (_T("REQUIREMENT_MAXNUMBER"), pstDO->bRequirementMaxNumber);
                    pRSOperation->GetFieldValue ( _T("NAME"), pstDO->Name);
                    pRSOperation->GetFieldValue (_T("ID"), pstDO->Id);
                    pstDO->Id += ID_OPERATION_UPDATEDATA;
                    
                    m_arDescOperation.Add (pstDO);

                    pRSOperation->MoveNext ();
                }
            }
            else {
                //Open RS Operation invalid
                MessageBox (_T("Open RS Operation invalid!"), _T("Ошибка!"), MB_ICONINFORMATION);
                iRes = -1;
            }
        }
        else
            iRes = -1; //RS NULL

        PTR_DB->ReleaseRS (this, pRSOperation);
    }
    else
        ; //PTR_DB

    return iRes;
}

BOOL HDlgKTSFiles::SetEnableOperation (void) {
    int i = 0, iItem = -1, iSelectedItem = -1, iLevel = -1, iAccessLevel = -1;
    DWORD_PTR pItemData = NULL;
    BOOL bAddOperation;
    CMenu *subMenuOperation = 0x0;
    if (GetMenu ())
        subMenuOperation = GetMenu ()->GetSubMenu (1);
    else
        ;

    iLevel = TREECTRL_KTS->GetSelectedLevel ();

    while (m_ptrComboBoxOperation->GetCount ())
        m_ptrComboBoxOperation->DeleteString (0);

    if (subMenuOperation)
        while (subMenuOperation->GetMenuItemCount ())
            subMenuOperation->DeleteMenu (0, MF_BYPOSITION);
    else
        ;

	//ASSERT (m_pRSOperation);

    if (m_arDescOperation.GetSize ()) {     
		for (i = 0; i < m_arDescOperation.GetSize (); i ++) {
			bAddOperation = false;
			iAccessLevel = m_arDescOperation.GetAt (i)->AccessLevel;
			if ((iLevel & iAccessLevel) && (m_arDescOperation.GetAt (i)->bUse)) {
				bAddOperation = true;
                if (TREECTRL_KTS->GetSelectedItemIsTemplate ()) {
                    ITEM_DATA *pItemData = ((ITEM_DATA *) TREECTRL_KTS->GetItemData (TREECTRL_KTS->GetSelectedItem ()));
				    //REQUIREMENT_TEMPLATE
				    if (m_arDescOperation.GetAt (i)->bRequirementTemplate && (HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_TEMPLATE & iAccessLevel))
						if ((pItemData->auto_create == TRUE) && (MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_LOCAL))
							//if (! (MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_DISABLED_AUTOCREATE))
                                bAddOperation = false;
                            //else
                            //    ;
                        else
                            ;
				    else
					    ;

                    //REQUIREMENT_MAXNUMBER
                    if (m_arDescOperation.GetAt (i)->bRequirementMaxNumber && (HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_TEMPLATE & iAccessLevel))
                        if (pItemData->max_number == 0)
							bAddOperation = false;
                        else
                            ;
				    else
                        ;
                }
				else
					/*bAddOperation = false*/;
			    
				if (bAddOperation) {
					iItem = m_ptrComboBoxOperation->AddString (WC_HSTRING (m_arDescOperation.GetAt (i)->Name));
					pItemData = (DWORD_PTR) m_arDescOperation.GetAt (i)->Id;
					m_ptrComboBoxOperation->SetItemData (iItem, pItemData);
			        
					//по умолчанию выбранная операция 'Обновить информацию'
					if (m_arDescOperation.GetAt (i)->Id == ID_OPERATION_UPDATEDATA)
						iSelectedItem = iItem;
					else
						;
			            
					if (subMenuOperation)
                        subMenuOperation->AppendMenuW (MF_ENABLED, m_arDescOperation.GetAt (i)->Id, WC_HSTRING (m_arDescOperation.GetAt (i)->Name));
                    else
                        ;
				}
				else
					;
			}
		}

		m_ptrComboBoxOperation->SetCurSel (iSelectedItem);
		//m_ptrComboBoxOperation->ShowDropDown (true);
            
		return TRUE;  // return TRUE  unless you set the focus to a control
	}
    else {
        //MessageBox (_T("Массив списка доступных операций ПУСТой!"), _T("Ошибка!"), MB_ICONINFORMATION);
        
        return FALSE;
    }
}

BOOL HDlgKTSFiles::SetListBoxItemFilesText (HCheckStateTemplate *pCST, int iItem, BOOL bNewItem) {
	BOOL bRes = true;
	
#if defined _DEBUG
    extern CMemoryState g_memStateBegin, g_memStateEnd, g_memStateDiff;
    g_memStateBegin.Checkpoint ();
#endif

	HString strItem;
    int iSubItem;
	
	//Изменён порядок установки значений колонок в зависимости от
	//Новая строка ИЛИ модификация существующей
	
	LVCOLUMN *plvCol = DEBUG_NEW LVCOLUMN;
	plvCol->cchTextMax = 256;
	plvCol->pszText = DEBUG_NEW TCHAR [plvCol->cchTextMax];
	iSubItem = 0;
	while (m_ptrListItemFiles->GetColumn (iSubItem, plvCol)) {
		//if (((iSubItem == 1) || (iSubItem == 2) || (iSubItem == 3)) && ((! bNewItem)))
		if ((iSubItem == 1) && (! bNewItem)) {
		    //Пропуск с 1-ой по 4-ю КОЛонки
			iSubItem  = 4;
			continue;
		}
        else
            ;

		switch (iSubItem) {
			case 0:
				strItem = GetStrState (pCST->GetStateItem (iItem));
				break;
			case 1:
				strItem = pCST->GetDateInfoSample (iItem);
				break;
			case 2:
				strItem = pCST->GetTimeInfoSample (iItem);
				break;
			case 3:
				strItem = pCST->GetExtInfoSample (iItem);
				break;
			case 4:
				strItem = pCST->GetDateInfoTemplate (iItem);
				break;
			case 5:
				strItem = pCST->GetTimeInfoTemplate (iItem);
				break;
			default: ;
		}

		if (! m_ptrListItemFiles->SetItemText(iItem, iSubItem, strItem.GetBuffer ())) {
            bRes = false;
			break;
		}
        else
            ;

		iSubItem ++;
	}

	delete plvCol->pszText;
	delete plvCol;
	
	strItem.free ();
	
#if defined _DEBUG
    g_memStateEnd.Checkpoint ();
    if (g_memStateDiff.Difference (g_memStateBegin, g_memStateEnd)) {
        TRACE (_T ("Memory leaked!\n"));
    }
    else
        ;
#endif
	
	return bRes;
}

CString HDlgKTSFiles::GetStrState (int state) {
	CString strRes (_T(""));
	
	switch (state) {
		case STATE_ITEM_NODATA:
			strRes = CString (_T("Нет данных"));
			break;
		case STATE_ITEM_OK:
			strRes = CString (_T("OK"));
			break;
		case STATE_ITEM_FAILED:
			strRes = CString (_T("Ошибка"));
			break;
		default: ;
	}
	
	return strRes;
}

BOOL HDlgKTSFiles::OnCmdMsg (UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
    FILELOG_VARIABLE

    int i = -1;
    CString strTemp;
    strTemp.Format (_T ("CDialog::OnCmdMsg::default {nID=%x, nCode=%x}\n"), nID, nCode);
    
    if (pHandlerInfo == NULL) {
        switch (nCode) {
            //case -1:
            case CN_COMMAND: {
                    //if ((nID > PREFIX_IDM_OPERATION) && (nID <= (PREFIX_IDM_OPERATION + GetMenu ()->GetSubMenu (1)->GetMenuItemCount ())))
                    //if ((nID > PREFIX_IDM_OPERATION) && (nID <= (PREFIX_IDM_OPERATION + m_arDescOperation.GetSize ())))
                    if ((nID > PREFIX_IDM_OPERATION) && (nID <= ID_LAST_OPERATION)) {
                        if (nID == ID_OPERATION_UPDATEDATA)
                            UpdateInfoListBoxItemFiles ();
                        else {
                            if (m_ptrListItemFiles->GetSelectedCount ()) {
                                HCheckStateTemplate *pCST = TREECTRL_KTS->GetObjectCheckStateTemplate (TREECTRL_KTS->GetSelectedItem ());
                                if (pCST) {
                                    switch (WaitForSingleObject (pCST->m_hEvtOperation, 0x0)) {
                                        case WAIT_ABANDONED:
		                                case WAIT_TIMEOUT:
		                                    //По ЭТОМу ШАБЛОНу всё ещё продолжается РАБОТа
		                                    MessageBox (_T ("Повторите попытку после окончания текущей операции!"), _T ("Внимание!"), MB_ICONINFORMATION);
			                                break;
		                                case WAIT_OBJECT_0:
		                                    ResetEvent (pCST->m_hEvtOperation);
        		                        
                                            GetSelectedListItemFiles ();
                                        
                                            switch (nID) {
                                                case ID_OPERATION_DELETE: //Удалить
                                                    OperationDel (pCST);
                                                    break;
                                                case ID_OPERATION_CREATE: //Создать
                                                    OperationCreate (pCST);
                                                    //SetEvent (pCST->m_hEvtOperation); //ПО ОКОНЧании ПОТОКа
                                                    break;
                                                case ID_OPERATION_INCREMENTEXT: //ИНкрементировать номер расширения
                                                    OperationIncNumberExt (pCST);
                                                    break;
                                                case ID_OPERATION_DECREMENTEXT: //ДЕкрементировать номер расширения
                                                    OperationDecNumberExt (pCST);
                                                    break;
                                                case ID_OPERATION_COPYPREV: //Копировать предыдущий
                                                    OperationCopyPrev (pCST);
                                                    break;
                                                case ID_OPERATION_COPYNEXT: //Копировать следующий
                                                    OperationCopyNext (pCST);
                                                    break;
                                                case ID_OPERATION_CHANGE_DATETIME: //Корректировать дата/время
                                                    OperationModifyDT (pCST);
                                                    break;
                                                case ID_OPERATION_CREATE_ALTERNATIVE: //Создать из АЛЬТЕРНАТИВНОГо источникА
                                                    OperationCreate (pCST, 1);
                                                    //SetEvent (pCST->m_hEvtOperation); //ПО ОКОНЧании ПОТОКа
                                                    break;
                                                default:
                                                    ;
                                            }

                                            if ((! (nID == ID_OPERATION_CREATE)) && (! (nID == ID_OPERATION_CREATE_ALTERNATIVE)))
                                                SetEvent (pCST->m_hEvtOperation);
                                            else
                                                ;

                                            m_arPosSelectedItem.RemoveAll ();
                                            m_arPosSelectedItem.FreeExtra ();
                                            break;
                                        default:
                                            ;
                                    }
                                }
                                else
                                    ; //Не найден элемент с массивом для текущего ШАБЛОНа
                                /*
						        SetEvent (m_hEvtDirectoryWatched);
						        */
                            }
                            else
                                MessageBox (_T ("Не выделено НИ одного файла!"), _T ("Внимание!"), MB_ICONINFORMATION);
                        }
                    }
                    else {
                    }
                }
                break;
            case WM_NOTIFY:
                {
                    //TRACE ("HDlgKTSFiles::OnCmdMsg::WM_NOTIFY");
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
    
    return HDlgKTS::OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
}

void HDlgKTSFiles::GetSelectedListItemFiles (void) {
    POSITION pos;
    int i;
    
    if (m_arPosSelectedItem.GetSize ()) {
        m_arPosSelectedItem.FreeExtra ();
        m_arPosSelectedItem.RemoveAll ();
    }
    else
        ;

    pos = m_ptrListItemFiles->GetFirstSelectedItemPosition();
    while (m_arPosSelectedItem.GetSize () < m_ptrListItemFiles->GetSelectedCount ()) {
        m_arPosSelectedItem.Add (pos);
        i = m_ptrListItemFiles->GetNextSelectedItem (pos);
    }
}

void HDlgKTSFiles::UpdateInfoListBoxItemFiles () {
    NMHDR hdr;
    HTREEITEM hItem, hItemChild;

    switch (TREECTRL_KTS->GetSelectedLevel ()) {
        case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_ROOT: //Выбрана корневая запись
		    hItem = TREECTRL_KTS->GetNextItem (TREECTRL_KTS->GetRootItem (), TVGN_CHILD);
		    while (hItem) {
			    hItemChild = TREECTRL_KTS->GetNextItem (hItem, TVGN_CHILD);
			    if (! hItemChild == 0x0) {
		            while (hItemChild) {
		                HCheckStateTemplate *pCST = TREECTRL_KTS->GetObjectCheckStateTemplate (TREECTRL_KTS->GetItemID (hItemChild));
		                if (pCST)
			                pCST->ClearTimeUpdate ();
                        else
                            ;
        			    			
		                hItemChild = TREECTRL_KTS->GetNextItem (hItemChild, TVGN_NEXT);
		            }
		        }
			    			    			
		        hItem = TREECTRL_KTS->GetNextItem (hItem, TVGN_NEXT);
		    }
            break;
        case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_CONTAINER: //Выбран один из БЛОКов    			
		    hItem = TREECTRL_KTS->GetNextItem (TREECTRL_KTS->GetSelectedItem (), TVGN_CHILD);
		    while (hItem)
		    {    			    
		        HCheckStateTemplate *pCST = TREECTRL_KTS->GetObjectCheckStateTemplate (TREECTRL_KTS->GetItemID (hItem));
		        if (pCST)
	                pCST->ClearTimeUpdate ();
                else
                    ;
						
		        hItem = TREECTRL_KTS->GetNextItem (hItem, TVGN_NEXT);
		    }
            break;
        case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_TEMPLATE:
		    HCheckStateTemplate *pCST = TREECTRL_KTS->GetObjectCheckStateTemplate (TREECTRL_KTS->GetSelectedItem ());
		    if (pCST)
		        pCST->ClearTimeUpdate ();
		    else
			    ;
            //'break' для 'case 2'
		    break;
    }
    
    hdr.code = TVN_SELCHANGED;
    hdr.hwndFrom = TREECTRL_KTS->GetSafeHwnd ();
    hdr.idFrom = GetDlgCtrlID ();
        
    SendMessage(WM_NOTIFY, (WPARAM) TREECTRL_KTS->GetSafeHwnd (), (LPARAM) &hdr);
}

void HDlgKTSFiles::OnBnClickedCommandOperation () {
    // TODO: Add your control notification handler code here
    CString strItem;
    FILELOG_VARIABLE

    m_ptrComboBoxOperation->GetLBText (m_ptrComboBoxOperation->GetCurSel (), strItem.GetBuffer (m_ptrComboBoxOperation->GetLBTextLen (m_ptrComboBoxOperation->GetCurSel ())));
    
    strLog.format (_T ("Инициирована операция - %S! Код: %i."), WC_HSTRING (HString (strItem.GetBuffer ())), m_ptrComboBoxOperation->GetItemData (m_ptrComboBoxOperation->GetCurSel ()));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), COMMON_KTS_DLG);

    SendMessage (WM_COMMAND, m_ptrComboBoxOperation->GetItemData (m_ptrComboBoxOperation->GetCurSel ()), 0);
}

LRESULT HDlgKTSFiles::OnChangeStaffMenu (WPARAM wp, LPARAM lp) {
    SetEnableOperation ();
    
    return 0;
}

void HDlgKTSFiles::OnTvnSelchangedTemplate(NMHDR *pNMHDR, LRESULT *pResult) {
    BOOL bRes  = false;
    int iItemState = -1, iItem = 0, iSubItem = 0, iCountFileFind = 0, iCountTemplateFind = 0, iCountError = 0;
    CString strItem, strFileFind, strTemplateFind, strError, strDateTime;
    HCheckStateTemplate *pCST = 0x0; //Для метки LEVEL_TREECTRL_TEMPLATE
    HTREEITEM hItem, hItemChild;
    LPNMTREEVIEW pNMTREEVIEW;
    pNMTREEVIEW = (LPNMTREEVIEW) pNMHDR;
    FILELOG_VARIABLE
   
    CRect rcClient;
    m_ptrListItemFiles->GetClientRect(&rcClient);
        
    m_ptrListItemFiles->DeleteAllItems ();
	while (m_ptrListItemFiles->GetHeaderCtrl()->GetItemCount()) {
		m_ptrListItemFiles->DeleteColumn (0);
	}
	
	iItem = 0;

    int iSelectLevelTreeCtrl = TREECTRL_KTS->GetSelectedLevel ();

    if (! (iSelectLevelTreeCtrl == HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_TEMPLATE)) {
        m_ptrCheckBoxAutoUpdate->SetCheck (BST_INDETERMINATE);
        m_ptrCheckBoxAutoUpdate->EnableWindow (false);
        
        iSubItem = 0;
		m_ptrListItemFiles->InsertColumn(iSubItem ++, _T ("Состояние"), LVCFMT_LEFT, (rcClient.right - rcClient.left) / 3, -1);
		m_ptrListItemFiles->InsertColumn(iSubItem ++, _T ("Наименование"), LVCFMT_LEFT, (rcClient.right - rcClient.left) / 3 * 2, -1);
    }
    else {
        m_ptrCheckBoxAutoUpdate->SetCheck (BST_CHECKED);
        m_ptrCheckBoxAutoUpdate->EnableWindow ();
        
		m_ptrListItemFiles->InsertColumn(iSubItem ++, _T("Состояние"), LVCFMT_LEFT, (rcClient.right - rcClient.left) / 6, -1);
	    m_ptrListItemFiles->InsertColumn(iSubItem ++, _T("Дата"), LVCFMT_LEFT, (rcClient.right - rcClient.left) / 6, -1);
	    m_ptrListItemFiles->InsertColumn(iSubItem ++, _T("Время"), LVCFMT_LEFT, (rcClient.right - rcClient.left) / 7, -1);
		m_ptrListItemFiles->InsertColumn(iSubItem ++, _T("Расширение"), LVCFMT_LEFT, (rcClient.right - rcClient.left) / 7, -1);
		m_ptrListItemFiles->InsertColumn(iSubItem ++, _T("Дата"), LVCFMT_LEFT, (rcClient.right - rcClient.left) / 6, -1);
	    m_ptrListItemFiles->InsertColumn(iSubItem ++, _T("Время"), LVCFMT_LEFT, (rcClient.right - rcClient.left) / 7, -1);
    }
	
    switch (iSelectLevelTreeCtrl) {
	case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_ROOT: //Выбрана корневая запись
            hItem = TREECTRL_KTS->GetNextItem (TREECTRL_KTS->GetRootItem (), TVGN_CHILD);
			while (hItem) {
			    iSubItem = 0;
			    iItemState = STATE_ITEM_OK;
				
				hItemChild = TREECTRL_KTS->GetNextItem (hItem, TVGN_CHILD);
				if (hItemChild == 0x0)
				    iItemState = STATE_ITEM_NODATA;
				else {
			        while (hItemChild) {
			            HCheckStateTemplate *pCST = TREECTRL_KTS->GetObjectCheckStateTemplate (TREECTRL_KTS->GetItemID (hItemChild));
			            if (pCST) {
				            pCST->Update ();
				            if (iItemState > pCST->GetState ())
				                iItemState = pCST->GetState ();
                            else
                                ;
    				            
				            iCountError += pCST->GetCountError ();
				            iCountFileFind += pCST->GetCountFileFind ();
				            iCountTemplateFind ++;
                        }
                        else {
                        }
        			    			
			            hItemChild = TREECTRL_KTS->GetNextItem (hItemChild, TVGN_NEXT);
			        }
			    }
				
				strItem = GetStrState (iItemState);
				
				if (m_ptrListItemFiles->InsertItem (LVIF_TEXT | LVIF_STATE | LVIF_PARAM, iItem, strItem, 0, LVIS_SELECTED, 0, (LPARAM) hItem) > -1) {
				//if (m_ptrListItemFiles->InsertItem (LVIF_TEXT, iItem, strItem, 0, 0, 0, 0) > -1) {
				    iSubItem = 1;
				    strItem = TREECTRL_KTS->GetItemText (hItem);
				    bRes = m_ptrListItemFiles->SetItemText(iItem, iSubItem, strItem);
				    
				    if (bRes) {
				        iItem ++;
				    }
				    else
				        m_ptrListItemFiles->DeleteItem (iItem);
				}
			    			
			    hItem = TREECTRL_KTS->GetNextItem (hItem, TVGN_NEXT);
			}
            break;
        case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_CONTAINER: //Выбран один из БЛОКов
			hItem = TREECTRL_KTS->GetNextItem (TREECTRL_KTS->GetSelectedItem (), TVGN_CHILD);
			while (hItem) {
			    iSubItem = 0;
			    
			    HCheckStateTemplate *pCST = TREECTRL_KTS->GetObjectCheckStateTemplate (TREECTRL_KTS->GetItemID (hItem));
			    if (pCST) {
				    pCST->Update ();
				    strItem = GetStrState (pCST->GetState ());

				    iCountError += pCST->GetCountError ();
				    iCountFileFind += pCST->GetCountFileFind ();
				    iCountTemplateFind ++;
                }
                else {
                    strItem = CString (_T ("Нет данных"));
                }

				if (m_ptrListItemFiles->InsertItem (LVIF_TEXT | LVIF_STATE | LVIF_PARAM, iItem, strItem, 0, LVIS_SELECTED, 0, (LPARAM) hItem)> -1) {
				    iSubItem = 1;
				    strItem = TREECTRL_KTS->GetItemText (hItem);
				    bRes = m_ptrListItemFiles->SetItemText(iItem, iSubItem, strItem);

				    if (bRes)
				        iItem ++;
                    else
                        m_ptrListItemFiles->DeleteItem (iItem);
                }
                else ;

			    hItem = TREECTRL_KTS->GetNextItem (hItem, TVGN_NEXT);
			}
            break;
        case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_TEMPLATE:
			/*
			//Так сортируем об'ектЫ ЛЮБОГО класса
			// Create an array object
			HTypedSortableArray <HCheckStateTemplate *> arTemp;

			srand (999); // Generate seed for rand()
			for (i = 0; i < 6; i ++)
			{
				HCheckStateTemplate *pCST = DEBUG_NEW HCheckStateTemplate (rand ());
				TRACE ("%i\n", pCST->GetID ());
				arTemp.Add (pCST);
			}
			
			long lBeginTick = GetTickCount ();

			arTemp.Sort (HCheckStateTemplate::CompareID);
			for (i = 0; i < 6; i++)
			{
				TRACE ("%i\n", arTemp [i]->GetID ());
			}

			TRACE ("Прошло тиков = %li\n", GetTickCount() - lBeginTick);

			arTemp.FreeExtra ();
			*/

			pCST = TREECTRL_KTS->GetObjectCheckStateTemplate (TREECTRL_KTS->GetSelectedItem ());
			if (pCST) {
				pCST->Update ();

				iItem = 0;
				//while (iItem < pCST->GetCountFileFind ())
				while (iItem < pCST->GetCountInfoSampleItem ()) {
					if (m_ptrListItemFiles->InsertItem (LVIF_TEXT | LVIF_STATE | LVIF_PARAM, iItem, _T(""), 0, 0, 0, 0) > -1) {	                
					    if (! SetListBoxItemFilesText (pCST, iItem ++))
							m_ptrListItemFiles->DeleteItem (iItem);
                        else
                            ;
                    }
                    else
                        ;
				}

				//Для отчёта в строке статуса ???
                iCountError = pCST->GetCountError ();
                iCountFileFind = pCST->GetCountFileFind ();
                iCountTemplateFind ++;
			}
			else
				iCountFileFind = 0;

            //'break' для 'case 2'
			break;
        default:
            ;
    }

	strLog = _T ("Просмотр: ") + TREECTRL_KTS->GetItemText (TREECTRL_KTS->GetSelectedItem ());
	strLog.appendChar (' ');

	//strLog.appendChar (0x20);
	strLog.appendChar ('-');
	strLog.appendChar (' ');

	strTemplateFind.Format (_T ("Шаблонов: %i"), iCountTemplateFind);
    m_ptrStatusBar->SetPaneText (ID_INDICATOR_TEMPLATE, strTemplateFind);
	strLog += strTemplateFind;
	strLog.appendChar (' ');
    
    strFileFind.Format (_T ("Файлов: %i"), iCountFileFind);
    m_ptrStatusBar->SetPaneText (ID_INDICATOR_FILE, strFileFind);
	strLog += strFileFind;
	strLog.appendChar (' ');
    
    strError.Format (_T ("Ошибок: %i"), iCountError);
    m_ptrStatusBar->SetPaneText(ID_INDICATOR_ERROR, strError);
	strLog += strError;
	strLog.appendChar (' ');
    
    strLog += TimeIndicators (ID_INDICATOR_TIME_PASSED, ID_INDICATOR_TIME_CURRENT);
    
    //Изменение доступности пунктов в контекстном меню (0-й индекс основного меню)
	TREECTRL_KTS->SetEnableMenuTemplate ();
	//Изменение состава "Действия" в ~ от типа элемента в "Дереве шаблонов" в 'ComboBoxOperation'
	SetEnableOperation ();

	FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), COMMON_KTS_DLG);
	strLog.free ();
}

LRESULT HDlgKTSFiles::OnAutoCreate (WPARAM wp, LPARAM) {
    HCheckStateTemplate *pCST = (HCheckStateTemplate *) wp;

    switch (WaitForSingleObject (pCST->m_hEvtOperation, 0x0)) {
        case WAIT_ABANDONED:
		case WAIT_TIMEOUT:
		    //По ЭТОМу ШАБЛОНу всё ещё продолжается РАБОТа
		    MessageBox (_T ("Повторите попытку после окончания текущей операции!"), _T ("Внимание!"), MB_ICONINFORMATION);
			break;
		case WAIT_OBJECT_0:
            ResetEvent (pCST->m_hEvtOperation);

            OperationCreate (pCST);
        default:
            ;
    }

    return 0;
}

#define SET_FULLPATH(fullPath, num) strNumber = HString (m_ptrListItemFiles->GetItemText (num, 3).GetBuffer ()); \
									if (! (fullPath.reverseFind ("*") == (fullPath.length () - 1))) { \
										strNumber.free (); \
										strNumber.appendNumber ((DWORD) num); \
									} else ; \
									fullPath.replace (_T ("*"), WC_HSTRING (strNumber));

void HDlgKTSFiles::OperationCopyPrev (HCheckStateTemplate *pCST) {
    HString strFullPath, strFullPathNew;

    int i = -1;
	HString strNumber;
    
    /*CString strLog;
    strLog.Format (_T ("'WaitFor...m_hEvtOperation' для 'CUserKTSDlg::OperationCopyPrev'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_3);*/
    
  //  switch (WaitForSingleObject (pCST->m_hEvtOperation, 0x0))
  //  {
  //      case WAIT_ABANDONED:
		//case WAIT_TIMEOUT:
		//    //По ЭТОМу ШАБЛОНу всё ещё продолжается РАБОТа
		//    MessageBox (_T ("Повторите попытку после окончания текущей операции!"), _T ("Внимание!"), MB_ICONINFORMATION);
		//	break;
		//case WAIT_OBJECT_0:
		//    ResetEvent (pCST->m_hEvtOperation);

		    i = 0;

            while (i < m_arPosSelectedItem.GetSize ()) {
                if (((int) m_arPosSelectedItem.GetAt (i) < m_ptrListItemFiles->GetItemCount ()) && ((int) m_arPosSelectedItem.GetAt (i) > 1)) {
                    strFullPathNew = pCST->GetFullPathTemplate ();
					SET_FULLPATH (strFullPathNew, (int) (m_arPosSelectedItem.GetAt (i) - 1))
                    
                    //strFullPath = strFullPathNew;
                    strFullPath = pCST->GetFullPathTemplate ();
                    //strFullPath.Replace (m_ptrListItemFiles->GetItemText ((int) (m_arPosSelectedItem.GetAt (i) - 1), 3), m_ptrListItemFiles->GetItemText ((int) (m_arPosSelectedItem.GetAt (i) - 0), 3));
					SET_FULLPATH (strFullPath, (int) (m_arPosSelectedItem.GetAt (i) - 0))
                    
                    CopyFileToFile (strFullPath, strFullPathNew,
										pCST->GetDTModifyInfoSample ((int) m_arPosSelectedItem.GetAt (i) - 1));
                }
                else
                    ; //Для НИЖНЕГО НЕТ предыдущего
                i ++;
            }
            
            /*SetEvent (pCST->m_hEvtOperation);
            break;
        default:
            ;
    }*/
    
    strFullPath.free (); strFullPathNew.free ();
}

void HDlgKTSFiles::OperationCopyNext (HCheckStateTemplate *pCST) {
    FILELOG_VARIABLE

    HString strFullPath, strFullPathNew;

    int i = 0;
	HString strNumber;
    
    /*CString strLog;
    strLog.Format (_T ("'WaitFor...m_hEvtOperation' для 'CUserKTSDlg::OperationCopyNext'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_3);*/
    
  //  switch (WaitForSingleObject (pCST->m_hEvtOperation, 0x0))
  //  {
  //      case WAIT_ABANDONED:
		//case WAIT_TIMEOUT:
		//    //По ЭТОМу ШАБЛОНу всё ещё продолжается РАБОТа
		//    MessageBox (_T ("Повторите попытку после окончания текущей операции!"), _T ("Внимание!"), MB_ICONINFORMATION);
		//	break;
		//case WAIT_OBJECT_0:
		//    ResetEvent (pCST->m_hEvtOperation);

			i = m_arPosSelectedItem.GetUpperBound ();

            while (i > -1) {
                //if ((int) m_arPosSelectedItem.GetAt (i) < (m_ptrListItemFiles->GetItemCount () + 1)) {
					//strFullPathNew = TREECTRL_KTS->GetItemFullPath (pCST->GetHTreeItem ());
					strFullPathNew = pCST->GetFullPathTemplate ();
                    SET_FULLPATH (strFullPathNew, (int) (m_arPosSelectedItem.GetAt (i) - 1))
									/*strNumber = HString (m_ptrListItemFiles->GetItemText ((int) (m_arPosSelectedItem.GetAt (i) - 1), 3).GetBuffer ()); \
									if (! (strFullPathNew.reverseFind ("*") == (strFullPathNew.length () - 1))) { \
										strNumber.free (); \
										strNumber.appendNumber ((DWORD) (int) (m_arPosSelectedItem.GetAt (i) - 1)); \
									} else ; \
									strFullPathNew.replace (_T ("*"), WC_HSTRING (strNumber));*/

                    strFullPath = pCST->GetFullPathTemplate ();
					SET_FULLPATH (strFullPath, (int) (m_arPosSelectedItem.GetAt (i) - 2))
									/*strNumber = HString (m_ptrListItemFiles->GetItemText (num, 3).GetBuffer ()); \
									if (! (fullPath.reverseFind ("*") == (fullPath.length () - 1))) { \
										strNumber.free (); \
										strNumber.appendNumber ((DWORD) (int) (m_arPosSelectedItem.GetAt (i) - 1)); \
									} else ; \
									fullPath.replace (_T ("*"), WC_HSTRING (strNumber));*/

                    strLog.format ("HDlgKTSFiles::OperationCopyNext...path = %S, pathNew = %S", WC_HSTRING (strFullPath), WC_HSTRING (strFullPathNew));
                    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);

                    CopyFileToFile (strFullPath, strFullPathNew,
										pCST->GetDTModifyInfoSample ((int) m_arPosSelectedItem.GetAt (i) - 1));
                //}
                //else
                //    ; //НИЖНИЙ НЕЛЬЗЯ ЗАМЕЩАТЬ
                //    ; //Для ВЕРХНЕГо НЕТ СЛЕДУЮЩЕГО

                i --;
            }
    
            /*SetEvent (pCST->m_hEvtOperation);
            break;
        default:
            ;
    }*/
    
    strFullPath.free (), strFullPathNew.free ();
}

void HDlgKTSFiles::OperationIncNumberExt (HCheckStateTemplate *pCST) {
    HString strFullPath, strFullPathNew, strNumber;
    CFileFind filefind;
    //CFileStatus fStatus;
    //CFile file;
    //LVITEM lvItem;

    int i = 0;
    
    /*CString strLog;
    strLog.Format (_T ("'WaitFor...m_hEvtOperation' для 'CUserKTSDlg::OperationIncNumberExt'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_3);*/
    
  //  switch (WaitForSingleObject (pCST->m_hEvtOperation, 0x0))
  //  {
  //      case WAIT_ABANDONED:
		//case WAIT_TIMEOUT:
		//    //По ЭТОМу ШАБЛОНу всё ещё продолжается РАБОТа
		//    MessageBox (_T ("Повторите попытку после окончания текущей операции!"), _T ("Внимание!"), MB_ICONINFORMATION);
		//	break;
		//case WAIT_OBJECT_0:
		//    ResetEvent (pCST->m_hEvtOperation);
		    
            while (i < m_arPosSelectedItem.GetSize ()) {
                //if (((int) m_arPosSelectedItem.GetAt (i) > 1) && ((int) m_arPosSelectedItem.GetAt (i) < m_ptrListItemFiles->GetItemCount ()))
                {
                    /*strFullPath = m_pTreeCtrlTemplate->GetSelectedItemFullPath ();
                    strFullPath.Replace (_T("*"), m_ptrListItemFiles->GetItemText ((int) (m_arPosSelectedItem.GetAt (i) - 1), 3));
                    
                    strFullPathNew = m_pTreeCtrlTemplate->GetSelectedItemFullPath ();
                    strFullPathNew.Replace (_T("*"), m_ptrListItemFiles->GetItemText ((int) (m_arPosSelectedItem.GetAt (i) - 2), 3));*/
                    
                    strFullPath = pCST->GetFullPathTemplate ();
					SET_FULLPATH (strFullPath, (int) (m_arPosSelectedItem.GetAt (i) - 1))
                    
                    strFullPathNew = pCST->GetFullPathTemplate ();
                    if (strNumber.length () == 3) {
						strNumber.format (_T ("%i"), strNumber.number () + 1);
						while (strNumber.length () < 3)
							strNumber.insert (0, "0");
					}
					else
						strNumber.format (_T ("%i"), strNumber.number () + 1);
                    strFullPathNew.replace ("*", strNumber.GetBuffer ());
                    
			        filefind.Close ();
                    if (filefind.FindFile (strFullPath.GetBuffer ())) {
				        filefind.Close ();
				        if (filefind.FindFile (strFullPathNew.GetBuffer ()))
					        CFile::Remove (strFullPathNew.GetBuffer ());
                                        
				        CFile::Rename (strFullPath.GetBuffer (), strFullPathNew.GetBuffer ());
			        }
					else ;

			        //ИЗМЕНЯТЬ содержимое (дата/время) ???

			        /*
			        //Все ИЗМЕННЕИЯ обрабатываются при их ОБНАРУЖЕНии в птоковой функции
                    pCST->CopyInfoTemplateItem ((int) (m_arPosSelectedItem.GetAt (i) - 1), (int) (m_arPosSelectedItem.GetAt (i) - 2));
		            pCST->SetState ((int) (m_arPosSelectedItem.GetAt (i) - 2));
		            SetListBoxItemFilesText (pCST, (int) (m_arPosSelectedItem.GetAt (i) - 2));

		            pCST->ClearDTInfoTemplate ((int) (m_arPosSelectedItem.GetAt (i) - 1));
		            pCST->SetState ((int) (m_arPosSelectedItem.GetAt (i) - 1));
		            SetListBoxItemFilesText (pCST, (int) (m_arPosSelectedItem.GetAt (i) - 1));
			        */
                }
                //else
                //    ; //Нельзя изменять ВЕРХНЮЮ строку - файл заменит самую НИЖНЮЮ
                //      //Нельзя изменять НИЖНЮЮ строку - ОСНОВА ОСНОВ
                      
                i ++;
            }
            
            /*
	        //Все ИЗМЕННЕИЯ обрабатываются при их ОБНАРУЖЕНии в птоковой функции
            i = 0;
            lvItem.mask = LVIF_STATE;
            lvItem.stateMask = LVIS_SELECTED;
            while (i < m_arPosSelectedItem.GetSize ())
            {
                if (((int) m_arPosSelectedItem.GetAt (i) > 1) && ((int) m_arPosSelectedItem.GetAt (i) < m_ptrListItemFiles->GetItemCount ()))
                {
                    lvItem.state = 0;
                    //m_ptrListItemFiles->SetItem((int) (m_arPosSelectedItem.GetAt (i) - 2), 0, LVIF_STATE, NULL, 0, 0, 0, 0);
                    m_ptrListItemFiles->SetItemState ((int) (m_arPosSelectedItem.GetAt (i) - 1), &lvItem);
                    
                    lvItem.state = LVIS_SELECTED;
                    //m_ptrListItemFiles->SetItem((int) (m_arPosSelectedItem.GetAt (i) - 2), 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
                    m_ptrListItemFiles->SetItemState ((int) (m_arPosSelectedItem.GetAt (i) - 2), &lvItem);
                }
                else
                    ;
                                            
                i ++;
            }
	        */
	        
            /*SetEvent (pCST->m_hEvtOperation);
            break;
        default:
            ;
    }*/
    
    strFullPath.free (), strFullPathNew.free (), strNumber.free ();
}

void HDlgKTSFiles::OperationDecNumberExt (HCheckStateTemplate *pCST) {
    HString strFullPath, strFullPathNew, strNumber;
    CFileFind filefind;
    //CFileStatus fStatus;
    //CFile file;
    //LVITEM lvItem;

    int i = -1;
    
    /*CString strLog;
    strLog.Format (_T ("'WaitFor...m_hEvtOperation' для 'CUserKTSDlg::OperationDecNumberExt'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_3);*/
    
  //  switch (WaitForSingleObject (pCST->m_hEvtOperation, 0x0))
  //  {
  //      case WAIT_ABANDONED:
		//case WAIT_TIMEOUT:
		//    //По ЭТОМу ШАБЛОНу всё ещё продолжается РАБОТа
		//    MessageBox (_T ("Повторите попытку после окончания текущей операции!"), _T ("Внимание!"), MB_ICONINFORMATION);
		//	break;
		//case WAIT_OBJECT_0:
		//    ResetEvent (pCST->m_hEvtOperation);
		    
		    i = m_arPosSelectedItem.GetUpperBound ();
		    
            while (i > -1) {
                //if ((int) m_arPosSelectedItem.GetAt (i) < m_ptrListItemFiles->GetItemCount () - 2)
                //if ((int) m_arPosSelectedItem.GetAt (i) < m_ptrListItemFiles->GetItemCount ())
                {
                    strFullPath = pCST->GetFullPathTemplate ();
					SET_FULLPATH (strFullPath, (int) (m_arPosSelectedItem.GetAt (i) - 1))
                    
                    strFullPathNew = pCST->GetFullPathTemplate ();
                    if (strNumber.length () == 3) {
						strNumber.format (_T ("%i"), strNumber.number () - 1);
						while (strNumber.length () < 3)
							strNumber.insert (0, "0");
					}
					else
						strNumber.format (_T ("%i"), strNumber.number () - 1);
                    strFullPathNew.replace ("*", strNumber.GetBuffer ());

                    filefind.Close ();
                    if (filefind.FindFile (strFullPath.GetBuffer ())) {
				        filefind.Close ();
				        if (filefind.FindFile (strFullPathNew.GetBuffer ()))
					        CFile::Remove (strFullPathNew.GetBuffer ());
                        	                
                        CFile::Rename (strFullPath.GetBuffer (), strFullPathNew.GetBuffer ());
			        }
					else ;

			        //ИЗМЕНЯТЬ содержимое (дата/время) ???

			        /*
			        //Все ИЗМЕННЕИЯ обрабатываются при их ОБНАРУЖЕНии в птоковой функции
                    pCST->CopyInfoTemplateItem ((int) (m_arPosSelectedItem.GetAt (i) - 1), (int) (m_arPosSelectedItem.GetAt (i)));
	                pCST->SetState ((int) (m_arPosSelectedItem.GetAt (i)));
	                SetListBoxItemFilesText (pCST, (int) (m_arPosSelectedItem.GetAt (i)));

	                pCST->ClearDTInfoTemplate ((int) (m_arPosSelectedItem.GetAt (i) - 1));
	                pCST->SetState ((int) (m_arPosSelectedItem.GetAt (i) - 1));
	                SetListBoxItemFilesText (pCST, (int) (m_arPosSelectedItem.GetAt (i) - 1));
			        */
                }
                //else
                //    ; //Нельзя изменять ПредНИЖНЮЮ строку - файл заменит самую НИЖНЮЮ
                //      //Нельзя изменять НИЖНЮЮ строку - ОСНОВА ОСНОВ
                      
                i --;
            }
            
            /*
	        //Все ИЗМЕННЕИЯ обрабатываются при их ОБНАРУЖЕНии в птоковой функции
            i = m_arPosSelectedItem.GetUpperBound ();
            lvItem.mask = LVIF_STATE;
            lvItem.stateMask = LVIS_SELECTED;
            while (i > -1)
            {
                if ((int) m_arPosSelectedItem.GetAt (i) < m_ptrListItemFiles->GetItemCount () - 2)
                {
                    lvItem.state = 0;
                    m_ptrListItemFiles->SetItemState ((int) (m_arPosSelectedItem.GetAt (i) - 1), &lvItem);
                    
                    lvItem.state = LVIS_SELECTED;
                    m_ptrListItemFiles->SetItemState ((int) (m_arPosSelectedItem.GetAt (i)), &lvItem);
                }
                else
                    ;
                                            
                i --;
            }
	        */
	        
            /*SetEvent (pCST->m_hEvtOperation);
            break;
        default:
            ;
    }*/
    
    strFullPath.free (), strFullPathNew.free (), strNumber.free ();
}

void HDlgKTSFiles::OperationDel (HCheckStateTemplate *pCST) {
    HString strFullPath, strNumber;
    int i = 0;
    
    /*CString strLog;
    strLog.Format (_T ("'WaitFor...m_hEvtOperation' для 'CUserKTSDlg::OperationDel'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_3);*/
    
  //  switch (WaitForSingleObject (pCST->m_hEvtOperation, 0x0))
  //  {
  //      case WAIT_ABANDONED:
		//case WAIT_TIMEOUT:
		//    //По ЭТОМу ШАБЛОНу всё ещё продолжается РАБОТа
		//    MessageBox (_T ("Повторите попытку после окончания текущей операции!"), _T ("Внимание!"), MB_ICONINFORMATION);
		//	break;
		//case WAIT_OBJECT_0:
		//    ResetEvent (pCST->m_hEvtOperation);
            while (i < m_arPosSelectedItem.GetSize ()) {
                if ((int) m_arPosSelectedItem.GetAt (i) < (m_ptrListItemFiles->GetItemCount () + 1))  {
					strFullPath = pCST->GetFullPathTemplate ();
					SET_FULLPATH (strFullPath, (int) (m_arPosSelectedItem.GetAt (i) - 1))
                                                
                    try { CFile::Remove (strFullPath.GetBuffer ()); }
                    catch (CFileException *err) {
                        //MessageBox (err->m_strError, _T("Ошибка!"));
						err->Delete ();
                        break;
                    }
    		        /*
			        //Все ИЗМЕННЕИЯ обрабатываются при их ОБНАРУЖЕНии в птоковой функции
	                HCheckStateTemplate *pCST = m_pTreeCtrlTemplate->GetObjectCheckStateTemplate (m_pTreeCtrlTemplate->GetSelectedItem ());
	                if (pCST) {
		                pCST->ClearDTInfoTemplate ((int) (m_arPosSelectedItem.GetAt (i) - 1));
		                pCST->SetState ((int) m_arPosSelectedItem.GetAt (i) - 1);
                        
		                SetListBoxItemFilesText (pCST, (int) (m_arPosSelectedItem.GetAt (i) - 1), false);
	                }
					else ;
			        */
                }
                else
                    ; //Нельзя изменять ВЕРХНЮЮ строку - файл заменит самую НИЖНЮЮ
                      //Нельзя изменять НИЖНЮЮ строку - ОСНОВА ОСНОВ
                
                i ++;
            }
            
            /*SetEvent (pCST->m_hEvtOperation);
            break;
        default:
            ;
    }*/
    
    strFullPath.free (); strNumber.free ();
}

void HDlgKTSFiles::OperationModifyDT (HCheckStateTemplate *pCST) {
	HString strFullPath, strNumber;

    int i = 0;
    while (i < m_arPosSelectedItem.GetSize ()) {
		strFullPath = TREECTRL_KTS->GetItemFullPath (pCST->GetHTreeItem ());
		//Замена '*'-расширения строкой-номером из 3-его столбца списуа состояния
		SET_FULLPATH (strFullPath, (int) (m_arPosSelectedItem.GetAt (i) - 1))

        MODIFY_DATETIME_FILE (pCST, strFullPath, (int) (m_arPosSelectedItem.GetAt (i) - 1))
                
        i ++;
    }
}

int HDlgKTSFiles::CopyFileToFile (HString &strFullPath, HString &strFullPathNew, CTime &tmModify) {
	int iRes = 0; //Success
	
	CFileFind filefind;
    CFileStatus fStatus;
    CFile file;
    BYTE *pContentFile = NULL;
	
	filefind.Close ();
    if (filefind.FindFile (strFullPathNew.GetBuffer ()))
        CFile::Remove (strFullPathNew.GetBuffer ());
	else ;

    filefind.Close ();
    if (filefind.FindFile (strFullPath.GetBuffer ())) {
        if (CFile::GetStatus (strFullPath.GetBuffer (), fStatus)) {
			file.Open (strFullPath.GetBuffer (), CFile::modeRead | CFile::typeBinary);
            pContentFile = DEBUG_NEW BYTE [fStatus.m_size + 1];
            file.Read (pContentFile, fStatus.m_size);
            file.Close ();
                            
            //ИЗМЕНЯТЬ содержимое (дата/время) ???
                            
            file.Open (strFullPathNew.GetBuffer (), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
            file.Write (pContentFile, fStatus.m_size);
            file.Close ();
            fStatus.m_mtime = tmModify;
            CFile::SetStatus (strFullPathNew.GetBuffer (), fStatus);

			delete pContentFile;
                            
			/*
			//Все ИЗМЕННЕИЯ обрабатываются при их ОБНАРУЖЕНии в птоковой функции
            pCST->CopyInfoSampleToTemplateItem ((int) (m_arPosSelectedItem.GetAt (i) - 1), (int) (m_arPosSelectedItem.GetAt (i) - 1));
            pCST->SetState ((int) (m_arPosSelectedItem.GetAt (i) - 1));
            SetListBoxItemFilesText (pCST, (int) (m_arPosSelectedItem.GetAt (i) - 1));
			*/
        }
        else
            ; //Нет информации о статусе файла
    }
    else
        ; //Не найден файл-источник для копирования

	return iRes;
}
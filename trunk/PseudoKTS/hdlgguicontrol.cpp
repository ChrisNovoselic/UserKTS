#include "stdafx.h"

#include <HDatabaseSQL.h>

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

//#include "pseudokts.h" //Для GetPtrThread
//#include "hthreadguifilesdb.h" //Для HThreadGUIFilesDB::INT_SOURCE_ID e.t.c.

#include "hdlgguicontrol.h"
#include "pseudokts.h" //GetPtrThread ()

IMPLEMENT_DYNCREATE (HDlgGUIControl, HDlgKTS)

EXT_DECLARE_HTHREAD_FILELOG

HDlgGUIControl::HDlgGUIControl (CWnd *pParent) : HDlgKTS (pParent),
                                                m_pTreeCtrlControl (0x0) {
}

HDlgGUIControl::~HDlgGUIControl (void) {
}

void HDlgGUIControl::DoDataExchange (CDataExchange* pDX) {
    HDlgKTS::DoDataExchange (pDX);
}

BEGIN_MESSAGE_MAP(HDlgGUIControl, HDlgKTS)
    ON_MESSAGE (WM_DESTROY, &OnDestroy)
    //ON_COMMAND (IDM_RESTORE_CONTROL, &HDlgKTS::OnCommandRestoreDlg)
    ON_BN_CLICKED (IDCANCEL, &OnBnClickedCancel)
    ON_COMMAND (IDM_EXIT, &OnBnClickedCancel)
    ON_NOTIFY (TVN_SELCHANGED, IDC_TREECTRL_CONTROL, &OnTvnSelchangedTemplate)
END_MESSAGE_MAP()

// HDlgChoiceDirectory message handlers

BOOL HDlgGUIControl::OnInitDialog () {
    int iRes = 0; //Success

    iRes = HDlgKTS::OnInitDialog ();

    SetWindowText (_T ("Эмулятор ядра БПО Энергия+ ver.5.x...: Контроль"));

    CButton *pButton = 0x0;

    CRect rcThis, rcTreeCtrl,
                rcListCtrl,
                rcCtrl,
                rcStatusBar;
    GetClientRect (&rcThis);

    CreateStatusBar (this);
    m_ptrStatusBar->GetClientRect (&rcStatusBar);

    m_pTreeCtrlControl = DEBUG_NEW CTreeCtrl ();
    rcTreeCtrl.left = OFFSET_CONTROL; rcTreeCtrl.right = 0.35 * rcThis.Width ();
    rcTreeCtrl.top = OFFSET_CONTROL; rcTreeCtrl.bottom = rcThis.bottom - rcStatusBar.Height () - OFFSET_CONTROL;
	m_pTreeCtrlControl->Create (WS_VISIBLE | WS_BORDER | WS_OVERLAPPED | WS_CHILD | WS_HSCROLL  | WS_VSCROLL | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS /*| TVS_CHECKBOXES | TVS_SINGLEEXPAND*/, rcTreeCtrl, this, IDC_TREECTRL_CONTROL);

    //iRes = InitializeTreeCtrlItems ();

    GetDlgItem (IDC_TREECTRL_CONTROL)->GetClientRect (&rcTreeCtrl);
    
    pButton = DEBUG_NEW CButton ();
    rcCtrl.left = rcThis.Width () - 70 - OFFSET_CONTROL; rcCtrl.right = rcThis.Width () - OFFSET_CONTROL;
    rcCtrl.top = rcThis.Height () - HEIGHT_CONTROL - OFFSET_CONTROL - rcStatusBar.Height (); rcCtrl.bottom = rcThis.Height () - OFFSET_CONTROL - rcStatusBar.Height ();
    iRes = pButton->Create (_T ("Закрыть"), WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | WS_TABSTOP, rcCtrl, this, IDCANCEL);
    ::SendDlgItemMessage (m_hWnd, IDCANCEL, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);

    CListCtrl *ptrListCtrl = 0x0; DWORD dwIdListCtrl = IDC_LIST_ITEM_VALUES;
    ptrListCtrl = DEBUG_NEW CListCtrl ();
    rcListCtrl.left = rcTreeCtrl.right + 2 * OFFSET_CONTROL; rcListCtrl.right = rcThis.Width () - OFFSET_CONTROL;
    rcListCtrl.top = rcTreeCtrl.top + 1 * OFFSET_CONTROL; rcListCtrl.bottom = ((rcThis.Height () - 2 * OFFSET_CONTROL - HEIGHT_CONTROL - rcStatusBar.Height ()) - rcListCtrl.top) / 2;
    ptrListCtrl->Create (WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP, rcListCtrl, this, dwIdListCtrl);
    DDX_Control (m_ptrDataExchange, dwIdListCtrl, *ptrListCtrl);
    ptrListCtrl->SetExtendedStyle (ptrListCtrl->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    //m_arPtrListCtrl [VALUES] = ptrListCtrl;
    InitializeListCtrl (IDC_LIST_ITEM_VALUES);

    ptrListCtrl = 0x0; dwIdListCtrl = IDC_LIST_ITEM_IMPULSES;
    ptrListCtrl = DEBUG_NEW CListCtrl ();
    rcListCtrl.left = rcTreeCtrl.right + 2 * OFFSET_CONTROL; rcListCtrl.right = rcThis.Width () - OFFSET_CONTROL;
    rcListCtrl.top = ((rcThis.Height () - 2 * OFFSET_CONTROL - HEIGHT_CONTROL - rcStatusBar.Height ()) - rcListCtrl.top) / 2; rcListCtrl.bottom = rcThis.Height () - 2 * OFFSET_CONTROL - HEIGHT_CONTROL - rcStatusBar.Height ();
    ptrListCtrl->Create (WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP, rcListCtrl, this, dwIdListCtrl);
    DDX_Control (m_ptrDataExchange, dwIdListCtrl, *ptrListCtrl);
    ptrListCtrl->SetExtendedStyle (ptrListCtrl->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    //m_arPtrListCtrl [IMPULSES] = ptrListCtrl;
    InitializeListCtrl (IDC_LIST_ITEM_IMPULSES);
    
    if (iRes == 0)
        return TRUE;
    else
        return FALSE;
}

afx_msg LRESULT HDlgGUIControl::OnDestroy (WPARAM wp, LPARAM lp) {
    int iCodeExit = wp;

    //GetDlgItem (IDC_TREECTRL_CONTROL)->DestroyWindow ();
    delete GetDlgItem (IDC_TREECTRL_CONTROL);

    delete GetDlgItem (IDC_LIST_ITEM_VALUES);
    delete GetDlgItem (IDC_LIST_ITEM_IMPULSES);

    HDlgKTS::OnDestroy ();

    return iCodeExit;
}

afx_msg void HDlgGUIControl::OnBnClickedCancel () {
    int iCodeExit = 0x0;
    
    ::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_USER_THREAD_GUI_CLOSE, iCodeExit, 0);
}

int HDlgGUIControl::CreateStatusBar (CWnd *pParent) {
    int iRes = 0;
    CRect rcThis;
    GetClientRect (&rcThis);

    int szSeparators = 2;
    m_ptrStatusBar = DEBUG_NEW CStatusBar ();
	m_ptrStatusBar->Create (pParent);
    m_arIdIndicators = DEBUG_NEW UINT [COUNT_INDICATORS];
    for (ID_INDICATORS id = ID_INDICATOR_TIME_PASSED; id < COUNT_INDICATORS; id ++) { m_arIdIndicators [id] = WM_USER + 967 + id; }
    m_ptrStatusBar->SetIndicators (m_arIdIndicators, COUNT_INDICATORS);

    m_ptrStatusBar->SetPaneInfo(ID_INDICATOR_TIME_QUERY, m_arIdIndicators [ID_INDICATOR_TIME_QUERY], SBPS_NORMAL, rcThis.Width () / (COUNT_INDICATORS + 0));
    //m_ptrStatusBar->SetPaneInfo(ID_INDICATOR_TIME_PASSED, m_arIdIndicators [ID_INDICATOR_TIME_PASSED], SBPS_NORMAL, (rcThis.Width () - (COUNT_INDICATORS - 2) * (rcThis.Width () / (COUNT_INDICATORS + 1)) - (COUNT_INDICATORS - 1) * (COUNT_INDICATORS - 1) * szSeparators) / 2);
    //m_ptrStatusBar->SetPaneInfo(ID_INDICATOR_TIME_PASSED, m_arIdIndicators [ID_INDICATOR_TIME_PASSED], SBPS_NORMAL, (rcThis.Width () - 1 * (rcThis.Width () / 2) - 2 * 2 * szSeparators) / 2);
    m_ptrStatusBar->SetPaneInfo(ID_INDICATOR_TIME_PASSED, m_arIdIndicators [ID_INDICATOR_TIME_PASSED], SBPS_NORMAL, rcThis.Width () / (COUNT_INDICATORS + 0));
    m_ptrStatusBar->SetPaneInfo(ID_INDICATOR_TIME_CURRENT, m_arIdIndicators [ID_INDICATOR_TIME_CURRENT], SBPS_STRETCH, 0);

    RepositionBars (AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_TIME_CURRENT);

    return iRes;
}

//Изменение состояния (прикрепленного рисунка) элемента TREE_CTRL
afx_msg void HDlgGUIControl::OnNotifyNMClick (NMHDR *, LRESULT *) {
    int iRes = 0; //Success
    BOOL bRes = false;
}

BOOL HDlgGUIControl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
    return HDlgKTS::OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
}

afx_msg void HDlgGUIControl::OnTvnSelchangedTemplate (NMHDR *, LRESULT *) {
    FILELOG_VARIABLE

    //int iUserNumberPTICControl = 9;
    HWinThread *ptrThread = 0x0;
    
    //HTREEITEM hItem = ((CTreeCtrl *) GetDlgItem (IDC_TREECTRL_CONTROL))->GetSelectedItem ();
    if (((CTreeCtrl *) GetDlgItem (IDC_TREECTRL_CONTROL))->GetNextItem (((CTreeCtrl *) GetDlgItem (IDC_TREECTRL_CONTROL))->GetSelectedItem (), TVGN_CHILD) == 0) {
        //iUserNumberPTICControl = (int) m_pTreeCtrlControl->GetItemData (hItem);

        ptrThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GETDATA);
        if (!(ptrThread == 0x0))
            //::PostThreadMessage (ptrThread->m_nThreadID, WM_USER_NUMBER_PTIC_CONTROL, iUserNumberPTICControl, 0);
            //ptrThread->PostThreadMessage (WM_USER_NUMBER_PTIC_CONTROL, iUserNumberPTICControl, 0);
            //((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GETDATA)->PostThreadMessage (WM_USER_NUMBER_PTIC_CONTROL, iUserNumberPTICControl, 0);
            //((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GETDATA)->PostThreadMessage (WM_USER_NUMBER_PTIC_CONTROL, (int) m_pTreeCtrlControl->GetItemData (hItem), 0);
            ptrThread->PostThreadMessage (WM_USER_NUMBER_PTIC_CONTROL, (int) m_pTreeCtrlControl->GetItemData (((CTreeCtrl *) GetDlgItem (IDC_TREECTRL_CONTROL))->GetSelectedItem ()), 0);
        else ;
    }
    else
        ;
}

int HDlgGUIControl::InitializeListCtrl (int id) {
    CListCtrl *ptrListCtrl = dynamic_cast <CListCtrl *> (GetDlgItem (id)); //(CListCtrl *) GetDlgItem (id);

    CRect rcClientListCtrl;

    CString strQuantity;
    switch (id) {
        case IDC_LIST_ITEM_VALUES:
            strQuantity = _T ("Значения");
            break;
        case IDC_LIST_ITEM_IMPULSES:
            strQuantity = _T ("Импульсы");
            break;
        default: ;
    }

    ptrListCtrl->GetClientRect(&rcClientListCtrl);
    ptrListCtrl->InsertColumn(0, strQuantity.GetBuffer (), LVCFMT_LEFT, (rcClientListCtrl.right - rcClientListCtrl.left) / 4, -1);
	ptrListCtrl->InsertColumn(1, _T ("SRC:PREV"), LVCFMT_LEFT, (rcClientListCtrl.right - rcClientListCtrl.left) / 5, -1);
    ptrListCtrl->InsertColumn(2, _T ("SRC:CUR"), LVCFMT_LEFT, (rcClientListCtrl.right - rcClientListCtrl.left) / 5, -1);
    ptrListCtrl->InsertColumn(3, _T ("PSEUDO"), LVCFMT_LEFT, (rcClientListCtrl.right - rcClientListCtrl.left) / 5, -1);
    ptrListCtrl->InsertColumn(4, _T ("ORIGIN"), LVCFMT_LEFT, (rcClientListCtrl.right - rcClientListCtrl.left) / 5, -1);

    BOOL bRes = false;
    ptrListCtrl->InsertItem (LVIF_TEXT | LVIF_STATE /*| LVIF_PARAM*/, 0, _T ("15 сек."), 0, LVIS_SELECTED, 0, (LPARAM) 0);
    ptrListCtrl->InsertItem (LVIF_TEXT | LVIF_STATE /*| LVIF_PARAM*/, 1, _T ("1 мин."), 0, LVIS_SELECTED, 0, (LPARAM) 0);
    ptrListCtrl->InsertItem (LVIF_TEXT | LVIF_STATE /*| LVIF_PARAM*/, 2, _T ("3 мин."), 0, LVIS_SELECTED, 0, (LPARAM) 0);
    ptrListCtrl->InsertItem (LVIF_TEXT | LVIF_STATE /*| LVIF_PARAM*/, 3, _T ("5 мин."), 0, LVIS_SELECTED, 0, (LPARAM) 0);
    ptrListCtrl->InsertItem (LVIF_TEXT | LVIF_STATE /*| LVIF_PARAM*/, 4, _T ("30 мин."), 0, LVIS_SELECTED, 0, (LPARAM) 0);
    ptrListCtrl->InsertItem (LVIF_TEXT | LVIF_STATE /*| LVIF_PARAM*/, 5, _T ("1 час"), 0, LVIS_SELECTED, 0, (LPARAM) 0);

    return 0;
}

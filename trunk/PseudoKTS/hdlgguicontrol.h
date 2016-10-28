#pragma once

#if ! defined _HDLG_GUI_CONTROL_
#define _HDLG_GUI_CONTROL_

#include <hdlgkts.h>

class HDlgGUIControl : public HDlgKTS {
    DECLARE_DYNCREATE (HDlgGUIControl)

public:
    HDlgGUIControl (CWnd * = 0x0);
    virtual ~HDlgGUIControl (void);

    virtual int CreateStatusBar (CWnd *);

    //enum TYPE_QUANTITY {VALUES, IMPULSES, COUNT_TYPE_QUANTITY};
    enum ID_INDICATORS {ID_INDICATOR_TIME_QUERY, ID_INDICATOR_TIME_PASSED, ID_INDICATOR_TIME_CURRENT, COUNT_INDICATORS};

    //Изменение состояния (прикрепленного рисунка) элемента TREE_CTRL
    virtual afx_msg void OnNotifyNMClick (NMHDR *, LRESULT *);

    virtual BOOL OnCmdMsg (UINT , int , void* , AFX_CMDHANDLERINFO* );

protected:
    virtual BOOL OnInitDialog ();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    afx_msg LRESULT OnDestroy (WPARAM , LPARAM);
    afx_msg void OnBnClickedCancel (void);

    virtual afx_msg void OnTvnSelchangedTemplate (NMHDR *, LRESULT *);

    CTreeCtrl *m_pTreeCtrlControl;
    int InitializeTreeCtrlItems (void);

    DECLARE_MESSAGE_MAP()

private:
    //CListCtrl *m_arPtrListCtrl [COUNT_TYPE_QUANTITY];
    int InitializeListCtrl (int );
};

ENUM_POSTFIXINCREMENT (HDlgGUIControl::ID_INDICATORS);

#else
#endif //_HDLG_GUI_CONTROL_
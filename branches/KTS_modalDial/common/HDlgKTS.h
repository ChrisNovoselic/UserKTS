#pragma once

#include <hdlg.h>

#if ! defined _HDLGKTS_H_
#define _HDLGKTS_H_

#include <HTreeCtrlKTS.h>

class HDlgKTS :	public HDlg {
	DECLARE_DYNAMIC (HDlgKTS)
public:
	HDlgKTS (CWnd * = 0x0);
	virtual ~HDlgKTS (void);

    virtual int CreateStatusBar (CWnd *) = 0;

    enum ID_INDICATORS {COUNT_INDICATORS};
	CStatusBar *m_ptrStatusBar;
	UINT *m_arIdIndicators;

    //Изменение состояния (прикрепленного рисунка) элемента TREE_CTRL
    virtual afx_msg void OnNotifyNMClick (NMHDR *, LRESULT *) = 0;

    virtual BOOL OnCmdMsg (UINT , int , void* , AFX_CMDHANDLERINFO* );

    HString TimeIndicators (int , int);

protected:

	virtual BOOL OnInitDialog ();
	virtual void DoDataExchange(CDataExchange* pDX); //DDX/DDV support
	BOOL PreTranslateMessage (MSG* );
    //afx_msg void OnCommandRestoreDlg ();
	void OnDestroy (void);
	afx_msg void OnAboutHelp ();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	LRESULT OnCommandToSystemTray (WPARAM , LPARAM );
	LRESULT OnCommandIconTray (WPARAM , LPARAM );

    CString makeTimeSpanNonstop (CTimeSpan );
    CString makeTimeSpanRemaining (CTimeSpan );

	DECLARE_MESSAGE_MAP()

private:
	
};

#else
#endif //_HDLGKTS_H_
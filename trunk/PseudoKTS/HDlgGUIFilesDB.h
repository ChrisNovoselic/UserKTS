#pragma once

#include <hdlgktsfiles.h>
#include "htreectrlpseudo.h"

#if ! defined IDD_DIALOG_GUI
	//#define IDD_DIALOG_GUI WM_USER + 8667
#else
#endif

class HDlgGUIFilesDB : public HDlgKTSFiles {
    DECLARE_DYNCREATE (HDlgGUIFilesDB)

public:
    HDlgGUIFilesDB (CWnd * = 0x0);
    virtual ~HDlgGUIFilesDB (void);

// Dialog Data
	//enum { IDD = IDD_DIALOG_GUI };

    virtual void OperationCreate (HCheckStateTemplate *, short = 0);
    int InitComboBoxDirWatch ();

    //Идентификатор источника данных
    int m_iIdSource;

protected:
    virtual BOOL OnInitDialog ();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    afx_msg LRESULT OnDestroy (WPARAM , LPARAM);
    afx_msg void OnBnClickedCancel (void);
    afx_msg void OnSourceDBSelectedChanged ();

	HTreeCtrlPseudo *m_pTreeCtrlTemplate;
    HTREEITEM m_hRoot, m_hClear;
	TVINSERTSTRUCT m_tvInsert;

	DECLARE_MESSAGE_MAP()
};


#pragma once

#include "TreeCtrlTemplate.h"
#include "HCheckStateTemplate.h"
#include "HDlgChoiceDirectory.h"
//#include "HThreadDescription.h" //для описания свойств потока и связи с этим потоком из вне
#include "HThreadDirectoryWatched.h"
#include "HThreadCreateTemplate.h"
#include "HThreadUndoClear.h"
#include <HDlgKTSFiles.h>

// CUserKTSDlg dialog
class CUserKTSDlg : public HDlgKTSFiles {
// Construction
public:
	CUserKTSDlg(CWnd* pParent = NULL);	// standard constructor
	void Destructor (void);
	~CUserKTSDlg ();	// standard destructor

// Dialog Data
	//enum { IDD = IDD_USERKTS_DIALOG };
	
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedCommandDirList();
    virtual afx_msg void OnTvnSelchangedTemplate (NMHDR *, LRESULT *);
    
    LRESULT OnCommandExitUndoClear (WPARAM , LPARAM );
        
    LRESULT OnCommandThreadDirWatch (WPARAM , LPARAM );
    LRESULT OnCommandEnterDirWatched (WPARAM , LPARAM );
    LRESULT OnCommandLeaveDirWatched (WPARAM , LPARAM );
	//void OnCommandRestoreDlg ();
	void OnCommandExit ();
	LRESULT OnCommandExitCreateTemplate (WPARAM , LPARAM );
	LRESULT OnCommandLiveCreateTemplate (WPARAM , LPARAM );
    LRESULT OnCommandProgressRules (WPARAM , LPARAM );
    LRESULT OnCommandEndWorkBDCreateTemplate (WPARAM , LPARAM );
    LRESULT OnUserMsg (WPARAM, LPARAM);

    virtual void OperationCreate (HCheckStateTemplate *, short = 0);
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
// Implementation
	// Generated message map functions
	virtual BOOL OnInitDialog ();
    /*afx_msg void OnKeyDown(UINT , UINT , UINT);
    afx_msg void OnSysKeyDown(UINT , UINT , UINT);*/
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
	afx_msg void OnAddTemplate ();
    afx_msg void OnChangeTemplate ();
    afx_msg void OnDelTemplate ();

	int getIndexThreadCreateTemplate (int );

    //virtual void GetClientTreeCtrl (CRect &rc) { m_pTreeCtrlTemplate->GetClientRect (rc); }
	
	DECLARE_MESSAGE_MAP()

private:
    CMenu *CreateMainMenu (void);
    int SeekRSTemplateById (int );

    int m_iIdTimerUpdate;
	
    int InitializeRS (TCHAR *);
    int ReleaseRS (TCHAR *, BOOL = true, BOOL = true);

	int MakeProgressCtrl (void); //Динамическое создание 'ProgressCtrl'
    afx_msg LRESULT OnMakeProgressCtrl (WPARAM, LPARAM);
	int RemoveProgressCtrl (int );
    
    CStatic *m_ptrStaticFullPath;

	CToolBar m_ToolBar;
	
	HTreeCtrlTemplate *m_pTreeCtrlTemplate;

	//CArray <HThreadCreateTemplate *> m_arPtrThreadCreateTemplate;
	CMapWordToPtr m_mapPtrThread;
	//CArray <HThreadDescription *> m_arDescThreadDirWatched;
	CArray <HThreadDirectoryWatched *> m_arPtrThreadDirWatched;
	CArray <CProgressCtrl *> m_arpProgressCtrl;
	
	//CMapStringToPtr m_pmapDBRS;
	CMapStringToPtr m_pmapDBRS;
	
	int CreateUIThreadsDirectoryWatched (void);
	void DestroyUIThreadsDirectoryWatched (void);
	void DestroyUIThreadsCreateTemplate (void);
	int TerminateHThreadCreateTemplate (HThreadCreateTemplate *);
#if defined _WORK_THREAD
	BOOL CreateThreadsDirectoryWatched (void);
	void DestroyThreadsDirectoryWatched (void);
#endif
	
public:
    afx_msg void OnBnClickedCancel ();
private:
};

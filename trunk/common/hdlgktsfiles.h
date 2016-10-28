#pragma once

#include <hdlgkts.h>

#if ! defined _HDLGKTS_FILES_H_
#define _HDLGKTS_FILES_H_

#include <HTreeCtrlKTS.h>

struct DESC_OPERATION {
	int AccessLevel;
	BOOL bUse;
	BOOL bRequirementTemplate;
    BOOL bRequirementMaxNumber;
	HString Name;
	int Id;
};

class HDlgKTSFiles :	public HDlgKTS {
	DECLARE_DYNAMIC (HDlgKTSFiles)
public:
	HDlgKTSFiles (CWnd * = 0x0);
	virtual ~HDlgKTSFiles (void);

    virtual int CreateStatusBar (CWnd *);
	int CreateCommonCtrl (CWnd *);

    enum ID_INDICATORS {ID_INDICATOR_TEMPLATE, ID_INDICATOR_FILE, ID_INDICATOR_ERROR, ID_INDICATOR_TIME_PASSED, ID_INDICATOR_TIME_CURRENT, COUNT_INDICATORS};

    CListCtrl *m_ptrListItemFiles;
    CButton *m_ptrCheckBoxAutoUpdate;
    CComboBox *m_ptrComboBoxOperation;

    virtual afx_msg void OnNotifyNMClick (NMHDR *, LRESULT *);
    afx_msg void OnNmItemdblclickListItemFiles (NMHDR *, LRESULT *);
    afx_msg void OnBnClickedCheckAutoUpdate ();
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    afx_msg void OnBnClickedCommandOperation ();

    virtual void OperationCreate (HCheckStateTemplate *, short = 0) = 0;
	virtual void OperationDel (HCheckStateTemplate *);
	virtual void OperationCopyPrev (HCheckStateTemplate *);
	virtual void OperationCopyNext (HCheckStateTemplate *);
	virtual void OperationIncNumberExt (HCheckStateTemplate *);
	virtual void OperationDecNumberExt (HCheckStateTemplate *);
	virtual void OperationModifyDT (HCheckStateTemplate *);

    CArray <POSITION> m_arPosSelectedItem; //Для групповых операций с файлами из 'm_ListItemFiles'
	void GetSelectedListItemFiles (void); //Заполнение выше указанного МАССИВа
	void UpdateInfoListBoxItemFiles (void);
    BOOL SetListBoxItemFilesText (HCheckStateTemplate *, int , BOOL = TRUE);

    BOOL SetEnableOperation (void);

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
    LRESULT OnChangeStaffMenu (WPARAM, LPARAM);
    LRESULT OnAutoCreate (WPARAM, LPARAM);
    virtual afx_msg void OnTvnSelchangedTemplate (NMHDR *, LRESULT *);

    CString makeTimeSpanNonstop (CTimeSpan );
    CString makeTimeSpanRemaining (CTimeSpan );

    CArray <DESC_OPERATION *> m_arDescOperation;
    int InitializeOperation (void); //Таблицу 'Operation' В ОПЕРАТИВНУЮ ПАМЯТЬ

    static CString GetStrState (int);

	int CopyFileToFile (HString &, HString &, CTime &); //для 'OperationCopyPrev', 'OperationCopyNext'

	DECLARE_MESSAGE_MAP()

private:
	
};

ENUM_POSTFIXINCREMENT (HDlgKTSFiles::ID_INDICATORS);
//inline HDlgKTSFiles::ID_INDICATORS operator++ (HDlgKTSFiles::ID_INDICATORS &id, int) { return id = (HDlgKTSFiles::ID_INDICATORS)(id + 1); }
//inline HDlgKTSFiles::ID_INDICATORS operator++ (HDlgKTSFiles::ID_INDICATORS &id, int) { HDlgKTSFiles::ID_INDICATORS prev = id; id = static_cast<HDlgKTSFiles::ID_INDICATORS>(static_cast<int>(id) + 1); return prev; }


#define MODIFY_DATETIME_FILE(pSCT, strFullPath, pos) { \
													CFileFind filefind; CFileStatus fStatus; \
													if (filefind.FindFile (strFullPath.GetBuffer ())) { \
														if (CFile::GetStatus (strFullPath.GetBuffer (), fStatus)) { \
															fStatus.m_mtime = pCST->GetDTModifyInfoSample (pos); \
															CFile::SetStatus (strFullPath.GetBuffer (), fStatus); \
														} else ; /*Нет информации о статусе файла*/ \
													} else ; /*Не найден файл-источник для копирования*/ \
													filefind.Close (); \
												}

#else
#endif //_HDLGKTS_FILES_H_
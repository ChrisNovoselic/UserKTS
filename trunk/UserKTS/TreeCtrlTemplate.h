#pragma once
#include "StdAfx.h"

#include <htreectrlkts.h>

#include "HRecordset.h"
#include "HContentTemplate.h"

// HTreeCtrlTemplate

class HTreeCtrlTemplate : public HTreeCtrlKTS {
	DECLARE_DYNAMIC(HTreeCtrlTemplate)

public:
	HTreeCtrlTemplate ();
	virtual ~HTreeCtrlTemplate ();

    virtual int InitializeItems (int , HTREEITEM = 0);
    int ReInitializeItems (HTREEITEM);

    afx_msg void OnRButtonDown(UINT , CPoint );
    //afx_msg void OnNotifyNMClick (NMHDR *pNMHDR, LRESULT *pResult); //ПеренЁс родиТЕЛю
	afx_msg void OnPopupAddTemplate();
    afx_msg void OnPopupChangeTemplate();
    afx_msg void OnPopupDelTemplate();
    
    HString GetItemPATH (HTREEITEM);
    HString GetSelectedItemPATH ();
    virtual HString GetItemFullPath (HTREEITEM);
	HString GetItemFullPath (int); //Не закончена ???
    HString GetSelectedItemFullPath ();
    int SetSelectedItemPATH (HString & );
	
	int IsValidPathItem (HString & , HString &);

    void UpdateObjectCheckStateTemplate (void);

	static int IsValidExt (HString &);
	static int IsValidExt (LPWSTR );
    
    int CreateTemplate (LPWSTR, HTREEITEM);
	HContentTemplate *CreateObjectContentTemplate (HTREEITEM);
	//HContentTemplate *CloneObjectContentTemplate (HCheckStateTemplate *);
protected:
	DECLARE_MESSAGE_MAP ()
	
private:
	int ChangeStateIsTemplate (HTREEITEM, HRecordset *, BOOL ); //Изменим состояние шаблона - ИМЕЕТ табл. 'TEMPLATE_XXXX' (XXXX = ID)

	int DeleteObjectCheckStateTemplate (int); //По 'ID'
    int AddObjectCheckStateTemplate (HTREEITEM); //По 'HTREEITEM'
};



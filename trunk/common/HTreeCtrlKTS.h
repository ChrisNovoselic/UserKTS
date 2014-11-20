#pragma once

#if ! defined _HTREECTRL_H_
#define _HTREECTRL_H_

#include "HCheckStateTemplate.h"

struct ITEM_DATA {
    int item_id;
    BOOL is_template;
    BOOL auto_create; //'auto_create' НЕ м.б. 'true', если 'is_template' == false
    int max_number;
	BOOL is_pseudo;
    LPWSTR item_path;

    ITEM_DATA (void) {
        item_id = -1;
        is_template = FALSE;
        auto_create = FALSE;
        max_number = 0;
		is_pseudo = FALSE;
        item_path = 0x0;
    };
};

class HTreeCtrlKTS : public CTreeCtrl {
	DECLARE_DYNAMIC(HTreeCtrlKTS)

public:
	HTreeCtrlKTS ();
	virtual ~HTreeCtrlKTS ();

	int InitializeRS (TCHAR *);
    void ReleaseRS (TCHAR *, BOOL = true, BOOL = true);
	
	enum ID_STATE_ITEM {STATE_ITEM_UNCHECKED = 666, STATE_ITEM_CHECKED, STATE_ITEM_RO_UNCHECKED, STATE_ITEM_RO_CHECKED, STATE_ITEM_MIXED};
	enum ITEM_LEVEL {TREECTRL_ROOT = 0x00000001, TREECTRL_CONTAINER = 0x00000002, TREECTRL_TEMPLATE = 0x00000004};

    virtual int InitializeItems (int , HTREEITEM = 0) = 0;
	int InitializeImageList (void);
	int ChangeItemImage (TVHITTESTINFO &);

    virtual int AddObjectCheckStateTemplate (HTREEITEM ) = 0;
    virtual void UpdateObjectCheckStateTemplate (void) = 0;
    
    HCheckStateTemplate *GetObjectCheckStateTemplate (HTREEITEM); //По элементу 'TreeCtrl'
    HCheckStateTemplate *GetObjectCheckStateTemplate (int); //По 'ID'
    HCheckStateTemplate *GetObjectCheckStateTemplate (HString &); //По 'Wildcard'
    HCheckStateTemplate *GetObjectCheckStateTemplate (LPWSTR); //По 'Wildcard'

    virtual HString GetItemFullPath (HTREEITEM) = 0;

    void FreeDataItems (HTREEITEM);

	int GetItemID (HTREEITEM );
	int GetSelectedItemID (void);

	int GetSelectedLevel (void);
	BOOL GetSelectedItemIsTemplate (void);

	BOOL SetEnableMenuTemplate (void);

protected:
    CPtrArray m_pArrayObjectCheckStateTemplate;

	int /*ID_STATE_ITEM*/ m_indxImageUnchecked;
	//CArray <int> m_arTreeCtrlLevel;
	CImageList *m_pImageListStateItem;

    int CreateImageList (void);
    int AddImage (DWORD);
	int SetParentImage (HTREEITEM, BOOL = true);

	CMapStringToPtr m_pmapDBRS;

	HTREEITEM FindItem (HTREEITEM, int ); //Поиск по 'ID'

	DECLARE_MESSAGE_MAP ()
	
private:
};

#define TREECTRL_KTS ((HTreeCtrlKTS *) GetDlgItem (IDC_TREECTRL_TEMPLATE))

#else
#endif //_HTREECTRL_H_
#include "stdafx.h"

#include "resource.h" //Идентификаторы 'ICON...'

#include <htreectrlkts.h>

EXT_DECLARE_HTHREAD_FILELOG

extern BOOL FreeMemoryDBObject(CObject *&);

// HTreeCtrlTemplate

IMPLEMENT_DYNAMIC(HTreeCtrlKTS, CTreeCtrl)

HTreeCtrlKTS::HTreeCtrlKTS () : m_pImageListStateItem (0x0) {
    FILELOG_VARIABLE

    HString strTmp;
    strTmp.format ("Конструктор списка ШАБЛОНов!");
    FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), TREE_CTRL_KTS);
}

HTreeCtrlKTS::~HTreeCtrlKTS () {
	FILELOG_VARIABLE

    int i = -1;
    HCheckStateTemplate *pObjectCheckStateTemplate = NULL;
    for (i = 0; i < m_pArrayObjectCheckStateTemplate.GetSize (); i ++) {
        pObjectCheckStateTemplate = (HCheckStateTemplate *) m_pArrayObjectCheckStateTemplate.GetAt (i);
        delete pObjectCheckStateTemplate;
    }
    m_pArrayObjectCheckStateTemplate.RemoveAll ();

	if (m_pImageListStateItem)
		m_pImageListStateItem->DeleteImageList ();
	else
		;

	delete m_pImageListStateItem;

	POSITION pos = m_pmapDBRS.GetStartPosition ();
	CString keyRS;
	HRecordset *ptrRS = 0x0;
	while (pos) {
		m_pmapDBRS.GetNextAssoc (pos, keyRS, (void *&) ptrRS);

		ReleaseRS (keyRS.GetBuffer ());
	}
	m_pmapDBRS.RemoveAll ();

    if (GetRootItem ())
        FreeDataItems (GetRootItem ());
    else
        ; //Нет "дерева" ШАБЛОНов

	HString strTmp;
    strTmp.format ("Деструктор списка ШАБЛОНов!");
    FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), TREE_CTRL_KTS);
}


BEGIN_MESSAGE_MAP(HTreeCtrlKTS, CTreeCtrl)
END_MESSAGE_MAP()

// HTreeCtrlTemplate message handlers

int HTreeCtrlKTS::InitializeImageList (void) {
    int iRes = 0; //Success
    
    CreateImageList ();
	    
    m_pImageListStateItem->SetBkColor (m_pImageListStateItem->GetBkColor ());
    CImageList *ptrPreviousImageList = NULL;
    //ptrPreviousImageList = SetImageList (m_pImageListStateItem, m_pImageListStateItem->GetImageCount ());
    ptrPreviousImageList = SetImageList (m_pImageListStateItem, LVSIL_NORMAL);
    
    if (MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_VIEW)
        m_indxImageUnchecked = ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666;
    else
        ;
    
    if (MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_ADMIN)
        m_indxImageUnchecked = ID_STATE_ITEM::STATE_ITEM_UNCHECKED - 666;
    else
        ;
        
    return iRes;
}

int HTreeCtrlKTS::CreateImageList (void) {
    int iRes = 0; //Success

    m_pImageListStateItem = DEBUG_NEW CImageList ();
    m_pImageListStateItem->Create (16, 16, 1, ILC_COLOR4, 4);
    
    iRes = AddImage (IDI_ICON_TREECTRL_STATE_UNCHECKED);
    iRes = AddImage (IDI_ICON_TREECTRL_STATE_CHECKED);
    iRes = AddImage (IDI_ICON_TREECTRL_STATE_RO_UNCHECKED);
    iRes = AddImage (IDI_ICON_TREECTRL_STATE_RO_CHECKED);
    iRes = AddImage (IDI_ICON_TREECTRL_STATE_MIXED);
    
    return iRes;
}

int HTreeCtrlKTS::AddImage (DWORD id) {
    int iRes = 0; //Success
    
    BOOL bRes;
    
    DWORD codeErr;
    TCHAR err_msg [255];
    HINSTANCE hInst = AfxGetInstanceHandle();
    
    HICON hIconState = 0;
    
    HBITMAP hBitmapState = 0;
    CBitmap *bmpState = NULL;

    TCHAR *resName = NULL;
        
    bRes = IS_INTRESOURCE (id);
    if (bRes) {        
        //Загрузка 'Bitmap' вариант №1
        //resName = MAKEINTRESOURCE (id);
        //hBitmapState = LoadBitmap (hInst, resName);
        //bmpState = CBitmap::FromHandle (hBitmapState);
        
        //Загрузка 'Bitmap' вариант №2
        //bmpState = new CBitmap ();
        //bRes = bmpState->LoadBitmap (id);
        //CSize szDimensionBitmap = bmpState->GetBitmapDimension ();
        //m_pImageListStateItem->Create (szDimensionBitmap.cx, szDimensionBitmap.cy, 1, ILC_COLORDDB, 3);

        //Добавление изображения в список
        //try { m_pImageListStateItem->Add (bmpState, RGB (255, 255, 255)); }
        //catch (CException *err)
        //{
        //    err->GetErrorMessage (err_msg, 255);
        //}
        //codeErr = GetLastError ();

        //resName = MAKEINTRESOURCE (id);
        //Загрузка 'icon' Вариант №1
        //hIconState = (HICON) ::LoadImage (hInst, resName, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
        
        //Загрузка 'icon' Вариант №2
        hIconState = AfxGetApp ()->LoadIcon (id);
        
        //Добавление 'icon' в список
        try { m_pImageListStateItem->Add (hIconState); }
        catch (CException *err)
        {
            err->GetErrorMessage (err_msg, 255);
        }
        codeErr = GetLastError ();
        
        //delete bmpState;
    }
    else
        iRes = -1;
    
    return iRes;
}

int HTreeCtrlKTS::ChangeItemImage (TVHITTESTINFO &ht) {
    int iRes = 0; //Success
    BOOL bRes = false;

    int nImage = -1, nSelectedImage = -1;
    printf ("ht.flags=%d", ht.flags);

    if (((ITEM_DATA *) GetItemData (ht.hItem))->is_template) {
		((ITEM_DATA *) GetItemData (ht.hItem))->auto_create = ! ((ITEM_DATA *) GetItemData (ht.hItem))->auto_create;

        //ЗаписЬ в БД
        LPCTSTR refKey;
        HRecordset *pRS = NULL;
        iRes = InitializeRS (_T (TABLENAME_TEMPLATE));
        
        if (iRes == 0) {
            bRes = GetItemImage (ht.hItem, nImage, nSelectedImage); //nImage == nSelectedImage !!!

		    //nImage == nSelectedImage !!!
            switch (nImage) {
                case HTreeCtrlKTS::ID_STATE_ITEM::STATE_ITEM_CHECKED - 666:
                    nImage = HTreeCtrlKTS::ID_STATE_ITEM::STATE_ITEM_UNCHECKED - 666;
                    break;
                case HTreeCtrlKTS::ID_STATE_ITEM::STATE_ITEM_UNCHECKED - 666:
                    nImage = HTreeCtrlKTS::ID_STATE_ITEM::STATE_ITEM_CHECKED - 666;
                    break;
                default:
                    ;
            }

		    nSelectedImage = nImage;

		    bRes = SetItemImage (ht.hItem, nImage, nSelectedImage); //nImage == nSelectedImage !!!    

            iRes = SetParentImage (GetParentItem (ht.hItem), ((ITEM_DATA *) GetItemData (ht.hItem))->auto_create);

            m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRS);
            //Своебразная проверка на 'валидность' pRS
            iRes = m_pmapDBRS.LookupKey (_T (TABLENAME_TEMPLATE), refKey);

            if (iRes == TRUE) {
                iRes = 0;

                pRS->Edit (_T ("ID"), (COleVariant) (long) GetItemID (ht.hItem));
                pRS->SetFieldValue (_T ("AUTO_CREATE"), (COleVariant)(VARIANT_BOOL) ((ITEM_DATA *) GetItemData (ht.hItem))->auto_create);

                try { pRS->Update (); }
                catch (CDBException *err) {
                    iRes = ERR_UPDATE_REC_IS_TEMPLATE;
                    MessageBox (err->m_strError, _T ("Ошибка!"));

				    err->Delete ();
                }
            }
            else
                ;

            ReleaseRS (_T (TABLENAME_TEMPLATE));

            //((CUserKTSDlg *) GetParent ())->SetEnableOperation ();
            this->GetParent ()->SendMessage (WM_CHANGE_STAFF_OPERATION);
        }
        else
            ;
    }
    else
        ;

    return iRes;
}

int HTreeCtrlKTS::SetParentImage (HTREEITEM hParent, BOOL bCreate) {
    int iRes = 0, //Success
        nImage, nSelectedImage;
    BOOL bRes = false;

    while (! hParent == 0) {
        if (bCreate)
            ((ITEM_DATA *) GetItemData (hParent))->auto_create ++;
        else
            ((ITEM_DATA *) GetItemData (hParent))->auto_create --;

        if (((ITEM_DATA *) GetItemData (hParent))->auto_create > 0) {
            bRes = GetItemImage (hParent, nImage, nSelectedImage);

            if (nImage == STATE_ITEM_MIXED - 666) //ИЛИ nSelectedImage == STATE_ITEM_MIXED - 666
                ;
            else
                bRes = SetItemImage (hParent, STATE_ITEM_MIXED - 666, STATE_ITEM_MIXED - 666);
        }
        else
            bRes = SetItemImage (hParent, STATE_ITEM_RO_UNCHECKED - 666, STATE_ITEM_RO_UNCHECKED - 666);

        hParent = GetParentItem (hParent);
    }
    
    return iRes;
}

void HTreeCtrlKTS::FreeDataItems (HTREEITEM hItem) {
    ITEM_DATA *pstID = NULL;

    if (hItem) {
        pstID = (ITEM_DATA *) GetItemData (hItem);
        if (pstID) {
#ifdef _DEBUG
            //TRACE (pstID->item_path);
            //TRACE (_T ("\n"));
#else
#endif
            delete pstID->item_path;
            delete pstID;
            pstID = NULL;
        }
        
        HTREEITEM hItemChild = GetNextItem (hItem, TVGN_CHILD);
        
        if (hItemChild) {
            while (hItemChild) {
                pstID = NULL;
                pstID = (ITEM_DATA *) GetItemData (hItemChild);
                if (pstID) {
#ifdef _DEBUG
                    //TRACE (pstID->item_path);
                    //TRACE (_T ("\n"));
#else
#endif
                    delete pstID->item_path;
                    delete pstID;
                    pstID = NULL;
                }
                SetItemData (hItemChild, NULL);
                FreeDataItems (hItemChild);
                hItemChild = GetNextItem (hItemChild, TVGN_NEXT);
            }
        }
        else
            ; // Идентификатор ШАБЛОНа == 0x0 ???
    }
    else
        ; //hItem == NULL ???
}

int HTreeCtrlKTS::GetSelectedLevel (void) {
    HTREEITEM hItem = GetParentItem (GetSelectedItem ());
    int level = HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_ROOT;

    while (! hItem == 0) {
        hItem = GetParentItem (hItem);
        level *= 2;
    }
    
    return level;
}

BOOL HTreeCtrlKTS::GetSelectedItemIsTemplate () {
    return ((ITEM_DATA *) GetItemData (GetSelectedItem ()))->is_template;
}

int HTreeCtrlKTS::GetSelectedItemID (void) {
    /*
    TVITEM tvItem;
    tvItem.hItem = GetSelectedItem ();
    tvItem.mask = TVIF_PARAM;
    
    if (GetItem (&tvItem))
        return tvItem.lParam;
 
    return -1;
    */
    
    return ((ITEM_DATA *) GetItemData (GetSelectedItem ()))->item_id;
}

int HTreeCtrlKTS::GetItemID (HTREEITEM hItem) {
    /*
    TVITEM tvItem;
    tvItem.hItem = hItem;
    tvItem.mask = TVIF_PARAM;
    
    if (GetItem (&tvItem))
        return ((ITEM_DATA *) tvItem.lParam)->item_id;
    
    return -1;
    */
    
    return ((ITEM_DATA *) GetItemData (hItem))->item_id;
}

HTREEITEM HTreeCtrlKTS::FindItem (HTREEITEM hItem, int id) {
    HTREEITEM hItemChild = 0x0, hRes = 0x0;
    
    int idFinded = ((ITEM_DATA *) GetItemData (hItem))->item_id;
    if (id == idFinded)
        return hItem;
    else {
        hItemChild = GetNextItem (hItem, TVGN_CHILD);
        
        if (hItemChild) {
            idFinded = ((ITEM_DATA *) GetItemData (hItemChild))->item_id; 
            if (id == idFinded)
                return hItemChild;
            else {
                while (hItemChild) {
                    hRes = FindItem (hItemChild, id);
                    if (hRes == NULL)
                        hItemChild = GetNextItem (hItemChild, TVGN_NEXT);
                    else
                        return hRes;
                }
                
                return NULL;
            }
        }
        else
            return NULL;
    }
}

BOOL HTreeCtrlKTS::SetEnableMenuTemplate (void) {
	BOOL bRes = true;
    CMenu* subMenu = 0x0;

    if (GetParent ()->GetMenu ())
        subMenu = GetParent ()->GetMenu ()->GetSubMenu (0);
    else
        ;

	//По состоянию пункта МЕНю 'Изменить' - определим РЕЖИМ_РАБОТЫ
	if ((subMenu) && (! (subMenu->GetMenuState (IDM_TEMPLATE_CHANGE, MF_BYCOMMAND) == (MF_DISABLED | MF_GRAYED)))) {
	    switch (GetSelectedLevel ()) {
		    case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_ROOT:
			    subMenu->EnableMenuItem(IDM_TEMPLATE_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			    subMenu->EnableMenuItem(IDM_TEMPLATE_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			    break;
		    case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_CONTAINER:
			    subMenu->EnableMenuItem(IDM_TEMPLATE_ADD, MF_BYCOMMAND | MF_ENABLED);
			    subMenu->EnableMenuItem(IDM_TEMPLATE_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			    break;
		    case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_TEMPLATE:
			    subMenu->EnableMenuItem(IDM_TEMPLATE_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			    subMenu->EnableMenuItem(IDM_TEMPLATE_DEL, MF_BYCOMMAND | MF_ENABLED);
			    break;
		    default:
			    bRes = false;
	    }
	}
	else
	    bRes = false;

	return bRes;
}

int HTreeCtrlKTS::InitializeRS (TCHAR *nameRS) {
    int iRes = 0;
    FILELOG_VARIABLE
    void *pRefRecordset = NULL;

    m_pmapDBRS.Lookup (nameRS, pRefRecordset);

    if (pRefRecordset) {
        //ERROR - ПОВТОРная попытка инициализации ???
        strLog.format ("Попытка ПОВТОРно инициализировать ТАБЛицу=%S", nameRS);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_KTS);
    }
    else {
        if (PTR_DB) {
            pRefRecordset = PTR_DB->GetRecordset (this, HString (nameRS));
            if (pRefRecordset)
                m_pmapDBRS.SetAt (nameRS, pRefRecordset);
            else
                iRes = -1;
        }
        else
            iRes = -1;
    }    

    return iRes;
}

void HTreeCtrlKTS::ReleaseRS (TCHAR *nameRS, BOOL bClose, BOOL bDelete) {
    PTR_DB->ReleaseRS (this, HRECORDSET (m_pmapDBRS [nameRS]), bClose, bDelete);
    m_pmapDBRS.RemoveKey (nameRS);
}


HCheckStateTemplate *HTreeCtrlKTS::GetObjectCheckStateTemplate (HTREEITEM hItem) {
    int i;
    HCheckStateTemplate *pCST;
    
    for (i = 0; i < m_pArrayObjectCheckStateTemplate.GetSize (); i ++) {
        pCST = (HCheckStateTemplate *) m_pArrayObjectCheckStateTemplate.GetAt (i);
        if ((pCST->GetID () == GetItemID (hItem)) && (((ITEM_DATA *) (GetItemData (GetParentItem (pCST->GetHTreeItem ()))))->item_id == GetItemID (GetParentItem (hItem))))
            return pCST;
    }
    
    return NULL;
}

HCheckStateTemplate *HTreeCtrlKTS::GetObjectCheckStateTemplate (int id) {
    int i;
    HCheckStateTemplate *pCST;
    
    for (i = 0; i < m_pArrayObjectCheckStateTemplate.GetSize (); i ++) {
        pCST = (HCheckStateTemplate *) m_pArrayObjectCheckStateTemplate.GetAt (i);
        if (pCST->GetID () == id)
            return pCST;
    }
    
    return NULL;
}

//Поиск 'HCheckStateTemplate' по 'Wildcard'
HCheckStateTemplate *HTreeCtrlKTS::GetObjectCheckStateTemplate (LPWSTR pwchWildcard) {
    int i = 0;
    HCheckStateTemplate *pCST = NULL;
    
    for (i = 0; i < m_pArrayObjectCheckStateTemplate.GetSize (); i ++) {
        pCST = (HCheckStateTemplate *) m_pArrayObjectCheckStateTemplate.GetAt (i);
        if (pCST->GetFullPathTemplate ().compareNoCase (pwchWildcard) == 0)
            break;
        else
            ;
    }
    
    if (i < m_pArrayObjectCheckStateTemplate.GetSize ())
        return pCST;
    else
        return 0x0;
}

//По 'Wildcard'
HCheckStateTemplate *HTreeCtrlKTS::GetObjectCheckStateTemplate (HString & strWildcard) {
    return GetObjectCheckStateTemplate (strWildcard.GetBuffer ());
}
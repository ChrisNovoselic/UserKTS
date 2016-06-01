#include "stdafx.h"

#include "htreectrlpseudo.h"

EXT_DECLARE_HTHREAD_FILELOG

IMPLEMENT_DYNCREATE (HTreeCtrlPseudo, HTreeCtrlKTS)

HTreeCtrlPseudo::HTreeCtrlPseudo (void) {
}

HTreeCtrlPseudo::~HTreeCtrlPseudo (void) {
}

BEGIN_MESSAGE_MAP (HTreeCtrlPseudo, HTreeCtrlKTS)
END_MESSAGE_MAP ()

int HTreeCtrlPseudo::InitializeItems (int idParent, HTREEITEM hParent) {
    FILELOG_VARIABLE
        
    HString strTmp;
    int idCurrent = 0,
        iRes = 0; //Success
    BOOL bRes = false; //Локальный НЕ УСПЕХ
    HTREEITEM hItem = 0;
    TVINSERTSTRUCT tvInsert; // = DEBUG_NEW TVINSERTSTRUCT;
    //LVITEM lvi;
    ITEM_DATA *pstID = 0x0;
    
    ASSERT (this);
    ASSERT (PTR_DB);
    
    strLog.format ("Вход в 'InitializeItems... для ID=%i'", idParent);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_PSEUDO);
    
    //Создание дерева ШАБЛОНов отчётов на основе табл. 'Template'
    HRecordset *pRSTemplate = 0x0;
        
    m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRSTemplate);
                       
    if (pRSTemplate) {
        strLog = "RSTemplate и RSTime инициализированы...'";
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_PSEUDO);
        
        if (pRSTemplate->IsOpen ()) {
            strLog.format ("RSTemplate и RSTime открыты...'");
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_PSEUDO);
            
            tvInsert.hInsertAfter = TVI_FIRST;
            tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                
	        //Переход на ПЕРВую запись
	        if (pRSTemplate->CanScroll ())
	            pRSTemplate->MoveFirst ();
	        else
	            ;
		    //Пока не достигнута КРАЙНяя запись
	        while (! pRSTemplate->IsEOF ()) {
		        if (pRSTemplate->GetFieldValue (_T ("ID_OWNER")).m_iVal == idParent) {
		            tvInsert.hParent = hParent;

				    pstID = DEBUG_NEW ITEM_DATA;
                    pRSTemplate->GetFieldValue (_T ("ID"), pstID->item_id);
				    pstID->is_template = TRUE; //pRSTemplate->GetFieldValue (_T ("IS_TEMPLATE"), pstID->is_template);
                    pRSTemplate->GetFieldValue (_T ("MAX_NUMBER_EXT"), pstID->max_number);
					pRSTemplate->GetFieldValue (_T ("IS_PSEUDO"), pstID->is_pseudo);
				    pRSTemplate->GetFieldValue (_T ("AUTO_CREATE"), pstID->auto_create);

                    HString strPath;
                    if (idParent == 0) {
                       if (MODE_APP & HWinAppKTS::MODE_LOCAL)
				            strPath.append (PTR_SETTINGS->GetOption (HString (_T ("NetSharedSource"))));
				        else
				            strPath.append (PTR_SETTINGS->GetOption (_T ("NetSharedName")));
				                
                        strPath.append (_T ("\\"));
                        pRSTemplate->GetFieldValue (_T ("ITEM_PATH"), strTmp);
                        strPath.append (strTmp);
                        strTmp.free ();
                    }
                    else {
                        //Для ВСЕх осталЬных
                        pRSTemplate->GetFieldValue (_T ("ITEM_PATH"), strPath);
                        if (strPath.compareNoCase (_T ("EMPTY")) == 0)
                            strPath.free ();
                        else
                            ;
                    }

                    if (strPath.length ()) {
				        pstID->item_path = DEBUG_NEW WCHAR [(strPath.length () + 1) * sizeof (WCHAR)];
				        memset (pstID->item_path, 0x0, (strPath.length () + 1) * sizeof (WCHAR));
				        memcpy (pstID->item_path, strPath.GetBuffer (), strPath.length () * sizeof (WCHAR));
                    }
                    else
                        ;

				    tvInsert.item.lParam = (DWORD_PTR) pstID;
                    //tvInsert.item.lParam = (DWORD_PTR) &pTmpID->item_id;
				        
                    CDBVariant dbvarData;
                    pRSTemplate->GetFieldValue (_T ("ITEM_NAME"), dbvarData);
		            tvInsert.item.pszText = dbvarData.m_pstring->GetBuffer ();

                    int iParentItem = 0; //Является ли 'Item' родителем (COUNT_PASS_ERROR_CREATE, MAX_NUMBER_EXT, ID_TIME_INTERVAL)
                    pRSTemplate->GetFieldValue (_T ("COUNT_PASS_ERROR_CREATE"), iParentItem);
                    if (iParentItem < 0) {
                        tvInsert.item.iImage = ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666;
                        tvInsert.item.iSelectedImage = ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666;
                    }
                    else {
                        if (pstID->is_template) {
							if (pstID->is_pseudo) {
								tvInsert.item.iImage = ID_STATE_ITEM::STATE_ITEM_RO_CHECKED - 666;
								tvInsert.item.iSelectedImage = ID_STATE_ITEM::STATE_ITEM_RO_CHECKED - 666;

								if (pstID->auto_create)
									; //ВСЁ ПРАВИЛьНо! При ПСЕВДО_создании 'auto_create' == TRUE
								else
									//ПСЕВДО_созданиЕ - но файл НЕ создается АВТОматически
									//Ошибка в БД ???
									pstID->auto_create = 1; //НеОБХОДИМо изменить значение ПОЛя 'auto_create' в ТАБЛицЕ ???
							}
							else {
								tvInsert.item.iImage = m_indxImageUnchecked;
								tvInsert.item.iSelectedImage = m_indxImageUnchecked;
                            
								if (pstID->auto_create) {
									tvInsert.item.iImage ++;
									tvInsert.item.iSelectedImage ++;
								}
								else
									; //ИндексЫ 'ICON' оставвим БЕЗ изменений
							}
                        }
                        else {
                            //ШАБЛОНА НЕТ - код НЕ выполняется НИКОГДа
                            tvInsert.item.iImage = ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666;
                            tvInsert.item.iSelectedImage = ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666;
                                
                            if (pstID->auto_create) {
                                //ШАБЛОНА НЕТ - но файл создается АВТОматически
								//Ошибка в БД ???
				                pstID->auto_create = 0;
				                ; //НеОБХОДИМо изменить значение ПОЛя 'auto_create' в ТАБЛицЕ ???
				            }
				            else
				                ; //ВСЁ ПРАВИЛьНо! При отсутствии ШАБЛОна авто_создание НЕВОЗМОЖНо
                        }

						if (pstID->auto_create)
							SetParentImage (hParent);
						else ;
                    }

				    //Добавление элемента дерева
				    hItem = InsertItem (&tvInsert);
				    strLog.format ("Добавление элемента дерева... для wildCard=%S", tvInsert.item.pszText);
                    //strLog.format ("Добавление элемента дерева... для wildCard=%s", "Пустой путь (EMPTY)");
                    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_PSEUDO);

				    //Запомним 'ID' для восстановления исходной (текущей) позиции
				    //2 РАЗа значЕНИЕ поля запросить НЕЛЬЗЯ
				    //pRSTemplate->GetFieldValue (_T ("ID"), idCurrent);
				    idCurrent = pstID->item_id;

			        iRes = InitializeItems (idCurrent, hItem);

                    //Исключить "БЛОКи" не имеющие дочерних записей ???
                    //т.е. не ШАБЛОН отчёта, но bRes == false ???
				    if (iRes == 0) {
                        //Это запись не имеет дочерних, т.е. это ШАБЛОН отчёта
						if (AddObjectCheckStateTemplate (hItem) == 0) {
						}
						else
							; //Не смогли ДОБАВИТЬ об'ект
				    }
                    else
                        ;
        			    
			        //Возврат на исходную позицию
			        pRSTemplate->SeekRSTemplateToItem (idCurrent);
		        }
        			
		        pRSTemplate->MoveNext ();
        			
	        }
    	        
	        if (hItem)
	            iRes = 1; //По умолчанию разве НЕ '0' ???
            else
                ;
	    }
        else {
	        MessageBox (_T("Не открыта целевая таблица!"), _T("Внимание!"), MB_ICONINFORMATION);
	        iRes = -1;
        }
	}
    else {
	    MessageBox (_T("Нет источника данных!"), _T("Внимание!"), MB_ICONINFORMATION);
        iRes = -1;
    }
	
	return iRes;
}

int HTreeCtrlPseudo::AddObjectCheckStateTemplate (HTREEITEM hItem) {
    int iRes = 0; //Success

    FILELOG_VARIABLE
	COleVariant oleData;
	//
	COleDateTime oleDataTime;
	HCheckStateTemplate *pCST = NULL;
	HString strFullPathTemplate = GetItemFullPath (hItem);
	CTimeSpan tmSpanData, tmSpanModify, tmSpanTask, tmSpanWork; //, tmSpanInterval;
	int iMinExt = 0, iMaxExt = -1, iIdInterval = -1, iCountPassErrorCreate = -1;
	
	strLog.format ("%s", "Вход в 'AddObjectCheckStateTemplate'...'");
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_PSEUDO);

    HRecordset *pRSTemplate = 0x0;
    m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRSTemplate);
	
	if ((! (pRSTemplate == 0x0)) && pRSTemplate->SeekRSTemplateToItem (GetItemID (hItem))) {
        strLog.format ("Это ШАБЛОН! - %S", WC_HSTRING (strFullPathTemplate));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_PSEUDO);

	    iIdInterval = pRSTemplate->GetFieldValue (_T ("ID_TIME_INTERVAL")).m_iVal;

        iMaxExt = pRSTemplate->GetFieldValue (_T ("MAX_NUMBER_EXT")).m_iVal;
	    
	    iCountPassErrorCreate = pRSTemplate->GetFieldValue (_T ("COUNT_PASS_ERROR_CREATE")).m_iVal;

	    pCST = DEBUG_NEW HCheckStateTemplate (GetItemID (hItem), hItem, strFullPathTemplate, 0, 0, 0, iIdInterval, 0, iMaxExt, iCountPassErrorCreate, false);

        if (m_pArrayObjectCheckStateTemplate.Add (pCST) < 0)
            //Не смогли ДОБАВИТЬ элемент в МАССИВ ???
		    iRes = -1;
		else
	        ; //bRes = true;

    }
    else
        ;

    return iRes;
}

HString HTreeCtrlPseudo::GetItemFullPath (HTREEITEM hItem) {
    HString strRes;

    strRes = HString (((ITEM_DATA *) GetItemData (hItem))->item_path);
	
	if (strRes.isUses () == 1) {	
        strRes = HString (((ITEM_DATA *) GetItemData (GetRootItem ()))->item_path) + HString (_T ("\\")) + strRes;
	}
    else
        ;

    return strRes;
}

void HTreeCtrlPseudo::UpdateObjectCheckStateTemplate (void) {
    FILELOG_VARIABLE
    int i;
    HCheckStateTemplate *pCST;
    HTREEITEM hSelectedItem = GetSelectedItem ();
	ITEM_DATA *pItemData = 0x0;
    
    //Структура для посылки сообщения
    NMHDR hdr;
    
    hdr.code = TVN_SELCHANGED;
    hdr.hwndFrom = m_hWnd;
    hdr.idFrom = GetDlgCtrlID ();
    
    /*strLog.format ("%s", "Вход 'UpdateObjectCheckStateTemplate'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2)*/
    
    for (int i = 0; i < m_pArrayObjectCheckStateTemplate.GetSize (); i ++) {
        pCST = (HCheckStateTemplate *) m_pArrayObjectCheckStateTemplate.GetAt (i);
        
        strLog.format ("Найден 'HObjectCheckStateTemplate' ID=%i", pCST->GetID ());
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_PSEUDO)
        
        //Обновлён?
        if (pCST->IsUpdate () == 0) {
            //Шаблон НЕ обновлЁн - обновляЕМ
            strLog.format ("Шаблон ID=%i требует обновления", pCST->GetID ());
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), /*HDEBUG*/TREE_CTRL_PSEUDO)

            pCST->ClearTimeUpdate ();
            
            BOOL bAutoCreate = false;
			pItemData = (ITEM_DATA *) GetItemData (pCST->GetHTreeItem ());
            //Запуск задачИ НА авто_создание файлА - после проверки на превышение числа ошибок (COUNT_PASS_ERROR_CREATE)
#if defined _RELEASE
            if ((MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_LOCAL) &&
				((pItemData->auto_create) || (pItemData->is_pseudo)))
#else
            if ((pItemData->auto_create) ||	(pItemData->is_pseudo))
#endif
            {
                if (pCST->GetCountError () > 1)
                    bAutoCreate = true;
                else {
                    if (! pCST->isAutoCreate ())
                        pCST->IncCountPassError ();
                    else
                        bAutoCreate = true;
                }

                if (bAutoCreate) {
                    GetParent()->SendMessage (WM_USER_AUTO_CREATE, (WPARAM) pCST, 0);
                }
                else
                    ;
            }
            else
                ; //((CUserKTSDlg *) GetParent ())->OperationCreate (pCST);

            //ШАБЛОН в котором произошли изменения ОТОБРАЖАЕТСЯ или
            //КОНТЕЙНЕР для ШАБЛОНа в котором произошли изменения ОТОБРАЖАЕТСЯ или
            //ОТОБРАЖАЕТСЯ КОРНЕВАой элемент дерева
            if ((hSelectedItem == pCST->GetHTreeItem ()) ||
                (hSelectedItem == GetParentItem (pCST->GetHTreeItem ())) ||
                (hSelectedItem == GetRootItem ()))
                if ((hSelectedItem == pCST->GetHTreeItem ()) && (((CButton *) GetParent ()->GetDlgItem (IDC_CHECK_AUTO_UPDATE))->GetCheck () == BST_UNCHECKED))
                    pCST->Update (); //Вообще НЕТ НЕОБХОДИМОСТИ что-то ДЕЛАТЬ
                else
                    GetParent()->SendMessage (WM_NOTIFY, (WPARAM) m_hWnd, (LPARAM) &hdr);
            else
                pCST->Update (); //Вообще НЕТ НЕОБХОДИМОСТИ что-то ДЕЛАТЬ

			pItemData = 0x0;
        }
        else {//ДА - проходим мимо
            strLog.format ("Шаблон ID=%i НЕ требует обновления", pCST->GetID ());
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_PSEUDO)
            
            pCST->ResetCountPassError ();
        }
    }

    strLog.format ("ВЫход 'UpdateObjectCheckStateTemplate'");
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_PSEUDO)
}
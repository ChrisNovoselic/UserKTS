// TreeCtrlTemplate.cpp : implementation file
//

#include "stdafx.h"
//#include "UserKTS.h"
//#include "UserKTSDlg.h" //OperationCreate, (CButton *) AutoCkeck

#include "TreeCtrlTemplate.h"

//extern CUserKTSApp theApp;
//EXT_DECLARE_HDATETIMEINTERVAL
EXT_DECLARE_HTHREAD_FILELOG

extern BOOL FreeMemoryDBObject (CObject *&);

// HTreeCtrlTemplate

IMPLEMENT_DYNAMIC(HTreeCtrlTemplate, HTreeCtrlKTS)

HTreeCtrlTemplate::HTreeCtrlTemplate () : HTreeCtrlKTS () {
}

HTreeCtrlTemplate::~HTreeCtrlTemplate () {
}

BEGIN_MESSAGE_MAP(HTreeCtrlTemplate, HTreeCtrlKTS)
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(IDM_TEMPLATE_ADD, &HTreeCtrlTemplate::OnPopupAddTemplate)
    ON_COMMAND(IDM_TEMPLATE_CHANGE, &HTreeCtrlTemplate::OnPopupChangeTemplate)
    ON_COMMAND(IDM_TEMPLATE_DEL, &HTreeCtrlTemplate::OnPopupDelTemplate)
END_MESSAGE_MAP()

// HTreeCtrlTemplate message handlers

void HTreeCtrlTemplate::OnRButtonDown(UINT nFlags, CPoint point) {
    // TODO: Add your message handler code here and/or call default
    ClientToScreen (&point);
    GetParent ()->GetMenu ()->GetSubMenu (0)->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    
    CTreeCtrl::OnRButtonDown (nFlags, point);
}

int HTreeCtrlTemplate::InitializeItems (int idParent, HTREEITEM hParent) {
    HString strTmp;
    FILELOG_VARIABLE
        
    int idCurrent = 0,
        iRes = 0; //Success
    BOOL bRes = false; //��������� �� �����
    HTREEITEM hItem = 0;
    TVINSERTSTRUCT tvInsert; // = DEBUG_NEW TVINSERTSTRUCT;
    //LVITEM lvi;
    ITEM_DATA *pstID = 0x0;    
    
    ASSERT (this);
    ASSERT (PTR_DB);
    
    strTmp.format ("���� � 'InitializeItems... ��� ID=%i'", idParent);
    FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), TREE_CTRL_TEMPLATE);
    
    //�������� ������ �������� ������� �� ������ ����. 'Template'
    if (PTR_DB) {
        HRecordset *pRSTemplate = NULL, *pRSTime = NULL;
        
        m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRSTemplate);
        m_pmapDBRS.Lookup (_T (TABLENAME_TIME), (void *&) pRSTime);
                       
        if (pRSTemplate && pRSTime) {
            strTmp = "RSTemplate � RSTime ����������������...'";
            FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), TREE_CTRL_TEMPLATE);
        
            if ((pRSTemplate->IsOpen ()) && (pRSTime->IsOpen ())) {
                strTmp.format ("RSTemplate � RSTime �������...'");
                FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), TREE_CTRL_TEMPLATE);
            
                tvInsert.hInsertAfter = TVI_FIRST;
                tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                
	            //������� �� ������ ������
	            if (pRSTemplate->CanScroll ())
	                pRSTemplate->MoveFirst ();
	            else
	                ;
		        //���� �� ���������� ������� ������
	            while (! pRSTemplate->IsEOF ()) {
		            if (pRSTemplate->GetFieldValue (_T ("ID_OWNER")).m_iVal == idParent) {
		                tvInsert.hParent = hParent;

				        pstID = DEBUG_NEW ITEM_DATA;
				        pRSTemplate->GetFieldValue (_T ("ID"), pstID->item_id);
				        pRSTemplate->GetFieldValue (_T ("IS_TEMPLATE"), pstID->is_template);
                        pRSTemplate->GetFieldValue (_T ("MAX_NUMBER_EXT"), pstID->max_number);
						pstID->is_pseudo = FALSE; //�� ��������� � ��� 'FALSE', � ����. 'TABLENAME_TEMPLATE' ������ ���� ���
				        pRSTemplate->GetFieldValue (_T ("AUTO_CREATE"), pstID->auto_create);

				        HString strPath;
				        if (idParent == 0) {
                            //������ ��� 'ROOT'
				            //if ((idParent == 0) && (((HWinAppKTS *) AfxGetApp ())->GetHostName ().compareNoCase (PTR_SETTINGS->GetOption (HString (_T ("HostNameTarget")))) == 0))
							if ((idParent == 0) && (MODE_APP & HWinAppKTS::MODE_LOCAL))
				                strPath.append (PTR_SETTINGS->GetOption (HString (_T ("NetSharedSource"))));
				            else
				                strPath.append (PTR_SETTINGS->GetOption (_T ("NetSharedName")));
				                
                            strPath.append (_T ("\\"));
                            pRSTemplate->GetFieldValue (_T ("ITEM_PATH"), strTmp);
                            strPath.append (strTmp);
                            strTmp.free ();
                        }
                        else
                            //��� ���� ���������
                            pRSTemplate->GetFieldValue (_T ("ITEM_PATH"), strPath);
                            
				        pstID->item_path = DEBUG_NEW WCHAR [(strPath.length () + 1) * sizeof (WCHAR)];
				        memset (pstID->item_path, 0x0, (strPath.length () + 1) * sizeof (WCHAR));
				        memcpy (pstID->item_path, strPath.GetBuffer (), strPath.length () * sizeof (WCHAR));

				        //SetItemData (hItem, (DWORD_PTR) pstID);
				        tvInsert.item.lParam = (DWORD_PTR) pstID;
				        
                        CDBVariant dbvarData;
                        pRSTemplate->GetFieldValue (_T ("ITEM_NAME"), dbvarData);
		                tvInsert.item.pszText = dbvarData.m_pstring->GetBuffer ();
		                //tvInsert.item.cchTextMax = strlen ((const char *) tvInsert.item.pszText);

                        int iParentItem = 0; //�������� �� 'Item' ��������� (COUNT_PASS_ERROR_CREATE, MAX_NUMBER_EXT, ID_TIME_INTERVAL)
                        pRSTemplate->GetFieldValue (_T ("COUNT_PASS_ERROR_CREATE"), iParentItem);
                        if (iParentItem < 0) {
                            tvInsert.item.iImage = ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666;
                            tvInsert.item.iSelectedImage = ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666;
                        }
                        else {
                            if (pstID->is_template) {
                                tvInsert.item.iImage = m_indxImageUnchecked;
                                tvInsert.item.iSelectedImage = m_indxImageUnchecked;
                            
                                if (pstID->auto_create) {
				                    tvInsert.item.iImage ++;
                                    tvInsert.item.iSelectedImage ++;
                                    
                                    SetParentImage (hParent);
				                }
				                else
				                    ;
                            }
                            else {
                                //������� ���
                                tvInsert.item.iImage = ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666;
                                tvInsert.item.iSelectedImage = ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666;
                                
                                if (pstID->auto_create) {
									//������� ��� - �� ���� ��������� �������������
                                    //������ � �� ???
				                    pstID->auto_create = 0;
				                    ; //���������� �������� �������� ���� 'auto_create' � ������� ???
				                }
				                else
				                    ; //�Ѩ ���������! ��� ���������� ������� ����_�������� ����������
                            }
                        }

				        //���������� �������� ������
				        hItem = InsertItem (&tvInsert);

				        strTmp.format ("���������� �������� ������... ��� wildCard=%S", pstID->item_path);
                        FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), TREE_CTRL_TEMPLATE);

				        //�������� 'ID' ��� �������������� �������� (�������) �������
				        //2 ���� �������� ���� ��������� ������
				        //pRSTemplate->GetFieldValue (_T ("ID"), idCurrent);
				        idCurrent = pstID->item_id;

			            iRes = InitializeItems (idCurrent, hItem);

                        //��������� "�����" �� ������� �������� ������� ???
                        //�.�. �� ������ ������, �� bRes == false ???
				        if (iRes == 0) {
                            //��� ������ �� ����� ��������, �.�. ��� ������ ������
						    if (AddObjectCheckStateTemplate (hItem) == 0) {	    					
							    //HObjectTemp *pOT = DEBUG_NEW HObjectTemp ();
							    //m_arObjectTemp.Add (* pOT);
							    //TRACE ("�������� ����� ��'��� � ��������: %i!\n", m_arObjectTemp.GetSize ());
						    }
						    else
							    ; //�� ������ �������� ��'���
				        }
        			    
			            //������� �� �������� �������
			            pRSTemplate->SeekRSTemplateToItem (idCurrent);
		            }
        			
		            pRSTemplate->MoveNext ();
        			
	            }
    	        
	            if (hItem)
	                //bRes = true;
	                iRes = 1; //�� ��������� ����� �� '0' ???
                else
                    ;
	        }
	        else {
	            MessageBox (_T("�� ������� ������� �������!"), _T("��������!"), MB_ICONINFORMATION);
	            iRes = -1;
            }
        }
        else {
            MessageBox (_T("��� ������� ������� �������!"), _T("��������!"), MB_ICONINFORMATION);
            iRes = -1;
        }
	}
    else {
	    MessageBox (_T("��� ��������� ������!"), _T("��������!"), MB_ICONINFORMATION);
        iRes = -1;
    }
	
	//delete ptvInsert;
	
	return iRes;
}

HString HTreeCtrlTemplate::GetSelectedItemPATH () {
    COleVariant dataOleVariant;

    //��� ��������� �������� (1-� � ���������� �������)
    //if (GetSelectedLevel () == 1)
    {
        return HString (((ITEM_DATA *) GetItemData (GetSelectedItem ()))->item_path);
    }
	/*    
	else
    {
        MessageBox (_T("������ ���� ������ ���� �� ������!"), _T("������!"), MB_ICONINFORMATION);
    }
    */
}

HString HTreeCtrlTemplate::GetSelectedItemFullPath () {
    return GetItemFullPath (GetSelectedItem ());
}

HString HTreeCtrlTemplate::GetItemPATH(HTREEITEM hItem) {
	return HString (((ITEM_DATA *) GetItemData (hItem))->item_path);
}

HString HTreeCtrlTemplate::GetItemFullPath(HTREEITEM hItem) {
	HString strResFullPath;

	strResFullPath = HString (((ITEM_DATA *) GetItemData (hItem))->item_path);
	
	if (strResFullPath.isUses () == 1) {	
	    hItem = GetParentItem (hItem);
	    while (hItem) {
	        strResFullPath = GetItemPATH (hItem) + HString (_T ("\\")) + strResFullPath;
	        hItem = GetParentItem (hItem);
	    }
	}
    else
        ;
	
	return strResFullPath;
}

//������ ���� �������� �� 'Template'
//�������� HDBSettings::GetItemFullPath
HString HTreeCtrlTemplate::GetItemFullPath (int id) //�� ��������� ???
{
	HString strResFullPath;
    int itemId;
    
    HTREEITEM hItem = FindItem (GetRootItem (), id);
    
	if (hItem) {
	    strResFullPath = HString (((ITEM_DATA *) GetItemData (hItem))->item_path);
    	
	    if (strResFullPath.isUses () == 1) {	
	        hItem = GetParentItem (hItem);
	        while (hItem) {
	            strResFullPath = GetItemPATH (hItem) + HString (_T("\\")) + strResFullPath;
	            hItem = GetParentItem (hItem);
	        }
	    }
    }
    else
        ;//�� ���Ĩ� �������������� ������� 'TreeCtrlTemplate' ???
	
	return strResFullPath;
}

int HTreeCtrlTemplate::SetSelectedItemPATH (HString & strItemPATH)
{
    int iRes = 0; //Success
    COleVariant dataOleVariant;
    HRecordset *pRS = NULL;
    
    //��� ��������� �������� (1-� � ���������� �������)
    /*
    if (GetSelectedLevel () == 1)
    {
    */
    if (PTR_DB)
    {
        m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRS);
    
        if (pRS)   
        {
            //SeekRSTemplateToItem (GetSelectedItem ());
        
            pRS->Edit (_T ("ID"), (COleVariant) (long) GetItemID (GetSelectedItem ()));
            
            if (pRS->SetFieldValue (_T ("ITEM_PATH"), strItemPATH.GetBuffer ()) > 0)
            {
                pRS->Update ();
            
                ITEM_DATA *pstID = ((ITEM_DATA *) GetItemData (GetSelectedItem ()));
                delete pstID->item_path;
                pstID->item_path = DEBUG_NEW WCHAR [(strItemPATH.length () + 1) * sizeof (WCHAR)];
		        memset (pstID->item_path, 0x0, (strItemPATH.length () + 1) * sizeof (WCHAR));
		        memcpy (pstID->item_path, strItemPATH.GetBuffer (), strItemPATH.length () * sizeof (WCHAR));
                
                //return true;
            }
            else
            {
                MessageBox (CString (_T("���������� ���������� �������� '������� ����������' ��� ")) + CString (GetItemText (GetSelectedItem ())) + CString (_T("!")), _T("������!"), MB_ICONINFORMATION);
                iRes = -1; //���������� ���������� ��������...
            }
        }
        else
            ; //�� ��������������� RS
    }
    /*
    }
    else
    {
        MessageBox (_T("������ ���� ������ ���� �� ������!"), _T("������!"), MB_ICONINFORMATION);
        return false;
    }
    */
    else
        iRes = -1; //��� ���������� � ��
        
    return iRes;
}

int HTreeCtrlTemplate::IsValidPathItem (HString &pathItem, HString &pathFull) {
    FILELOG_VARIABLE

    strLog.format (_T ("HTreeCtrlTemplate::IsValidPathItem (pathItem=%S, pathFull=%S)"), WC_HSTRING (pathItem), WC_HSTRING (pathFull));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_TEMPLATE);
    
    pathItem.upr ();
    pathFull.upr ();
    
    if (pathFull.find (0, pathItem) > -1)
    {//���� ��� ����� ���� � ������ ����� �����
        return IsValidExt (pathFull.right (pathFull.length () - pathFull.reverseFind ('.') - 1));
    }
    else
        return ERR_BAD_PATH;
}

//BOOL HTreeCtrlTemplate::ReInitializeItems (HTREEITEM hItem)
int HTreeCtrlTemplate::ReInitializeItems (HTREEITEM hItem) {
    int iRes = 0;
    HTREEITEM hItemChild = GetNextItem (hItem, TVGN_CHILD);
    
    if (hItemChild) {
        while (hItemChild && (iRes == 0)) {
            iRes = ReInitializeItems (hItemChild);
            hItemChild = GetNextItem (hItemChild, TVGN_NEXT);
        }
    }
    else {
        iRes = DeleteObjectCheckStateTemplate (GetItemID (hItem));
        if (iRes == 0) {
            if (! (AddObjectCheckStateTemplate (hItem) == 0))
                iRes = -1; //�� ������ �������� ��'���
            else
                ;
        }
        else
            ; //???
    
        //return true;
    }

    return iRes;
}

int HTreeCtrlTemplate::DeleteObjectCheckStateTemplate (int id)
{
    int i, iRes = 0;
    HCheckStateTemplate *pCST;
    
    for (i = 0; i < m_pArrayObjectCheckStateTemplate.GetSize (); i ++)
    {
        pCST = (HCheckStateTemplate *) m_pArrayObjectCheckStateTemplate.GetAt (i);
        if (pCST->GetID () == id)
            break;
    }
    
    if (i < m_pArrayObjectCheckStateTemplate.GetSize ())
    {
        m_pArrayObjectCheckStateTemplate.RemoveAt (i);
        delete pCST;
        
        //iRes = 0;
    }
    else  {
        //�� ��� ������
        iRes = -1;
    }
    
    return iRes;
}

HContentTemplate *HTreeCtrlTemplate::CreateObjectContentTemplate (HTREEITEM hItem) {
    BOOL bRes = true;
	COleVariant oleData;
	CDBVariant dbData;
	COleDateTime oleDataTime;
	HContentTemplate *pRes = NULL;
	HString strFullPathTemplate = GetItemFullPath (hItem);
	CTimeSpan tmSpanData, tmSpanModify, tmSpanTask, tmSpanWork; //, tmSpanInterval;
	int iIdInterval, iMinExt = 0, iMaxExt = -1;

    HRecordset *pRSTemplate = NULL;
    m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRSTemplate);

	if ((! (pRSTemplate == 0x0)) && HObjectTemplate::IsTemplate (strFullPathTemplate) && pRSTemplate->SeekRSTemplateToItem (GetItemID (hItem))) {
        //�������� �� ������������ �������� (pRSTemplate == NULL) ??? 'SeekRSTemplateToItem' ��� ����� �������� ��������� !!!
	    iIdInterval = pRSTemplate->GetFieldValue (_T ("ID_TIME_INTERVAL")).m_iVal; //??? ������ '.lVal', ���� '.iVal'
	    pRSTemplate->GetFieldValue (_T ("INTERVAL_DATA"), oleDataTime);
	    tmSpanData = CTimeSpan (oleDataTime.GetHour () * 60 * 60 + oleDataTime.GetMinute () * 60 + oleDataTime.GetSecond ());
	    pRSTemplate->GetFieldValue (_T ("INTERVAL_MODIFY"), oleDataTime);
	    tmSpanModify = CTimeSpan (oleDataTime.GetHour () * 60 * 60 + oleDataTime.GetMinute () * 60 + oleDataTime.GetSecond ());
	    pRSTemplate->GetFieldValue (_T ("INTERVAL_TASK"), oleDataTime);
	    tmSpanTask = CTimeSpan (oleDataTime.GetHour () * 60 * 60 + oleDataTime.GetMinute () * 60 + oleDataTime.GetSecond ());
	    pRSTemplate->GetFieldValue (_T ("INTERVAL_WORK"), oleDataTime);
	    tmSpanWork = CTimeSpan (oleDataTime.GetHour () * 60 * 60 + oleDataTime.GetMinute () * 60 + oleDataTime.GetSecond ());
	    
	    pRSTemplate->GetFieldValue (_T ("MAX_NUMBER_EXT"), dbData);
	    iMaxExt = dbData.m_iVal;
	    if ((! (iIdInterval > 0)) || (! (iMaxExt > 0)))
	    {//����������� �������� ��� ����������� ������������ ����� ���������� ��� �������
	        pRes = DEBUG_NEW HContentTemplate (GetItemID (hItem), hItem, strFullPathTemplate, tmSpanModify, tmSpanTask, tmSpanWork, iIdInterval, iMinExt, iMaxExt, false);
    	    
	        //����������� � ��������� ������ ����������
	        //����. ����� ���������� ������� (���-�� ��������� ������)
	        iMaxExt = pRes->GetCountFileFind () - 1;
	        //�������� ������������ �������
	        iIdInterval = pRes->GetIdInterval ();
	        //�������� ������������ ������ ����� ������������ 1-�� � ���� ������ �������
	        tmSpanModify = pRes->GetSpanModify ();
    	    
	        if ((! (iIdInterval > 0)) || (! (iMaxExt > 0)))
	            //��� ���������� ��� ������
	            bRes = false;
	        else
	        {	            
                pRSTemplate->Edit (_T ("ID"), (COleVariant) (long) GetItemID (GetSelectedItem ()));
    	    
                pRSTemplate->SetFieldValue (_T ("MAX_NUMBER_EXT"), (LONG) iMaxExt);
                
                //SetIdTimeSpanInterval (tmSpanInterval); //ID_TIME_INTERVAL
                pRSTemplate->SetFieldValue (_T("ID_TIME_INTERVAL"), (LONG) iIdInterval);
                
                oleDataTime.SetTime (0, 0, 0);
                pRSTemplate->SetFieldValue (_T("INTERVAL_DATA"), oleDataTime);
                oleDataTime.SetTime (tmSpanModify.GetHours (), tmSpanModify.GetMinutes (), tmSpanModify.GetSeconds ());
                pRSTemplate->SetFieldValue (_T("INTERVAL_MODIFY"), oleDataTime);
                pRSTemplate->SetFieldValue (_T("INTERVAL_TASK"), oleDataTime);
                oleDataTime.SetTime (0, 1, 6);
                pRSTemplate->SetFieldValue (_T("INTERVAL_WORK"), oleDataTime);
        	    
                try { pRSTemplate->Update (); }
                catch (CDBException *err) {
                    MessageBox (_T("������!"), _T ("���������� ���������� �� �������!\n") + err->m_strError, MB_ICONINFORMATION);
                    bRes = false;

					err->Delete ();
                }
	        }
	    }
	    else
	        pRes = DEBUG_NEW HContentTemplate (GetItemID (hItem), hItem, strFullPathTemplate, tmSpanModify, tmSpanTask, tmSpanWork, iIdInterval, iMinExt, iMaxExt, true);
	        
	    pRes->SetSpanData (tmSpanData);
    	    
    }
	else
	    //�� ������ ��������� ����������������� � RSTemplate
	    //���
	    //�� ������
		bRes = false;

	if (bRes)
	    return pRes;
	else
	    return NULL;
}

int HTreeCtrlTemplate::AddObjectCheckStateTemplate (HTREEITEM hItem) {
	int iRes = 0;
	FILELOG_VARIABLE
	COleVariant oleData;
	//
	COleDateTime oleDataTime;
	HCheckStateTemplate *pCST = NULL;
	HString strFullPathTemplate = GetItemFullPath (hItem);
	CTimeSpan tmSpanData, tmSpanModify, tmSpanTask, tmSpanWork; //, tmSpanInterval;
	int iMinExt = 0, iMaxExt = -1, iIdInterval = -1, iCountPassErrorCreate = -1;
	
	strLog.format ("%s", "���� � 'AddObjectCheckStateTemplate'...'");
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_TEMPLATE);

    HRecordset *pRSTemplate = NULL;
	m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRSTemplate);
	
	if ((! (pRSTemplate == 0x0)) && HObjectTemplate::IsTemplate (strFullPathTemplate) && pRSTemplate->SeekRSTemplateToItem (GetItemID (hItem))) {
	    strLog.format ("��� ������! - %S", WC_HSTRING (strFullPathTemplate));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_TEMPLATE);

        //�������� �� ������������ �������� (pRSTemplate == NULL) ??? 'SeekRSTemplateToItem' ��� ����� �������� ��������� !!!
	    //tmSpanInterval = GetTimeSpanInterval (pRSTemplate->GetFieldValue (_T ("ID_TIME_INTERVAL")).m_iVal);
	    iIdInterval = pRSTemplate->GetFieldValue (_T ("ID_TIME_INTERVAL")).m_iVal;
	    
	    pRSTemplate->GetFieldValue (_T ("INTERVAL_DATA"), oleDataTime);
	    //��� �������� ������ �� 'CDBVariant'
	    //tmSpanData = CTimeSpan (oleDataTime.hour * 60 * 60 + oleDataTime.minute * 60 + oleDataTime.second);
	    tmSpanData = CTimeSpan (oleDataTime.GetHour () * 60 * 60 + oleDataTime.GetMinute () * 60 + oleDataTime.GetSecond ());
	    pRSTemplate->GetFieldValue (_T ("INTERVAL_MODIFY"), oleDataTime);
	    tmSpanModify = CTimeSpan (oleDataTime.GetHour () * 60 * 60 + oleDataTime.GetMinute () * 60 + oleDataTime.GetSecond ());
	    pRSTemplate->GetFieldValue (_T ("INTERVAL_TASK"), oleDataTime);
	    tmSpanTask = CTimeSpan (oleDataTime.GetHour () * 60 * 60 + oleDataTime.GetMinute () * 60 + oleDataTime.GetSecond ());
	    pRSTemplate->GetFieldValue (_T ("INTERVAL_WORK"), oleDataTime);
	    tmSpanWork = CTimeSpan (oleDataTime.GetHour () * 60 * 60 + oleDataTime.GetMinute () * 60 + oleDataTime.GetSecond ());
	    
	    iMaxExt = pRSTemplate->GetFieldValue (_T ("MAX_NUMBER_EXT")).m_iVal;
	    
	    iCountPassErrorCreate = pRSTemplate->GetFieldValue (_T ("COUNT_PASS_ERROR_CREATE")).m_iVal;
	    
	    if ((! (iIdInterval > 0)) || (! (iMaxExt > 0))) {
	        pCST = DEBUG_NEW HCheckStateTemplate (GetItemID (hItem), hItem, strFullPathTemplate, tmSpanModify, tmSpanTask, tmSpanWork, iIdInterval, iMinExt, iMaxExt, iCountPassErrorCreate, false);
    	    
	        //����������� � ��������� ������ ����������
	        //����. ����� ���������� ������� (���-�� ��������� ������)
	        iMaxExt = pCST->GetCountFileFind () - 1;
	        //�������� ������������ �������
	        iIdInterval = pCST->GetIdInterval ();
	        //�������� ������������ ������ ����� ������������ 1-�� � ���� ������ �������
	        tmSpanModify = pCST->GetSpanModify ();
    	    
	        if ((! (iIdInterval > 0)) || (! (iMaxExt > 0)))
	            ; //��� ���������� ��� ������
	        else {	            
                //pRSTemplate->Edit (_T ("ID"), (COleVariant) (long) GetItemID (GetSelectedItem ()));
                pRSTemplate->Edit (_T ("ID"), (COleVariant) (long) pCST->GetID ());
    	    
                pRSTemplate->SetFieldValue (_T ("MAX_NUMBER_EXT"), (LONG) iMaxExt);

                //SetIdTimeSpanInterval (tmSpanInterval); //ID_TIME_INTERVAL
                pRSTemplate->SetFieldValue (_T ("ID_TIME_INTERVAL"), (LONG) iIdInterval);
                
                //��� ���������������� CDBVarint::date
                //oleDataTime.year = 1977;
	            //oleDataTime.month = 11;
	            //oleDataTime.day = 15;
                //oleDataTime.hour = 0;
                //oleDataTime.minute = 0;
                //oleDataTime.second = 0;
                //��� ���������������� COleDateTime
                oleDataTime.SetTime (0, 0, 0);
                pRSTemplate->SetFieldValue (_T("INTERVAL_DATA"), oleDataTime);
                //��� ���������������� CDBVarint::date
                //oleDataTime.hour = tmSpanModify.GetHours ();
                //oleDataTime.minute = tmSpanModify.GetMinutes ();
                //oleDataTime.second = tmSpanModify.GetSeconds ();
                //��� ���������������� COleDateTime
                oleDataTime.SetTime (tmSpanModify.GetHours (), tmSpanModify.GetMinutes (), tmSpanModify.GetSeconds ());
                pRSTemplate->SetFieldValue (_T("INTERVAL_MODIFY"), oleDataTime);
                pRSTemplate->SetFieldValue (_T("INTERVAL_TASK"), oleDataTime);
                oleDataTime.SetTime (0, 1, 6);
                pRSTemplate->SetFieldValue (_T("INTERVAL_WORK"), oleDataTime);
                //pRSTemplate->SetFieldValue (_T("CODE_PAGE"), (COleVariant) (LONG) 0);
        	    
                try { pRSTemplate->Update (); }
                catch (CDBException *err) {
                    iRes = -1;

					err->Delete ();
                }
	        }
	    }
	    else
	        pCST = DEBUG_NEW HCheckStateTemplate (GetItemID (hItem), hItem, strFullPathTemplate, tmSpanModify, tmSpanTask, tmSpanWork, iIdInterval, iMinExt, iMaxExt, iCountPassErrorCreate, true);

	    if (m_pArrayObjectCheckStateTemplate.Add (pCST) < 0)
            //�� ������ �������� ������� � ������ ???
		    iRes = -1;
		else
	        ; //bRes = true;
	}
	else
	    //�� ������ ��������� ����������������� � RSTemplate
	    //���
	    //�� ������
		iRes = -1;
		
    strLog.format ("����� �� 'AddObjectCheckStateTemplate' � �����: %i'", iRes);
    //strLog.format ("����� �� 'AddObjectCheckStateTemplate' � �����������: %i'", bRes);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_TEMPLATE);

	return iRes;
}

void HTreeCtrlTemplate::OnPopupAddTemplate () {
    // TODO: Add your command handler code here
    CFileDialog *pDlgFile = NULL;
    TVINSERTSTRUCT tvInsert;
    ITEM_DATA *pstID;
    int idParent = -1, idTemplate = -1;
    HTREEITEM hItem = 0x0;
    HString title = _T ("��������!"), msg;
        
    if (GetSelectedLevel () == HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_CONTAINER)
    {//������ ���� �� ������
        //����������� ��������� �������� ������ ��� ����������
        tvInsert.hParent = GetSelectedItem ();
        tvInsert.hInsertAfter = TVI_LAST;
        tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
        //������ ��� ������ ������� �������� ������
        try { pDlgFile = DEBUG_NEW CFileDialog (TRUE); }
        catch (CException *err) {
            TRACE ("������ �������� ���� �������  '����� ��������!'");

			err->Delete ();
        }
        if (pDlgFile) {
            HString pathItem = GetSelectedItemFullPath (), fileName;
            //������������� ������ �� ��� �������������
            InitializeRS (_T (TABLENAME_TEMPLATE));
            InitializeRS (_T (TABLENAME_TIME));
            //������������� �������� ��� ������ �������
            pDlgFile->m_pOFN->lpstrInitialDir = pathItem.GetBuffer ();
            if (pathItem.length () > 0) {
                if (pDlgFile->DoModal() == IDOK) {
                    //����� ���������� ��������� �� ���������, � ��������� ??? - �� � 'IsValidPathItem'
                    int iRes = IsValidPathItem (pathItem, HString (pDlgFile->m_pOFN->lpstrFile));
                    if (iRes == 0) {
                        fileName = (HString (pDlgFile->m_pOFN->lpstrFile)).mid ((HString (pDlgFile->m_pOFN->lpstrFile)).reverseFind ('\\') + 1, (HString (pDlgFile->m_pOFN->lpstrFile)).reverseFind ('.') - (HString (pDlgFile->m_pOFN->lpstrFile)).reverseFind ('\\') - 1);
                        fileName.upr ();
                        
                        //����� ����������� ������ ��������� ������� ���������� 
                        //� ����. 'ListTable' ���� 'INBLOCK_NAME'
                        //� ����. 'Template' ���� 'ITEM_NAME'
                        //����� �� ������ ��� ???
                        
                        idParent = GetSelectedItemID ();
                        /*
                        //����� ������ � ����������� 'ID_OWNER' � 'ITEM_NAME'('ITEM_PATH')
					    //�������� ����������� �� 'ACCESS'
                        pRSTemplate->MoveFirst ();
                        while (pRSTemplate->IsEOF ())
                        {
                            if ((pRSTemplate->GetFieldValue ("ID_OWNER") == idParent) && (pRSTemplate->GetFieldValue ("ITEM_NAME") == fileName.GetBuffer ()))
                            {
                                iRes = ERR_DOUBLE_REC_TEMPLATE;
                                break;
                            }
                            pRSTemplate->MoveNext ();
                        }
                        */
                        if (iRes == 0) {
                            HRecordset *pRSTemplate = NULL;
	                        m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRSTemplate);

                            //�������� �� ������������ �������� (pRSTemplate == NULL) ??? !!!
                            if (pRSTemplate) {
                                //���������� ������ � ����. 'Template'
                                pRSTemplate->MoveLast ();
                                pRSTemplate->AddNew ();

                                //�������� ���� 'ID_OWNER'
                                pRSTemplate->SetFieldValue (_T ("ID_OWNER"), idParent);
                                //�������� ���� 'ITEM_NAME'
                                pRSTemplate->SetFieldValue (_T ("ITEM_NAME"), fileName);
                                //�������� ���� 'ITEM_PATH'
                                pRSTemplate->SetFieldValue (_T ("ITEM_PATH"), fileName + HString (_T(".*")));
                                //�������� ���� 'IS_TEMPLATE'
                                pRSTemplate->SetFieldValue (_T ("IS_TEMPLATE"), false);
                                //�������� ���� 'AUTO_CHANGE_SOURCE'
                                BOOL bAutoChangeSource = false;
                                if (PTR_SETTINGS->IsAlternativeSource (idParent))
                                    bAutoChangeSource = true;
                                else
                                    ;
                                pRSTemplate->SetFieldValue (_T ("AUTO_CHANGE_SOURCE"), bAutoChangeSource);
                                
                                //�������� ���� - ����_�������� �����
                                pRSTemplate->SetFieldValue (_T ("AUTO_CREATE"), (COleVariant)(VARIANT_BOOL) 0);
                                //�������� ���� - ���������� ������ ��� ���������� �������� ���������� ������ ����_�������� ����� (��� ������� 'AUTO_CREATE == true')
                                pRSTemplate->SetFieldValue (_T ("COUNT_PASS_ERROR_CREATE"), (COleVariant)(LONG) 3);
                                //�������� ���� 'COUNT_FILES'
                                pRSTemplate->SetFieldValue (_T ("MAX_NUMBER_EXT"), 0);
                                //�������� ���� 'ID_TIME_INTERVAL'
                                pRSTemplate->SetFieldValue (_T ("ID_TIME_INTERVAL"), 0);
                                //�������� ���� 'INTERVAL_DATA'
                                pRSTemplate->SetFieldValue (_T ("INTERVAL_DATA"), _T ("0:0:0"));
                                //�������� ���� 'INTERVAL_MODIFY'
                                pRSTemplate->SetFieldValue (_T ("INTERVAL_MODIFY"), (COleVariant) _T ("0:0:0"));
                                //�������� ���� 'INTERVAL_TASK'
                                pRSTemplate->SetFieldValue (_T ("INTERVAL_TASK"), (COleVariant) _T ("0:0:0"));
                                //�������� ���� 'INTERVAL_WORK'
                                pRSTemplate->SetFieldValue (_T ("INTERVAL_WORK"), (COleVariant) _T ("0:0:0"));
                                //�������� ���� 'CODE_PAGE'
                                //pRSTemplate->SetFieldValue (_T("CODE_PAGE"), (COleVariant)(LONG) 0);

                                try { pRSTemplate->Update (); }
                                catch (CDBException *err) {
                                    iRes = ERR_ADD_REC_TEMPLATE;

									err->Delete ();
                                }
                                                        
                                if (iRes == 0) {                            
                                    idTemplate = pRSTemplate->GetMaxValue (_T ("ID"));
                                    iRes = pRSTemplate->SeekRSTemplateToItem (idTemplate);
                                
					                if (iRes = TRUE) {
					                    //����������� ������ ������ - �������
					                    iRes = 0;
					                    HString title, msg;
					                    title = _T ("��������!");
					                    msg.format ("������ ������� '%S' ��������?", WC_HSTRING (fileName));
					                    iRes = MessageBox (WC_HSTRING (msg), WC_HSTRING (title), MB_YESNO);
                                        if (IDYES == iRes) {
                                            iRes = 0;
                                            
                                            pstID = DEBUG_NEW ITEM_DATA;
				                            pstID->item_id = idTemplate;
            				                
				                            pRSTemplate->GetFieldValue (_T ("IS_TEMPLATE"), pstID->is_template);
				                            HString strPath;
				                            pRSTemplate->GetFieldValue (_T ("ITEM_PATH"), strPath);
				                            pstID->item_path = DEBUG_NEW WCHAR [(strPath.length () + 1) * sizeof (WCHAR)];
				                            memset (pstID->item_path, 0x0, (strPath.length () + 1) * sizeof (WCHAR));
				                            memcpy (pstID->item_path, strPath.GetBuffer (), strPath.length () * sizeof (WCHAR));
                        				    
				                            //SetItemData (hItem, (DWORD_PTR) pstID);
				                            tvInsert.item.lParam = (DWORD_PTR) pstID;
                        		            
		                                    tvInsert.item.pszText = fileName.GetBuffer ();
		                                    tvInsert.item.cchTextMax = strlen ((const char *) fileName.GetBuffer ());
                					    
						                    hItem = InsertItem (&tvInsert);
                                            
                                            iRes = CreateTemplate (pDlgFile->m_pOFN->lpstrFile, hItem);
                                            
                                            //������� ��������� ������� - ����� ����. 'TEMPLATE_XXXX' (XXXX = ID)
                                            if (iRes == 0) {
                                                iRes = ChangeStateIsTemplate (hItem, pRSTemplate, true);
                                            }
                                            else {
												//������������ ������ (�������� �������� ������, ������ � �������)
                                                if (pstID) {
                                                    if (pstID->item_path)
                                                        delete pstID->item_path;
													else
														;

                                                    delete pstID;
                                                }
                                                else
                                                    ; //��������� ����� ������ ???
                                                
                                                DeleteItem (hItem);
                                                
                                                pRSTemplate->Delete ();
                                            }

						                    //� ����. 'ListTable' ��� ������������� 19.12.2011
                                            /*
						                    if (iRes == 0)
						                    {
							                    //������������� ���������� ������ � ����. 'ListTable'
							                    //����� �������� ����. 'fileName'
							                    m_pRSListTable->AddNew ();
							                    //�������� ���� 'INBLOCK_NAME'
								                //fileName �.�. ���������� ��� ������ ��������� ???
							                    m_pRSListTable->SetFieldValue (_T("INBLOCK_NAME"), fileName.GetBuffer ());
                    	                        
							                    //�������� ���� 'LINE_HEADER_COUNT'
							                    //�� ������������ �����-������� 'fTemplate', 'strContentTeplateFile'
							                    int iLineHeaderCount = 5;
                    	                        
							                    m_pRSListTable->SetFieldValue (_T("LINE_HEADER_COUNT"), (COleVariant)(LONG) iLineHeaderCount);
                    	                        
							                    try { m_pRSListTable->Update (); }
							                    catch (CDBException *err) {
													//�� ��������� ����� � ����. 'ListTable' � ����� ���� ��������� ����.
								                    MessageBox (_T(""), _T(""), MB_ICONINFORMATION);
								                    //������� ����.
								                    PTR_DB->DeleteTableDef (fileName);
								                    iRes = ERR_ADD_REC_LISTTABLE;

													err->Delete ();
							                    }

							                    if (iRes == 0) {
								                    //��������� ������ � ����. 'ListTable' �� ����������� ������
								                    m_pRSListTable->MoveFirst ();
								                    while (! m_pRSListTable->IsEOF ()) {
									                    if ((m_pRSListTable->GetFieldValue (_T("INBLOCK_NAME")).m_pstring == fileName) && (m_pRSListTable->GetFieldValue (_T("LINE_HEADER_COUNT")).m_iVal == iLineHeaderCount))
								                            break;
                    		                        
									                    m_pRSListTable->MoveNext ();
								                    }
                    		                        
								                    //�������� ���� 'ID_TABLE' ��� ���������� ������ � ����. 'ListTable'
								                    //� �������� ����� ������� ��� 'fileName'                                                    
								                    pRSTemplate->SetFieldValue (_T("ID_TABLE"), (COleVariant) m_pRSListTable->GetFieldValue (_T("ID")));
							                    }
						                    }
						                    */
                                        }
                                        else {
                                            //�������� ���� 'ID_TABLE' ��� ���������� ������ � ����. 'ListTable'
                                            //� �������� ����� ������� ��� 'fileName' �� 19.12.2011
                                            //pRSTemplate->SetFieldValue (_T("ID_TABLE"), (COleVariant)(LONG) -1);
                                            
                                            //�������� ���� 'IS_TEMPLATE' ������� 'false' ����� 19.12.2011
                                            
                                            //������ ������� �� �������� 26.12.2011, �� ������ ���
                                            iRes = 0;
                                        }
                                    }
                                    else
                                        ; //�� ������ ����������������� ��� 'ID' ����� ����������� ������
                                }
                                else
                                    ; //������ ��� 'Upadte' ����� ������ (iRes = ERR_ADD_REC_TEMPLATE;)
                                    
                                switch (iRes) {
                                    case ERR_UPDATE_REC_IS_TEMPLATE:
                                        break;
                                    case ERR_CREATE_OBJECT_TEMPLATE:
                                        break;
                                    default:
                                        ;
                                }
        					    
					            if (iRes == 0 && (! (hItem == 0))) {	
						            if (AddObjectCheckStateTemplate (hItem) == 0) {
								        if (SelectItem (hItem) == 0) {
								            msg.format ("�� ������ �������� ������ � ������ ������� ������ ��� '%S'!", GetItemText (GetSelectedItem ()).GetBuffer ());   
									        MessageBox (WC_HSTRING (msg), WC_HSTRING (title), MB_ICONINFORMATION);
        									
									        //����������	1) ������� ������ �� ����. 'Template' ???
									        //				2) ������� ����. ��� ������� ???, ���� ���� ������� ???
									        //				3) ������� ������ �� ����. 'ListTable' ???, ���� ���� ��������� ???
								        }
        								
								        //this->GetParent ()->SendMessage (WM_USER_MESSAGE);
							        }
							        else
								        ; //�� ������ �������� ��'���
					            }
					            else
					                ; //ERROR: (iRes == 0) - Update
                            }
                            else
                                ; //ERROR: ������ ������� 'Template' == NULL
                        }
                        else
                            ; //MessageBox (������ � ����. c '' � '' ��� ����);
                    }
                    else {
                        switch (iRes) {
                            case ERR_BAD_PATH:
                                msg.format ("��������� ���� ��� ������� �� � ���������� ��� '%S'!", GetItemText (GetSelectedItem ()).GetBuffer ());
                                break;
                            case ERR_BAD_EXT:
                                msg.format ("���������� ����� ��� ������� �� ���������!");
                                break;
                            default: ;
                        }
                        
                        MessageBox (WC_HSTRING (msg), WC_HSTRING (title), MB_ICONINFORMATION);
                    }
                }
            }
            else {
                msg.format ("�� ��������������� ������� �������� ��� '%S'!", GetItemText (GetSelectedItem ()).GetBuffer ());
                MessageBox (WC_HSTRING (msg),WC_HSTRING (title), MB_ICONINFORMATION);
            }

            ReleaseRS (_T (TABLENAME_TEMPLATE));
            ReleaseRS (_T (TABLENAME_TIME));
            
            if (pDlgFile)
                delete pDlgFile;
			else
				;
            
            fileName.free ();
        }
        else
            ; //�� ��� ������ ������ (����) ��� ������ ��������
    }
    else {
        MessageBox (_T("������ ���� ������ ���� �� ������!"), _T("������!"), MB_ICONINFORMATION);
    }
}

void HTreeCtrlTemplate::OnPopupChangeTemplate () {
    // TODO: Add your command handler code here
    FILELOG_VARIABLE

    CFileDialog *pDlgFile = NULL;
    HString pathItem, fileName, strSQL,
            title = _T ("��������!"), msg;
    NMHDR hdr;
    int idTemplate = -1;
        
    switch (GetSelectedLevel ()) {
        case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_ROOT: //������� ������ 'ROOT'
        case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_CONTAINER: //������ ���� �� ������
            GetParent ()->GetDlgItem (IDC_COMMAND_DIR_LIST)->SendMessage (BM_CLICK);
            break;
        case HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_TEMPLATE: //������ ���� �� ��������
            InitializeRS (_T (TABLENAME_TEMPLATE));
            InitializeRS (_T (TABLENAME_TIME));
        
            pathItem = GetItemFullPath (GetParentItem (GetSelectedItem ()));
            
            pDlgFile = DEBUG_NEW CFileDialog (TRUE);
            
            pDlgFile->m_pOFN->lpstrInitialDir = pathItem.GetBuffer ();
            if (pathItem.length () > 0) {
                if (pDlgFile->DoModal() == IDOK) {
                    //����� ���������� ��������� �� ���������, � ��������� ??? - �� � 'IsValidPathItem'
                    int iRes = IsValidPathItem (pathItem, HString (pDlgFile->m_pOFN->lpstrFile));
                    if (iRes == 0) {
                        fileName = (HString (pDlgFile->m_pOFN->lpstrFile)).mid ((HString (pDlgFile->m_pOFN->lpstrFile)).reverseFind ('\\') + 1, (HString (pDlgFile->m_pOFN->lpstrFile)).reverseFind ('.') - (HString (pDlgFile->m_pOFN->lpstrFile)).reverseFind ('\\') - 1);
                        fileName.upr ();
                        
                        //������� ������
                        iRes = ERR_REC_NOT_FOUND;
                        
                        /*
                        pRSTemplate->MoveFirst ();
                        while (! pRSTemplate->IsEOF ())
                        {
                            if (pRSTemplate->GetFieldValue (_T("ID")).m_iVal == GetSelectedItemID ())
                            {
                                iRes = 0;
                                break;
                            }
                                
                            pRSTemplate->MoveNext ();
                        }
                        */
                        HRecordset *pRSTemplate = NULL;
                        m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRSTemplate);
                        
                        if (pRSTemplate) {
                            if (pRSTemplate->SeekRSTemplateToItem (GetSelectedItemID ()))
                                iRes = 0;
							else
								;

                            if (iRes == 0) {
                                //�������� �������� ...
                                //COleDateTime oleDataTime;
                                //pRSTemplate->GetFieldValue (_T ("INTERVAL_DATA"), oleDataTime);
                                //.SetTime (0, 0, 0);
                            
                                //��������� ������ � ����. 'Template'
                                pRSTemplate->Edit (_T ("ID"), (COleVariant) (long) GetItemID (GetSelectedItem ()));

                                //�������� ���� 'ITEM_NAME'
                                pRSTemplate->SetFieldValue (_T("ITEM_NAME"), (COleVariant ) fileName.GetBuffer ());
                                //�������� ���� 'ITEM_PATH'
                                pRSTemplate->SetFieldValue (_T("ITEM_PATH"), (COleVariant ) (fileName + HString (_T(".*"))).GetBuffer ());
                                //�������� ���� 'AUTO_CHANGE_SOURCE'
                                BOOL bAutoChangeSource = false;
                                if (PTR_SETTINGS->IsAlternativeSource (GetItemID (GetParentItem (GetSelectedItem ()))))
                                    bAutoChangeSource = true;
                                else
                                    ;
                                pRSTemplate->SetFieldValue (_T ("AUTO_CHANGE_SOURCE"), bAutoChangeSource);
                                
                                //�������� ���� - ����_�������� �����
                                pRSTemplate->SetFieldValue (_T ("AUTO_CREATE"), (COleVariant)(VARIANT_BOOL) 0);
                                //�������� ���� - ���������� ������ ��� ���������� �������� ���������� ������ ����_�������� ����� (��� ������� 'AUTO_CREATE == true')
                                pRSTemplate->SetFieldValue (_T ("COUNT_PASS_ERROR_CREATE"), (COleVariant)(LONG) 3);
                                //�������� ���� 'COUNT_FILES'
                                pRSTemplate->SetFieldValue (_T ("MAX_NUMBER_EXT"), (COleVariant)(LONG) 0);
                                //�������� ���� 'ID_TIME_INTERVAL'
                                pRSTemplate->SetFieldValue (_T ("ID_TIME_INTERVAL"), (COleVariant) (LONG) 0);
                                
                                //�������� ���� 'INTERVAL_DATA' ��������������� ???
                                //pRSTemplate->SetFieldValue (_T ("INTERVAL_DATA"), (COleVariant) _T ("0:0:0"));
                                //pRSTemplate->SetFieldValue (_T ("INTERVAL_DATA"), oleDataTime);
                                
                                //�������� ���� 'INTERVAL_MODIFY'
                                pRSTemplate->SetFieldValue (_T ("INTERVAL_MODIFY"), (COleVariant) _T ("0:0:0"));
                                //�������� ���� 'INTERVAL_TASK'
                                //pRSTemplate->SetFieldValue (_T ("INTERVAL_TASK"), (COleVariant) _T ("0:0:0"));
                                //�������� ���� 'INTERVAL_WORK'
                                //pRSTemplate->SetFieldValue (_T ("INTERVAL_WORK"), (COleVariant) _T ("0:0:0"));
                                //�������� ���� 'CODE_PAGE'
                                //pRSTemplate->SetFieldValue (_T ("CODE_PAGE"), (COleVariant)(LONG) 0);
                                
                                try { pRSTemplate->Update (); }
                                catch (CDBException *err) {
                                    MessageBox (err->m_strError, _T("������!"));
                                    iRes = ERR_EDIT_REC_TEMPLATE;

									err->Delete ();
                                }
                                
                                if (iRes == 0) {
                                    //����������������� ������ ������ - �������
                                    idTemplate = GetSelectedItemID ();
                                    if (pRSTemplate->SeekRSTemplateToItem (idTemplate)) {
                                        BOOL bIsTemplate = pRSTemplate->GetFieldValue (_T ("IS_TEMPLATE")).m_boolVal;
                                        if (bIsTemplate) {
											//�������� ����. 'TEMPLATE_XXX' (XXX = ID)
                                            HString title = _T ("��������!"), msg;
                                            msg.format ("������ ��� '%S' ��������?", WC_HSTRING (fileName));
                                            iRes = MessageBox (WC_HSTRING (msg), WC_HSTRING (title), MB_YESNO);
                                            if (IDYES == iRes) {
                                                iRes = CreateTemplate (pDlgFile->m_pOFN->lpstrFile, GetSelectedItem ());
					                                    
		                                        switch (iRes) {
		                                            case IDNO: //��� 'CreateTemplate' ��� ������� ����� '������ ������� �� ��������'
		                                                strSQL.format ("DROP TABLE %S", WC_HSTRING (HObjectTemplate::GetNameTableTemplate (idTemplate)));
                                                        //�������� ����. 'TEMPLATE_XXX' (XXX = ID)
                                                        PTR_DB->ExecuteSQL (WC_HSTRING (strSQL));
                                                        bIsTemplate = false;
		                                                //break;
		                                            case 0: //������� ��� ������� ������� �������
		                                                iRes = ChangeStateIsTemplate (GetSelectedItem (), pRSTemplate, bIsTemplate);
		                                                break;
                                                    default:
                                                        ; //��� ���� ��������� ��� ������
		                                        }
                                            }
                                            else
                                                iRes = 0; //�� �������� ���������� �������
                                        }
                                        else {
                                            iRes = CreateTemplate (pDlgFile->m_pOFN->lpstrFile, GetSelectedItem ()); // ��� ��������� �������
                                            
                                            //������� ��������� ������� - ����� ����. 'TEMPLATE_XXXX' (XXXX = ID)
                                            if (iRes == 0)
                                                iRes = ChangeStateIsTemplate (GetSelectedItem (), pRSTemplate, true);
                                            else
                                                ; //�� �������� ���������, �.�. ������ �� ��� ������ ???
                                        }
                                    }
                                    else
                                        iRes = -1; //������� ��� ����������������
                                }
                                
                                switch (iRes) {
                                    case ERR_UPDATE_REC_IS_TEMPLATE:
                                        break;
                                    case ERR_CREATE_OBJECT_TEMPLATE:
                                        break;
                                    default:
                                        ;
                                }
        					    
					            if (iRes == 0) {
						            SetItemText (GetSelectedItem (), fileName.GetBuffer ());
    						        
						            HCheckStateTemplate *pCST = GetObjectCheckStateTemplate (GetSelectedItemID ());
						            if (pCST)
		                                if (! (DeleteObjectCheckStateTemplate (GetSelectedItemID ()) == 0)) {
		                                    //������ ��� ��������
                                            FILELOG_IF_WRITE (HERROR)
                                            strLog.format ("������ ��� �������� ObjectCheckStateTemplate c ID = %i", GetSelectedItemID ());
                                            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
                                            FILELOG_ENDIF
                                        }
                                        else
                                            ;
        							
						            if (AddObjectCheckStateTemplate (GetSelectedItem ()) == 0) {
								        //������� ��������� ������ � �������
									    //this->GetParent ()->SendMessage (WM_START_CHECK_ITEM);
    	                                
									    hdr.code = TVN_SELCHANGED;
									    hdr.hwndFrom = m_hWnd;
									    hdr.idFrom = GetDlgCtrlID ();
    	                                
									    this->GetParent ()->SendMessage(WM_NOTIFY, (WPARAM) m_hWnd, (LPARAM) &hdr);
								    }
									else
										;
					            }
                            }
                            else
                                ; //MessageBox (������ � ����. c '' � '' ��� ����);
                        }
                        else
                            ; //PTR_DB == NULL ???
                    }
                    else {
                        switch (iRes) {
                            case ERR_BAD_PATH:
                                msg.format ("��������� ���� ��� ������� �� � ���������� ��� '%S'!", GetItemText (GetSelectedItem ()).GetBuffer ());
                                break;
                            case ERR_BAD_EXT:
                                msg.format ("���������� ����� ��� ������� �� ���������!");
                                break;
                            default: ;
                        }
                        
                        MessageBox (WC_HSTRING (msg), WC_HSTRING (title), MB_ICONINFORMATION);
                    }
                }
            }
            else {
                msg.format ("�� ��������������� ������� �������� ��� '%S'!", GetItemText (GetSelectedItem ()));
                MessageBox (WC_HSTRING (msg), WC_HSTRING (title), MB_ICONINFORMATION);
            }
            
            ReleaseRS (_T (TABLENAME_TEMPLATE));
            ReleaseRS (_T (TABLENAME_TIME));
            
            fileName.free ();
            delete pDlgFile;
            
            break;
        default:
            ;
    }
}

void HTreeCtrlTemplate::OnPopupDelTemplate (void) {
    // TODO: Add your command handler code here
    FILELOG_VARIABLE

    HString strSQL;
    int iddb;
    
    if (GetSelectedLevel () == HTreeCtrlKTS::ITEM_LEVEL::TREECTRL_TEMPLATE) {
        //������ ���� �� ��������
        //� ����. 'ListTable' ��� ������������� 19.12.2011
        /*
        m_pRSListTable->MoveFirst ();
        while (! m_pRSListTable->IsEOF ())
        {
            if (m_pRSListTable->GetFieldValue (_T("ID")).m_iVal == GetSelectedItemID_TABLE ())
            {
                PTR_DB->DeleteTableDef (m_pRSListTable->GetFieldValue (_T("INBLOCK_NAME")).m_pstring);
                m_pRSListTable->Delete ();
                break;
            }
            
            m_pRSListTable->MoveNext ();
        }
        */
        
        InitializeRS (_T (TABLENAME_TEMPLATE));
        
        HRecordset *pRSTemplate = NULL;
        m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRSTemplate);
        /*if (pRSTemplate->CanScroll ())
            pRSTemplate->MoveFirst ();
        else
            ;*/
        while (! pRSTemplate->IsEOF ()) {
            pRSTemplate->GetFieldValue (_T("ID"), iddb);
            if (iddb == GetSelectedItemID ()) {
                strSQL.format ("DROP TABLE %S", HObjectTemplate::GetNameTableTemplate (iddb).GetBuffer ());
                if (pRSTemplate->GetFieldValue (_T("IS_TEMPLATE")).m_boolVal)
                    //�������� ����. 'TEMPLATE_XXX' (XXX = ID)
                    PTR_DB->ExecuteSQL (strSQL.GetBuffer ());
                
                HCheckStateTemplate *pCST = GetObjectCheckStateTemplate (GetSelectedItemID ());
		        if (pCST)
		            if (! (DeleteObjectCheckStateTemplate (GetSelectedItemID ()) == 0)) {
		                //������ ��� ��������
                        FILELOG_IF_WRITE (HERROR)
                        strLog.format ("������ ��� �������� ObjectCheckStateTemplate c ID = %i", GetSelectedItemID ());
                        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
                        FILELOG_ENDIF
                    }
                    else
                        ;
		                                
                pRSTemplate->Delete ();
                HTREEITEM hParentItem = GetParentItem (GetSelectedItem ());
                DeleteItem (GetSelectedItem ());
                SelectItem (hParentItem);
                break;
            }
            
            pRSTemplate->MoveNext ();
        }
        
        ReleaseRS (_T (TABLENAME_TEMPLATE));
    }
    else {
        MessageBox (_T("������ ���� ������ ���� �� ��������!"), _T("������!"), MB_ICONINFORMATION);
    }
}

//������� �������� (� ��������) ��������� ������������ ������� �� 'ID' ���������
//long HTreeCtrlTemplate::GetTimeSpanInterval (int id)
//{
//    LONG lRes = -1;
//    HString strLog;
//    TCHAR nameRS [] = _T (TABLENAME_TIME);
//
//    HRecordset *pRSTime = NULL;
//    m_pmapDBRS.Lookup (nameRS, (void *&) pRSTime);
//
//    if (pRSTime && (id > 0))
//    {
//        pRSTime->MoveFirst ();
//
//        while (! pRSTime->IsEOF ())
//        {
//            if (pRSTime->GetFieldValue (_T("ID")).m_iVal == id)
//                lRes = pRSTime->GetFieldValue (_T("INTERVAL_SECONDS")).m_lVal;
//
//            pRSTime->MoveNext ();
//        }
//    }
//    else
//        ;
//
//    if (lRes < 0)
//    {
//        if (id > 0)
//            strLog.format ("�� ���������������� �������=%s"), nameRS);
//        else
//            strLog.format ("��� �������������� ��������� � �������=%i"), id);
//
//        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2);
//    }
//    else
//        ;
//
//    return lRes;
//}

//void HTreeCtrlTemplate::SetIdTimeSpanInterval (CTimeSpan tmSpan)
//{
//    HString strLog;
//    HRecordset *pRSTemplate = NULL, *pRSTime = NULL;
//
//    m_pmapDBRS.Lookup (_T (TABLENAME_TEMPLATE), (void *&) pRSTemplate);
//    m_pmapDBRS.Lookup (_T (TABLENAME_TIME), (void *&) pRSTime);
//
//    if (pRSTemplate && pRSTime)
//    {
//        if (pRSTemplate->CanUpdate ())
//        {
//            pRSTemplate->SetFieldValue (_T("ID_TIME_INTERVAL"), (COleVariant) (LONG) -1);
//
//            pRSTime->MoveFirst ();
//            while (! pRSTime->IsEOF ())
//            {
//                if (pRSTime->GetFieldValue (_T("INTERVAL_SECONDS")).m_lVal == tmSpan.GetTotalSeconds ())
//                {
//	                pRSTemplate->SetFieldValue (_T("ID_TIME_INTERVAL"), (LONG) pRSTime->GetFieldValue (_T("ID")).m_iVal);
//	                break;
//                }
//
//                pRSTime->MoveNext ();
//            }
//        }
//        else
//            ; //��� ����������� �������� ������� ������ � ������� 'Template'
//    }
//    else
//    {
//        strLog.format ("�� ���������������� ���� �� ������ ('Template' ��� 'Time')"));
//        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2);
//    }
//}

int HTreeCtrlTemplate::CreateTemplate (LPWSTR lpstrFile, HTREEITEM hItem) {
    int iRes = -1;
    HContentTemplate *pobjTemplate = NULL;
    HString msg = HString (_T ("������ ������� '")) + (HString (lpstrFile)).mid ((HString (lpstrFile)).reverseFind ('\\') + 1, (HString (lpstrFile)).reverseFind ('.') - (HString (lpstrFile)).reverseFind ('\\') - 1) + HString (_T ("' ��������?"));
    iRes = MessageBox (msg.GetBuffer (), _T ("��������!"), MB_YESNO);
    if (IDYES == iRes)
    {
        iRes = 0;

        //����� ������������ ������������ ???
        pobjTemplate = CreateObjectContentTemplate (hItem);
        
        if (pobjTemplate)
        {
            iRes = pobjTemplate->CreateTable ();
            if (iRes)
            {
                //ERR_CREATE_TABLE
                msg = HString (_T("�� ������ �������� ������� ������� ������ ��� '")) + GetItemText (hItem) + HString (_T("'!"));
                MessageBoxW (msg.GetBuffer (), _T("��������!"), MB_ICONINFORMATION);
                //pRSTemplate->Delete ();
            }
            else
            {
                //������ �� ���������� ����� (���� ��� ��� ����������/���������)
                iRes = pobjTemplate->ReadContentTemplate (HString (lpstrFile));
                
                //������ �� ���������� ����� (���� ��� ��� ����������/���������)
                iRes = pobjTemplate->ParseTemplate (PTR_SETTINGS->GetSourceField (GetItemID (GetParentItem (hItem)), HString (_T ("TYPE"))));
                
                //���������� ��������� ������� ������� �� ���������� �����
                iRes = pobjTemplate->SetContentTemplate ();
                //iRes = objTemplate.ParseTemplate (HString (pDlgFile->m_pOFN->lpstrFile));
                
                if (iRes)
                    //ERR_SET_CONTENT_TEMPLATE
                    ;
                else
                    ;
            }
            
            delete pobjTemplate;
        }
        else
            iRes = ERR_CREATE_OBJECT_TEMPLATE;
    }
    else
        ; //��� ������� ������ �������� �� ��������
        
    return iRes;
}

//������� ��������� ������� - ����� ����. 'TEMPLATE_XXXX' (XXXX = ID)
int HTreeCtrlTemplate::ChangeStateIsTemplate (HTREEITEM hItem, HRecordset *pRS, BOOL val)
{
    int iRes = 0;
    LPCTSTR refKey;
    
    //����������� �������� �� '����������' pRS
    iRes = m_pmapDBRS.LookupKey (_T (TABLENAME_TEMPLATE), refKey);
    
    if (iRes == TRUE) {
        iRes = 0;
    
        pRS->Edit (_T ("ID"), (COleVariant) (long) GetItemID (hItem));
        pRS->SetFieldValue (_T("IS_TEMPLATE"), (COleVariant)(VARIANT_BOOL) val);
        
        try { pRS->Update (); }
        catch (CDBException *err) {
            iRes = ERR_UPDATE_REC_IS_TEMPLATE;
            MessageBox (err->m_strError, _T("������!"));

			err->Delete ();
        }
        
        if (iRes == 0) {
            ((ITEM_DATA *) GetItemData (hItem))->is_template = val;
            
            if (val == 1)
                iRes = SetItemImage (hItem, ID_STATE_ITEM::STATE_ITEM_UNCHECKED - 666, ID_STATE_ITEM::STATE_ITEM_UNCHECKED - 666);
            else {
                iRes = SetItemImage (hItem, ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666, ID_STATE_ITEM::STATE_ITEM_RO_UNCHECKED - 666);
                //�� ������ ������
                ((ITEM_DATA *) GetItemData (hItem))->auto_create = 0;
            }
        }
    }
    else
        iRes = -1;
        
    return iRes;
}

int HTreeCtrlTemplate::IsValidExt (HString & strExt) {
	return IsValidExt (WC_HSTRING (strExt));
}

int HTreeCtrlTemplate::IsValidExt (LPWSTR strExt) {
	int lenExt = 3;
	if (wcslen (strExt) == lenExt)
    {//���������� ������� �� 3-� �������� (4 ������ � ������)
        //if ((strExt.GetAt (lenExt - 4) == '.') &&
		if ((strExt [lenExt - 3] > 47 && strExt [lenExt - 3] < 58) &&
            (strExt [lenExt - 2] > 47 && strExt [lenExt - 2] < 58) &&
            (strExt [lenExt - 1] > 47 && strExt [lenExt - 1] < 58))
        {//������ �� 3-� �������� �����
            return 0;
        }
        else
            return ERR_BAD_EXT;
    }
    else
        return ERR_BAD_EXT;
}

void HTreeCtrlTemplate::UpdateObjectCheckStateTemplate (void) {
    FILELOG_VARIABLE
    int i;
    
    HCheckStateTemplate *pCST;
    HTREEITEM hSelectedItem = GetSelectedItem ();
    
    //��������� ��� ������� ���������
    NMHDR hdr;
    hdr.code = TVN_SELCHANGED;
    hdr.hwndFrom = m_hWnd;
    hdr.idFrom = GetDlgCtrlID ();
    
    /*strLog.format ("%s", "���� 'UpdateObjectCheckStateTemplate'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2)*/
    
    for (int i = 0; i < m_pArrayObjectCheckStateTemplate.GetSize (); i ++) {
        pCST = (HCheckStateTemplate *) m_pArrayObjectCheckStateTemplate.GetAt (i);
        
        strLog.format ("������ 'HObjectCheckStateTemplate' ID=%i", pCST->GetID ());
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_TEMPLATE)
        
        //�������?
        if (pCST->IsUpdate () == 0) {
            //������ �� ������� - ���������
            strLog.format ("������ ID=%i ������� ����������", pCST->GetID ());
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_TEMPLATE)
            
            pCST->ClearTimeUpdate ();
            BOOL bAutoCreate = false;

            //������ ������ �� ����_�������� ����� - ����� �������� �� ���������� ����� ������ (COUNT_PASS_ERROR_CREATE)
#if defined _RELEASE
            if ((MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_LOCAL) && (((ITEM_DATA *) GetItemData (pCST->GetHTreeItem ()))->auto_create))
#else
                if (((ITEM_DATA *) GetItemData (pCST->GetHTreeItem ()))->auto_create)
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
                        //ResetEvent (pCST->m_hEvtOperation);
                        //((CUserKTSDlg *) GetParent ())->OperationCreate (pCST);
                        
                        GetParent()->SendMessage (WM_USER_AUTO_CREATE, (WPARAM) pCST, 0);
                    }
                    else
                        ;
                }
                else
                    ;
                    
                //((CUserKTSDlg *) GetParent ())->OperationCreate (pCST);
                
                //������ � ������� ��������� ��������� ������������ ���
                //��������� ��� ������� � ������� ��������� ��������� ������������ ���
                //������������ ��������� ������� ������
                if ((hSelectedItem == pCST->GetHTreeItem ()) ||
                    (hSelectedItem == GetParentItem (pCST->GetHTreeItem ())) ||
                    (hSelectedItem == GetRootItem ()))
                    if ((hSelectedItem == pCST->GetHTreeItem ()) && (((CButton *) GetParent ()->GetDlgItem (IDC_CHECK_AUTO_UPDATE))->GetCheck () == BST_UNCHECKED))
                        pCST->Update (); //������ ��� ������������� ���-�� ������
                    else
                        GetParent()->SendMessage(WM_NOTIFY, (WPARAM) m_hWnd, (LPARAM) &hdr);
                else
                    pCST->Update (); //������ ��� ������������� ���-�� ������
            }
            else {
                //�� - �������� ����
                strLog.format ("������ ID=%i �� ������� ����������", pCST->GetID ());
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_TEMPLATE)
                
                pCST->ResetCountPassError ();
            }
        }
        
        strLog.format ("����� 'UpdateObjectCheckStateTemplate'");
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), TREE_CTRL_TEMPLATE)
}
#include "StdAfx.h"

#include "HContentTemplate.h"
#include "HData.h"

#include "HOnceLineTemplate.h"

#if  defined PAR_ENERG_VER_5
#include "HEnerg5Data.h"
#endif

#if defined PAR_ENERG_VER_6
#include "HEnerg6Data.h"
#endif

#if defined PAR_TORNADO_PTK_1
#include "HTornado1Data.h"
#endif

#if defined PAR_TORNADO_PTK_6
#include "HTornado6Data.h"
#endif

#if defined PTR_DB
#include "HRecordset.h"
#else
#endif

EXT_DECLARE_HTHREAD_FILELOG
EXT_DECLARE_WSA

extern int FindDate(int, char * , int = 5); //�� ��������� ����� �� ������������ ��������
extern int FindTime(int , char * , int = 5); //�� ��������� ����� �� ������������ ��������

extern double roundToPrecision (double , int );

//EXT_DECLARE_HFILELOG

IMPLEMENT_DYNCREATE (HContentTemplate, HObjectTemplate)

HContentTemplate::HContentTemplate (int id, HTREEITEM hItem, HString strWildcard, CTimeSpan tmSpanModify, CTimeSpan tmSpanTask, CTimeSpan tmSpanWork, int idInterval, int iMinExt, int iMaxExt, BOOL bFullInfo) : HObjectTemplate (id, hItem, strWildcard, tmSpanModify, tmSpanTask, tmSpanWork, idInterval, iMinExt, iMaxExt, bFullInfo) {
    m_tmSpanData = -1;
    m_iStop = 0;

    SetInfo (bFullInfo);
}

HContentTemplate::~HContentTemplate (void) {
    int i = -1;
    
    for (i = 0; i < m_arstrAContent.GetSize (); i++)
        m_arstrAContent.GetAt (i).free ();
    
    m_arstrAContent.RemoveAll ();
    m_arstrAContent.FreeExtra ();
    
    HOnceLineTemplate *pOLTemplate = NULL;
    for (i = 0; i < m_arptrOnceLineTemplate.GetSize (); i++)
    {
        pOLTemplate = (HOnceLineTemplate *) m_arptrOnceLineTemplate.GetAt (i);
        delete (HOnceLineTemplate *) pOLTemplate;
    }
    m_arptrOnceLineTemplate.RemoveAll ();
    m_arptrOnceLineTemplate.FreeExtra ();
}

void HContentTemplate::CopyContentTemplate (CPtrArray &ar) {
    int i = 0;
    HOnceLineTemplate *pOLTemplate = NULL;
        
    m_arptrOnceLineTemplate.SetSize (ar.GetSize ());
    for (i = 0; i < ar.GetSize (); i ++)
    {
        pOLTemplate = DEBUG_NEW HOnceLineTemplate ();
        ((HOnceLineTemplate *) ar.GetAt (i))->CopyTo (pOLTemplate);
        m_arptrOnceLineTemplate.SetAt (i, pOLTemplate);
    }
        
    m_ardblParametrsTotalData.SetSize (m_arptrOnceLineTemplate.GetSize ());
}

// HObjectTemplate member functions
int HContentTemplate::ParseTemplate (HString strTypeSource) {
    int i = 0;
    int iPosIdentifier = -1, iPosParametr = -1, iLenParametr = 0;
    int iRes = 0; //Success
    
    HOnceLineTemplate *pOLTemplate;
    
    FILELOG_VARIABLE
    CArray <CStringA> arStrPosition;
    CArray <CStringA> arStrPlace;
    
    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
    strLog.format ("���� � 'ParseTemplate'.");
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
    FILELOG_ENDIF
    
	/*
	//strTmp.GetBufferSetLength (MAX_PATH);
	strTmp.Format ("������ ����� �� �����-�������...");
	FILELOG_WRITE_WITHDT (strTmp.GetBuffer ());
	strTmp.Empty ();
	*/
	
	strTypeSource.upr ();
	
	if (strTypeSource.isUses () == 1)
    {//���� �� ������
        for (i = 0; i < m_arstrAContent.GetSize (); i ++)
        {        
            pOLTemplate = DEBUG_NEW HOnceLineTemplate (m_arstrAContent.GetAt (i));
            m_arptrOnceLineTemplate.Add (pOLTemplate);
        
            //������ ���� ����/������� �� ����� ������� 'd,m,y; h,n,s'
            switch (i + 1)
            {
                case 1: //1-� ������
                    iPosParametr = FindDate (0, pOLTemplate->GetBufferTemplate ());
                    if (iPosParametr > -1)
                    {
                        iLenParametr = pOLTemplate->AddParametr (iPosParametr, "dt_cur");
                    }
                    break;
                case 2: //2-� ������
                    iPosParametr = FindTime (0, pOLTemplate->GetBufferTemplate ());
                    if (iPosParametr > -1)
                    {
                        iLenParametr = pOLTemplate->AddParametr (iPosParametr, "tm_cur");
                        
                        /*if (FindTime (0, pOLTemplate->GetBufferTemplate ()) < FindDate (0, pOLTemplate->GetBufferTemplate ()))
                        {
                            iPosParametr = FindTime (0, pOLTemplate->GetBufferTemplate ());
                            
                            if (iPosParametr > -1)
                            {
                                iLenParametr = pOLTemplate->AddParametr (iPosParametr, "tm_beg");
                                
                                iPosParametr = FindDate (0, pOLTemplate->GetBufferTemplate ());
                                if (iPosParametr > -1)
                                {
                                    iLenParametr = pOLTemplate->AddParametr (iPosParametr, "dt_beg");
                                    
                                    iPosParametr = FindTime (0, pOLTemplate->GetBufferTemplate ());
                                    if (iPosParametr > -1)
                                    {
                                        iLenParametr = pOLTemplate->AddParametr (iPosParametr, "tm_end");
                                        
                                        iPosParametr = FindDate (0, pOLTemplate->GetBufferTemplate ());
                                        if (iPosParametr > -1)
                                        {
                                            iLenParametr = pOLTemplate->AddParametr (iPosParametr, "dt_end");
                                        }
                                    }
                                }
                            }
                        }
                        else*/
                            //������� �� ����� ������� => ������� � ��Ψ�
                            pOLTemplate->AddParametrsDT ();
                    }
                    else
                        ; //��������� � �� ���������� ���������� �����-�������
                    break;
                case 3:
                    break;
                default:
                    if (strlen (pOLTemplate->GetBufferTemplate ()) > 59)
                    {//������ ����� ����� ������ 59 �������� (�.�. ������)
                        HString strAParametrValue, strAParametrType;
                                            
                        pOLTemplate->AddParametr (59, "frml", "=P1", 10);
                        
                        if (strTypeSource.find (0, _T ("KTS")) > -1)
                        {
                            iPosIdentifier = pOLTemplate->FindIdentifierParametrType (0);
                            if (iPosIdentifier > -1)
                            {
                                while (iPosIdentifier > -1)
                                {
                                    strAParametrType = pOLTemplate->GetParametrTypeOfIdentifier (iPosIdentifier);
                                    strAParametrValue = pOLTemplate->FindParametrValue (iPosIdentifier);
                                    pOLTemplate->AddParametr (59, strAParametrType, strAParametrValue, 10);
                                    
                                    iPosIdentifier = pOLTemplate->FindIdentifierParametrType (++ iPosIdentifier);
                                }
                            }
                        }
                        else
                            if (strTypeSource.find (0, _T ("TORNADO")) > -1)
                            {
                                iPosIdentifier = 13;
                               
                                strAParametrType = pOLTemplate->GetParametrTypeOfIdentifier (-1);
                                strAParametrValue = HString (pOLTemplate->GetBufferTemplate ());
                                strAParametrValue = strAParametrValue.tokenize (" ", iPosIdentifier);
                                
                                pOLTemplate->AddParametr (59, strAParametrType, strAParametrValue, 10);
                                
                                iPosIdentifier = pOLTemplate->FindIdentifierParametrType (++ iPosIdentifier);
                            }
                            else
                                ;
                        
                        pOLTemplate->SetSizeArrayParametrData ();
                    }
                    else //������ ����� ����� ������ 59 ��������
                        if (i + 1 > 5)
                        {
                            pOLTemplate = (HOnceLineTemplate *) m_arptrOnceLineTemplate.GetAt (m_arptrOnceLineTemplate.GetSize () - 1);
                            m_arptrOnceLineTemplate.RemoveAt (m_arptrOnceLineTemplate.GetSize () - 1);
                            delete pOLTemplate;
                        }
                        else
                            ;
            }
        }
        
        if (m_arptrOnceLineTemplate.GetSize ())
        {
            m_ardblParametrsTotalData.SetSize (m_arptrOnceLineTemplate.GetSize ());
            if (m_ardblParametrsTotalData.GetSize () == 0)
                iRes = ERR_PARSE_NULL;
            else
                ; //Success (iRes == 0 ???)
        }
        else
            iRes = ERR_PARSE_NULL;
    }
    else
    {
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("������� ������ ��� %s'::ParseTemplate' �� ����������", MB_HSTRING (HString (GetRuntimeClass ()->m_lpszClassName)));
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
        FILELOG_ENDIF
        
        iRes = ERR_ARGS; //Error ???
    }

	/*
	strTmp.Format ("��������� ������ ����� �� �����-�������.");
	FILELOG_WRITE_WITHDT (strTmp.GetBuffer ());
	strTmp.Empty ();
	*/
	
	FILELOG_IF_WRITE (CONTENT_TEMPLATE)
	strLog.format ("����� �� 'ParseTemplate' � ����� %i", iRes);
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
    FILELOG_ENDIF

    return iRes;
}

#ifdef PTR_DB

int HContentTemplate::IsExistsTable ()
{
    if (m_iIdTemplate)
        return IsExistsTable (GetNameTableTemplate (m_iIdTemplate));
    else
        return 0;
}

int HContentTemplate::IsExistsTable (HString nameTable)
{
    int iRes = TRUE;
    HString strQuery;
    CRecordset rs (PTR_DB);

    strQuery = _T ("SELECT 1 FROM ");
    strQuery.append (nameTable);

    //try { rs.Open (CRecordset::dynaset, _T ("SELECT Flags FROM MSysObjects WHERE Type=1 and name=") + nameTable); }
    try { rs.Open (CRecordset::dynaset, WC_HSTRING (strQuery)); }
    catch (CDBException *err) {
        iRes = ERR_QUERY_DB;
		err->Delete ();
    }
    
    if (iRes == TRUE)
        //������������� �������� ���������� ������� ???
        if (rs.IsEOF ())
            //��� �������, �� ��������� ���� ���� ???
            iRes = FALSE;
        
    rs.Close ();
        
    return iRes;
}

//int HObjectTemplate::CreateTable (int id)
int HContentTemplate::CreateTable ()
{
    int iRes = 0;
    //m_iIdTemplate = id;
    HString strQuery;
    
    /*    
    //�������� ����. � ������ �� ������ ������������ �����-������� 'fTemplate', 'strContentTeplateFile'
    //������� �1
    CDaoTableDef *pTblTemplate = DEBUG_NEW CDaoTableDef (PTR_DB);
    try { pTblTemplate->Create (GetNameTableTemplate (m_iIdTemplate).GetBuffer ()); }
    catch (CDBException *err) {
		err->Delete ();
    }
    
    CDaoFieldInfo daoFieldInfo;
    daoFieldInfo.m_strName = HString (_T("NUMBER_LINE"));
    daoFieldInfo.m_nType = dbInteger;
    daoFieldInfo.m_lSize = 2;
    daoFieldInfo.m_lAttributes = dbUpdatableField;
    daoFieldInfo.m_bAllowZeroLength = false;
    daoFieldInfo.m_bRequired = true;
    //m_strDefaultValue = ;
    pTblTemplate->CreateField (daoFieldInfo);

    daoFieldInfo.m_strName = _T("CONTENT_LINE");
    daoFieldInfo.m_nType = dbText;
    daoFieldInfo.m_lSize = 255;
    daoFieldInfo.m_lAttributes = dbUpdatableField | dbFixedField;
    daoFieldInfo.m_bAllowZeroLength = true;
    daoFieldInfo.m_bRequired = true;
    //m_strDefaultValue = ;
    pTblTemplate->CreateField (daoFieldInfo);
    
    daoFieldInfo.m_strName = _T("PARAM_TYPE");
    daoFieldInfo.m_nType = dbText;
    daoFieldInfo.m_lSize = 128;
    daoFieldInfo.m_lAttributes = dbUpdatableField | dbFixedField;
    daoFieldInfo.m_bAllowZeroLength = true;
    daoFieldInfo.m_bRequired = false;
    //m_strDefaultValue = ;
    pTblTemplate->CreateField (daoFieldInfo);
    
    daoFieldInfo.m_strName = _T("PARAM_VALUE");
    daoFieldInfo.m_nType = dbText;
    daoFieldInfo.m_lSize = 128;
    daoFieldInfo.m_lAttributes = dbUpdatableField | dbFixedField;
    daoFieldInfo.m_bAllowZeroLength = true;
    daoFieldInfo.m_bRequired = false;
    //m_strDefaultValue = ;
    pTblTemplate->CreateField (daoFieldInfo);        
    
    CDaoIndexInfo daoIndexInfo;
    daoIndexInfo.m_strName = HString (_T("Primary_key"));
    daoIndexInfo.m_bPrimary = true;
    daoIndexInfo.m_bRequired = true;
    daoIndexInfo.m_bUnique = true;
    daoIndexInfo.m_bIgnoreNulls = false;
    daoIndexInfo.m_nFields = 1;
    CDaoIndexFieldInfo daoIndexFieldInfo [1];
    daoIndexFieldInfo [0].m_strName = _T("NUMBER_LINE");
    daoIndexFieldInfo [0].m_bDescending = false;
    daoIndexInfo.m_pFieldInfos = daoIndexFieldInfo;
    pTblTemplate->CreateIndex (daoIndexInfo);
    
    
    try { pTblTemplate->Append (); }
    catch (CDBException *err) {
        if (err->m_pErrorInfo->m_lErrorCode == 3010)
            PTR_DB->DeleteTableDef (GetNameTableTemplate (m_iIdTemplate).GetBuffer ());
		else
			;
            
        pTblTemplate->Append ();

		err->Delete ();
    }
    
    pTblTemplate->Close ();
    
    delete pTblTemplate;
    */ 
    
    //������� �2
    if (IsExistsTable () == TRUE) {
        strQuery.free ();
        strQuery = HString (_T ("DROP TABLE "));
        strQuery.append (GetNameTableTemplate (m_iIdTemplate));
        
        try { PTR_DB->ExecuteSQL (WC_HSTRING (strQuery)); }
        catch (CDBException *err) {
            iRes = ERR_DELETE_TABLE;
			err->Delete ();
        }
    }
    else
        ;
    
    if (iRes == 0) {
        strQuery.free ();
        strQuery = HString (_T ("CREATE TABLE "));
        //fileName �.�. ���������� ��� ������ ��������� ???
        strQuery.append (GetNameTableTemplate (m_iIdTemplate));
        strQuery.append (_T (" (NUMBER_LINE INT PRIMARY KEY, CONTENT_LINE CHAR (255), PARAM_TYPE CHAR (128), PARAM_VALUE CHAR (128))"));
        
        try { PTR_DB->ExecuteSQL (WC_HSTRING (strQuery)); }
        catch (CDBException *err) {
            iRes = ERR_CREATE_TABLE;
			err->Delete ();
        }
        
        /*    
        strQuery.Empty ();
        strQuery = HString (_T("ON UPDATE"));
        
        try { PTR_DB->ExecuteSQL (strQuery); }
        catch (CDBException *err) {
			err->Delete ();
            return ERR_CREATE_TABLE;
        }
        */
    }
    else
        ;
        
    return iRes;
}

int HContentTemplate::SetContentTemplate ()
{
    int i, iRes = 0, p, c;
    HOnceLineTemplate *pOLTemplate = NULL;
    HString strAParametrType, strAParametrValue,
			strNameTableTemplate = GetNameTableTemplate (m_iIdTemplate);;
    
    //�������� ��'���� 'CDaoTableDef'
    InitializeRS (WC_HSTRING (strNameTableTemplate));
    HRecordset *pRSTemplate = (HRecordset *) m_pmapDBRS [WC_HSTRING (strNameTableTemplate)];
  
    COleVariant data;
    //for (i = 0; i < m_arstrAContent.GetSize (); i ++)
    for (i = 0; i < m_arptrOnceLineTemplate.GetSize (); i ++)
    {
        pOLTemplate = (HOnceLineTemplate *) m_arptrOnceLineTemplate.GetAt (i);
    
        pRSTemplate->AddNew ();

        data = (COleVariant ) (LONG) (i + 1);
        pRSTemplate->SetFieldValue (_T("NUMBER_LINE"), data);
        data = (COleVariant) CString (pOLTemplate->GetBufferTemplate ());
        pRSTemplate->SetFieldValue (_T("CONTENT_LINE"), data);
        
        strAParametrType.free ();
        strAParametrValue.free ();
        
        c = 0;
        p = 0;
        while (pOLTemplate->GetParametrTypeDetail (p, c).isUses () == 1)
        {
            strAParametrType.append (pOLTemplate->GetParametrTypeDetail (p, c));
            strAParametrValue.append (pOLTemplate->GetParametrValueDetail (p, c));
            
            c ++;
        
            while (pOLTemplate->GetParametrTypeDetail (p, c).isUses () == 1)
            //�� ����� �� ����� 'IsChnlGrpUserNumber' ???
            {
                strAParametrType.appendChar (',');
                strAParametrType.append (pOLTemplate->GetParametrTypeDetail (p, c));
                 
                strAParametrValue.appendChar (',');
                strAParametrValue.append (pOLTemplate->GetParametrValueDetail (p, c));                    
                
                c ++;
            } //���� 'while' �� ���������
                        
            c = 0;
            p ++;
            
            if (pOLTemplate->GetParametrTypeDetail (p, c).isUses () == 0)
                break;
            else
                strAParametrType.appendChar (';');
        } //���� 'while' �� ����������
        
        data = (COleVariant) CString (pOLTemplate->GetBufferTemplate ());
        pRSTemplate->SetFieldValue (_T("CONTENT_LINE"), data);
        data = (COleVariant) CString (strAParametrType.GetBuffer ());
        pRSTemplate->SetFieldValue (_T("PARAM_TYPE"), data);
        data = (COleVariant) CString (strAParametrValue.GetBuffer ());
        pRSTemplate->SetFieldValue (_T("PARAM_VALUE"), data);

        try { pRSTemplate->Update (); }
        catch (CDBException *err) {
            iRes = ERR_SET_CONTENT_TEMPLATE;
			err->Delete ();
            break;
        }
    }

    ReleaseRS (WC_HSTRING (strNameTableTemplate), true, true);
    
    strAParametrType.free (); strAParametrValue.free ();
    
    return iRes;
}

int HContentTemplate::GetContentTemplate (int iIdTemplate) //������ �� ��
{
    int i, iRes = 0;
	FILELOG_VARIABLE
    HOnceLineTemplate *pOLTemplate = NULL;
    HString strAContent, strAParametrType, strAParametrValue;
	HRecordset *pRSTemplate = NULL;
    
    if (iIdTemplate < 0)
        iIdTemplate = m_iIdTemplate;
	else
		;
    
	HString strNameTableTemplate = GetNameTableTemplate (iIdTemplate);

    //�������� ��'���� 'CDaoTableDef'
	iRes = InitializeRS (WC_HSTRING (strNameTableTemplate));

	FILELOG_IF_WRITE (CONTENT_TEMPLATE)
    strLog.format ("HObjectTemplate::InitializeRS (%s) = %i", MB_HSTRING (strNameTableTemplate), iRes);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), CONTENT_TEMPLATE);
    FILELOG_ENDIF

	if (iRes == 0) {
		pRSTemplate = (HRecordset *) m_pmapDBRS [WC_HSTRING (strNameTableTemplate)];
	        
		if (pRSTemplate) {
			//EnterCriticalSection ();
			
			i = 1;
			if (pRSTemplate->CanScroll ()) {
				TRY { pRSTemplate->MoveFirst (); }
				CATCH_ALL (err) {
					i = 0;
					iRes = -1;
					THROW (err);
				}
				END_CATCH_ALL
			}
			else
				;

			FILELOG_IF_WRITE (CONTENT_TEMPLATE)
			strLog.format ("HContentTemplate::GetContentTemplate (%s)...MoveFirst...", MB_HSTRING (strNameTableTemplate));
			FILELOG_WRITE_WITHDT (strLog.GetBuffer (), CONTENT_TEMPLATE);
			FILELOG_ENDIF

			while ((! pRSTemplate->IsEOF ()) && (i > 0)) {
				/*FILELOG_IF_WRITE (CONTENT_TEMPLATE)
				strLog.format ("HContentTemplate::GetContentTemplate (NUMBER_LINE - ��) ������ �%i", i);
				FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
				FILELOG_ENDIF*/

				//pRSTemplate->GetFieldValue (_T ("NUMBER_LINE"), i);

				/*FILELOG_IF_WRITE (CONTENT_TEMPLATE)
				strLog.format ("HContentTemplate::GetContentTemplate (NUMBER_LINE - �����) ������ �%i", i);
				FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
				FILELOG_ENDIF*/
		        
				if (i > 0) {
					/*FILELOG_IF_WRITE (CONTENT_TEMPLATE)
					strLog.format ("HContentTemplate::GetContentTemplate ...���� ������� (CONTENT_LINE - ��) ������ �%i", i);
					FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
					FILELOG_ENDIF*/

					pRSTemplate->GetFieldValue (_T ("CONTENT_LINE"), strAContent);
					strAContent.trimRight ();

					FILELOG_IF_WRITE (CONTENT_TEMPLATE)
					strLog.format ("HContentTemplate::GetContentTemplate ...���� ������� (DEBUG_NEW HOnceLineTemplate - ��) ������ �%i", i);
					FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
					FILELOG_ENDIF
					
					pOLTemplate = NULL;
					//pOLTemplate = DEBUG_NEW HOnceLineTemplate (strAContent);

					if (! (pOLTemplate == NULL)) { TRY { delete pOLTemplate; } CATCH_ALL (err) { THROW (err); } END_CATCH_ALL; pOLTemplate = NULL; } else ;
					while (pOLTemplate == NULL) { TRY { pOLTemplate = DEBUG_NEW HOnceLineTemplate (strAContent); } CATCH_ALL (err) { THROW (err); } END_CATCH_ALL }

					FILELOG_IF_WRITE (CONTENT_TEMPLATE)
					strLog.format ("HContentTemplate::GetContentTemplate ...���� ������� (m_arptrOnceLineTemplate.InsertAt - ��) ������ �%i", i);
					FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
					FILELOG_ENDIF

					TRY { m_arptrOnceLineTemplate.InsertAt (i - 1, pOLTemplate); }
					CATCH_ALL (err) { THROW (err); iRes = -1; break; } END_CATCH_ALL

					FILELOG_IF_WRITE (CONTENT_TEMPLATE)
					strLog.format ("HContentTemplate::GetContentTemplate ...���� ������� (PARAM_TYPE - ��) ������ �%i", i);
					FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
					FILELOG_ENDIF
			        
					pRSTemplate->GetFieldValue (_T ("PARAM_TYPE"), strAParametrType);
					strAParametrType.trim ();
					pOLTemplate->SetParametrsType (strAParametrType);

					FILELOG_IF_WRITE (CONTENT_TEMPLATE)
					strLog.format ("HContentTemplate::GetContentTemplate ...���� ������� (PARAM_VALUE - ��) ������ �%i", i);
					FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
					FILELOG_ENDIF
			        
					pRSTemplate->GetFieldValue (_T ("PARAM_VALUE"), strAParametrValue);
					strAParametrValue.trim ();
					pOLTemplate->SetParametrsValue (strAParametrValue);

					FILELOG_IF_WRITE (CONTENT_TEMPLATE)
					strLog.format ("HContentTemplate::GetContentTemplate ...���� ������� (pOLTemplate->SetSizeArrayParametrData - ��) ������ �%i", i);
					FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
					FILELOG_ENDIF
			        
					pOLTemplate->SetSizeArrayParametrData ();
			     
					FILELOG_IF_WRITE (CONTENT_TEMPLATE)
					strLog.format ("HContentTemplate::GetContentTemplate (��� ��������� - �����) ������ �%i", i);
					FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
					FILELOG_ENDIF

					pRSTemplate->MoveNext ();
					i ++;
				}
				else {
					iRes = -1;
					break;
				}
			}

			//LeaveCriticalSection ();
			
			FILELOG_IF_WRITE (CONTENT_TEMPLATE)
			strLog.format ("HContentTemplate::GetContentTemplate (%s)...pRSTemplate->IsEOF () = TRUE...���-� = %i", MB_HSTRING (strNameTableTemplate), iRes);
			FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
			FILELOG_ENDIF

			m_ardblParametrsTotalData.SetSize (m_arptrOnceLineTemplate.GetSize ());
		}
		else
			iRes = -1;

		FILELOG_IF_WRITE (CONTENT_TEMPLATE)
		strLog.format ("HContentTemplate::GetContentTemplate (%s)...������ ������� (������ = %i)...���-� = %i", MB_HSTRING (strNameTableTemplate), m_arptrOnceLineTemplate.GetSize (), iRes);
		FILELOG_WRITE_WITHDT (strLog.GetBuffer (), CONTENT_TEMPLATE);
		FILELOG_ENDIF
	    
		ReleaseRS (WC_HSTRING (strNameTableTemplate), true, true);
		//ReleaseRS (strNameTableTemplate.GetBuffer (), false, false);
	}
	else
		;
    
    return iRes;
}
#endif

//���������� ������� ��� ������, ��� ������� 'NULL'
//int HContentTemplate::GenerateContentTemplate (HString strTypeSource)
int HContentTemplate::GenerateContentTemplate () {
    int iRes = 0, i;
    //VARIANT varData;
    HOnceLineTemplate *pOLTemplate;
    HString strATmp;
    FILELOG_VARIABLE
    
    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
    //strATmp.Format ("���� � 'GenerateContentTemplate' � ����������: %s",  _WC_TO_MB (CStringW (strTypeSource)));
    strLog.format ("���� � 'GenerateContentTemplate' � ����������: %s",  "���������� ���!");
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
    FILELOG_ENDIF
    
    //������� ��˸��� ��������
    m_iCountNullValue = 0;

    CTime tmCalculate = m_tmTarget;
    CTimeSpan tmSpanInterval = CTimeSpan (PTR_DATETIMEINTERVAL->GetInterval (m_iIdInterval, tmCalculate, HDateTimeInterval::DIRECTION_DATETIME::FUTURE, TRUE));

    //����� ��������� ����/����� (��� 0-4 ������)
    //��� ��������� (>4) ��������� ����������� ������
    for (i = 0; i < m_arptrOnceLineTemplate.GetSize (); i ++) {
		/*if (m_iStop) {
            iRes = ERR_BREAK;
            break;
        }
        else
            ;*/

        pOLTemplate = (HOnceLineTemplate *) m_arptrOnceLineTemplate.ElementAt (i);
        strATmp.free ();
        switch (i) {
            case 0:
                if (pOLTemplate->GetParametrType (0).compareNoCase ("dt_cur") == 0)
                    pOLTemplate->ApplyParametrData (0, HString ((CTime::GetCurrentTime ().Format ("%d.%m.%Y").GetBuffer ())));
	            break;
            case 1:
	            //������ ������
	            //����� - �������
	            if (pOLTemplate->GetParametrType (0).compareNoCase ("tm_cur") == 0)
                    pOLTemplate->ApplyParametrData (0, HString ((CTime::GetCurrentTime ().Format ("%H:%M:%S").GetBuffer ())));
				else
					;
	            //����� ������
	            //�����  - ������
	            strATmp.free ();
	            strATmp = HString (tmCalculate.Format ("%H:%M").GetBuffer ());
	            if (pOLTemplate->GetParametrType (1).compareNoCase ("tm_beg") == 0)
                    pOLTemplate->ApplyParametrData (1, strATmp);
				else
					;
	            //���� - ������
	            strATmp.free ();
	            strATmp = HString (tmCalculate.Format ("%d.%m.%y").GetBuffer ());
	            if (pOLTemplate->GetParametrType (2).compareNoCase ("dt_beg") == 0)
                    pOLTemplate->ApplyParametrData (2, strATmp);
				else
					;
                //�����  - �����
                strATmp.free ();
                strATmp = strATmp + HString ((CTime (tmCalculate + GetIntervalVariable (tmCalculate, DIRECTION_FUTURE))).Format ("%H:%M").GetBuffer ());
                if (pOLTemplate->GetParametrType (3).compareNoCase ("tm_end") == 0)
                    pOLTemplate->ApplyParametrData (3, strATmp);
				else
					;
	            //���� - �����
	            strATmp.free ();
	            strATmp = strATmp + HString ((CTime (tmCalculate + GetIntervalVariable (tmCalculate, DIRECTION_FUTURE))).Format ("%d.%m.%y").GetBuffer ());
	            if (pOLTemplate->GetParametrType (4).compareNoCase ("dt_end") == 0)
                    pOLTemplate->ApplyParametrData (4, strATmp);
				else
					;
	            break;
            default:
                //strATmp.Format ("GenerateContentTemplate: ������ �%i",  i + 1);
                //FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_3);
                //strATmp.Empty ();
                if (strlen (pOLTemplate->GetBufferTarget ()) > 59)
                {
	                pOLTemplate->ApplyParametrData (0, m_ardblParametrsTotalData [i]);
	                
	                if (m_ardblParametrsTotalData [i] == 0)
	                {
	                    m_iCountNullValue ++;
	                }
	                else
	                    ;
                }
                else
                    ;
	            break;
        }
        
        strATmp = pOLTemplate->GetBufferTarget ();
        strATmp.trimRight ();
        
        if (strATmp.isUses () == 0)
            strATmp.appendChar (' ');
            
        m_arstrAContent.Add (strATmp);
        //m_arstrAContent.GetAt (m_arstrAContent.GetSize () - 1).Delete (79, m_arstrAContent.GetAt (m_arstrAContent.GetSize () - 1).GetLength ());
        //m_arstrAContent.GetAt (m_arstrAContent.GetSize () - 1).TrimRight ();
    }
    
    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
    strLog.format ("����� �� 'GenerateContentTemplate' � �����: %i", iRes);
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
    FILELOG_ENDIF
    
    strATmp.free ();
    
    return iRes;
}

int HContentTemplate::ReadContentTemplate (HString strAfullPath) {
    int i = 0, iResponce = 0;
    
    CFile fTemplate;
    BYTE ch = 0x0;
    short shCountCR = 0;
    FILELOG_VARIABLE
    HString strTmp;
    HString strAOnceLine, strATmp, strABufferOfFile;
    //CArray <BYTE> arContentTeplateFile;
    CFileException *err = 0x0;
    
    if (strAfullPath.isUses () == 0) {
        m_pHFILast->FileExt (strTmp);
        strATmp = strTmp;
        strAfullPath.format ("%s", m_strWildcard.left (m_strWildcard.length () - 1));
        strAfullPath.append (strATmp);
        strATmp.free ();
    }
	else
		;
    
    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
	strLog.format ("��������� ��������� ����� ������� ���� ������: %s", MB_HSTRING (strAfullPath));
	FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
	FILELOG_ENDIF
	strATmp.free ();
    	
	if (fTemplate.Open (WC_HSTRING (strAfullPath), CFile::modeRead, err) == 0) {
	    FILELOG_IF_WRITE (HERROR)
	    strLog.format ("������ �������� �����: %s (OS: %i, Runtime: %i)", MB_HSTRING (strAfullPath), err->ErrnoToException (err->m_cause), err->OsErrorToException (err->m_lOsError));
	    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
	    FILELOG_ENDIF
	    strATmp.free ();
	    
	    iResponce = -1;
    }
    else {
	    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
	    strLog.format ("������ ��������� ����� ������� ���� ������: %s. ������: %i", MB_HSTRING (strAfullPath), fTemplate.GetLength ());
	    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
	    FILELOG_ENDIF
	    strATmp.free ();

        strABufferOfFile.setLength (fTemplate.GetLength ());
        fTemplate.Read (MB_HSTRING (strABufferOfFile), fTemplate.GetLength ());
        fTemplate.Close ();
    	
	    /*
	    strATmp.Format ("������ ������ �����: %s.", strFileNameOfRead);
	    FILELOG_WRITE_WITHDT (strATmp.GetBuffer ());
	    strATmp.Empty ();
        */

        FILELOG_IF_WRITE (CONTENT_TEMPLATE)
	    strLog.format ("������� ��������� �����-�������");
	    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
	    FILELOG_ENDIF
	    strATmp.free ();

        while (i < strABufferOfFile.length ()) {
            ch = MB_AT (strABufferOfFile, i);
            if ((ch == 10) || (ch == 13))
            {
                m_arstrAContent.Add (strAOnceLine);
                
                FILELOG_IF_WRITE (CONTENT_TEMPLATE)
                strLog.format ("��������� ������ �%i ������ %i �� �����-�������", i + 1, strAOnceLine.length ());
	            FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
	            FILELOG_ENDIF
         
                strAOnceLine.free ();
                
                while (((ch == 10) || (ch == 13)) && ((i + 1) < strABufferOfFile.length ()))
                {
                    if (ch == 0xD)
                    {
                        shCountCR ++;
                        
                        if ((shCountCR > 1) && (m_arstrAContent.GetSize () < 5))
                            m_arstrAContent.Add (" ");
                        else
                            ;
                    }
                    else
                        ;
                        
                    i ++;
                    ch = MB_AT (strABufferOfFile, i + 1);
                }
                
                shCountCR = 0;
            }
            else
                strAOnceLine.appendChar ((char) ch);
                        
            i ++;
        }
        
        FILELOG_IF_WRITE (CONTENT_TEMPLATE)
        strLog.format ("������������ ����� �� �����-������� %i", m_arstrAContent.GetSize ());
	    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
	    FILELOG_ENDIF
	    
	    strATmp.free ();
        strABufferOfFile.free ();
    }
    
    return iResponce;
}

//�� ����/����� �����������
CTime HContentTemplate::GetDTTarget (CTime tm)
{
    CTime tmRes = -1;
    FILELOG_VARIABLE
    
    if (m_tmSpanData > -1)
        tmRes = tm + (m_tmSpanData - m_tmSpanModify);
    else
    {//�� ��������� ����� (���������) ����� ������ ��� ������������ ����� � ������� 'HTreeCtrlTemplate::CreateObjectContentTemplate'
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("%s", "�� ��������� ����� (���������) ����� ������ ��� ������������ �����");
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
        FILELOG_ENDIF
    }
    
    return tmRes;
}

void HContentTemplate::NewTarget (CTime tm, HString strAPath)
{
    int i = -1;

    SYSTEMTIME stmTmp;
    tm.GetAsSystemTime (stmTmp);

    CTimeSpan tsVariable = GetIntervalVariable (tm, HDateTimeInterval::DIRECTION_DATETIME::FUTURE);

    ////������ ��� �������� �� ����-���� 26.10.2014 02:00
    //if (! (tsVariable.GetTotalSeconds () < (24 * 60 * 60)))
    //    tm += 60 * 60;
    //else
    //    ;

    //����/����� - ������ ����� ����������
    m_tmTarget = tm;

    SetTarget (&m_stHITTarget, tm + tsVariable + m_tmSpanModify, false);
    //SetTarget (&m_stHITTarget, tm + m_tmSpanInterval);
    
    if (strAPath.isUses () == 1)
        m_strDirOutput = strAPath;
    else
        m_strDirOutput = m_strWildcard.left (m_strWildcard.reverseFind ('\\'));
        
    for (i = 0; i < m_arptrOnceLineTemplate.GetSize (); i++)
    {
        m_ardblParametrsTotalData [i] = 0x0;
    
        if (! m_arptrOnceLineTemplate.ElementAt (i) == NULL)
            ((HOnceLineTemplate *) m_arptrOnceLineTemplate.ElementAt (i))->NewTarget ();
        else
        {
            TRACE ("Bad ptr with index ...");
        }            
    }
    
    for (i = 0; i < m_arstrAContent.GetSize (); i++)
    {
        m_arstrAContent.GetAt (i).free ();
    }
    
    m_arstrAContent.RemoveAll ();
    m_arstrAContent.FreeExtra ();
}

int HContentTemplate::CalculateTemplate (HString strDataBase, HString strTypeSource) {
    FILELOG_VARIABLE
    HString strATmp;
    int iRes = -1;
    double val = 0;
    
    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
    strLog.format ("���� � 'CalculateTemplate' � �����������: %s, %s", MB_HSTRING (strDataBase), MB_HSTRING (strTypeSource));
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
    FILELOG_ENDIF
    
    //������� ��˸��� ��������
    m_iCountNullValue = 0;
    
    //CTimeSpan tmSpanInterval = GetIntervalVariable (m_tmTarget, HDateTimeInterval::DIRECTION_DATETIME::FUTURE);
    CTime tmCalculate = m_tmTarget;
    CTimeSpan tmSpanInterval = CTimeSpan (PTR_DATETIMEINTERVAL->GetInterval (m_iIdInterval, tmCalculate, HDateTimeInterval::DIRECTION_DATETIME::FUTURE, TRUE));

    if (m_arptrOnceLineTemplate.GetSize ()) {
        if ((strDataBase.isUses () == 1) && (strTypeSource.isUses () == 1)) {
            strDataBase.upr ();
            strTypeSource.upr ();

            HData *objData = NULL;
	        if (strTypeSource.find (0, _T ("KTS_5")) > -1) {
#ifdef PAR_ENERG_VER_5
	            objData = DEBUG_NEW HEnerg5Data (MB_HSTRING (strDataBase));
#endif
	        }
	        else {
	            if (strTypeSource.find (0, _T ("KTS_6")) > -1) {
#ifdef PAR_ENERG_VER_6
	                objData = DEBUG_NEW HEnerg6Data (MB_HSTRING (strDataBase));
            
                    if (strTypeSource.find (0, _T ("TEPLO")) > -1)
                        ((HEnergData *) objData)->SetClassVal (15);
                    else
                        if (strTypeSource.find (0, _T ("ELEKTRO")) > -1)
                            ((HEnergData *) objData)->SetClassVal (25);
                        else
                            ; //��� �� ��� ???
#endif
                }
                else {
                    if (strTypeSource.find (0, _T ("TORNADO_PTK_1")) > -1) {
#ifdef PAR_TORNADO_PTK_1
	                    objData = DEBUG_NEW HTornado1Data (MB_HSTRING (strDataBase));
#endif
                    }
                    else {
                        if (strTypeSource.find (0, _T ("TORNADO_PTK_6")) > -1) {
#ifdef PAR_TORNADO_PTK_6
							objData = DEBUG_NEW HTornado6Data (MB_HSTRING (strDataBase));
#endif
                        }
                        else
                            ;
                    }
                }
	        }

	        if (objData) {
	            int i = -1;

        	    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
	            strLog.format ("CalculateTemplate: ������ 'objData'");
                FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
                FILELOG_ENDIF
        	
	            if (PTR_DB_DATA (objData)->IsConnectDB () == TRUE) {
	                iRes = 0; //Success
        	        
        	        FILELOG_IF_WRITE (CONTENT_TEMPLATE)
	                strLog.format ("CalculateTemplate: ����� � �� �����������!");
                    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
                    FILELOG_ENDIF
        	    
	                HOnceLineTemplate *pOLTemplate = NULL;
                	
	                int p, c;
#ifdef PAR_ENERG
	                if (strTypeSource.find (0, _T ("TEPLO")) > -1)
	                    ((HEnergData *) objData)->SetValueParametrVyvod (tmCalculate, tmSpanInterval.GetTotalMinutes ());
                    else
                        /*if (strTypeSource.Find (HString ("TORNADO")) > -1)
	                        iRes = (((HTornadoData *) objData)->PrepareGetValue (tmCalculate, tmSpanInterval.GetTotalMinutes ()));
                        else*/
                            ;
#endif
                    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
                    strLog.format ("CalculateTemplate: ������ 'm_arptrOnceLineTemplate'=%i", m_arptrOnceLineTemplate.GetSize ());
                    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
                    FILELOG_ENDIF

                    if (iRes == 0) {
                        //���� �� ���� ������� � �������
                        //���������� X ����� ���������
                        for (i = 3; i < m_arptrOnceLineTemplate.GetSize (); i ++) {
                            if (m_iStop) {
                                iRes = ERR_BREAK;
                                break;
                            }
                            else
                                ;

                            pOLTemplate = (HOnceLineTemplate *) m_arptrOnceLineTemplate.ElementAt (i);

                            FILELOG_IF_WRITE (CONTENT_TEMPLATE)
                            //strLog.format ("CalculateTemplate: ����� 'GetBufferTarget ()'=%i", strlen (pOLTemplate->GetBufferTarget ()));
                            strLog.format ("CalculateTemplate: ������ �%i 'GetBufferTarget ()'=%s", i, pOLTemplate->GetBufferTarget ());
                            FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
                            FILELOG_ENDIF

                            if (strlen (pOLTemplate->GetBufferTarget ()) > 59) {
                                //pOLTemplate->SetSizeArrayParametrData ();

                                m_ardblParametrsTotalData.SetAt (i, 0);
                                c = 1;
                                p = 0;
                                while (pOLTemplate->GetParametrTypeDetail (p, 0).isUses () == 1) {
                                    while ((pOLTemplate->GetParametrTypeDetail (p, c).isUses () == 1) && (pOLTemplate->GetParametrValueDetail (p, c).isUses () == 1))
                                    //�� ����� �� ����� 'IsChnlGrpUserNumber' ???
                                    {
                                        FILELOG_IF_WRITE (CONTENT_TEMPLATE)
                                        strLog.format ("CalculateTemplate: �������� (p=%i, c=%i)=%s", p, c, MB_HSTRING (pOLTemplate->GetParametrTypeDetail (p, c)));
                                        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
                                        FILELOG_ENDIF
#ifdef PAR_ENERG
                                        if (pOLTemplate->GetParametrTypeDetail (p, c).compareNoCase (ENERG_CHANNEL) == 0) {
                                            val = objData->GetValueOfChannel (MB_HSTRING (pOLTemplate->GetParametrValueDetail (p, c)), tmCalculate, tmSpanInterval.GetTotalMinutes ());
                                            if ((strTypeSource.find (0, _T ("ELEKTRO")) > -1) && (strTypeSource.find (0, _T ("KTS_6")) > -1)) {
                                                val /= (((double) tmSpanInterval.GetTotalMinutes ()) / 60);
                                            }
                                            else
                                                ;
                                        }
                                        else
                                            if (pOLTemplate->GetParametrTypeDetail (p, c).compareNoCase (ENERG_GROUP) == 0)
                                                val = ((HEnergData *) objData)->GetValueOfGroup (MB_HSTRING (pOLTemplate->GetParametrValueDetail (p, c)), tmCalculate, tmSpanInterval.GetTotalMinutes ());
                                            else
#endif
#ifdef PAR_TORNADO
                                                if (pOLTemplate->GetParametrTypeDetail (p, c).compareNoCase (TORNADO_SIGNAL) == 0)
                                                    val = objData->GetValueOfChannel (MB_HSTRING (pOLTemplate->GetParametrValueDetail (p, c)), tmCalculate, tmSpanInterval.GetTotalMinutes ());
                                                else
#endif
                                                ;

                                        //����� �����/������ �.�.
                                        //���� ����� �� ������������� - ������� ������� ��������
                                        /*
                                        if (! IsAccumulationChnlGrp (arChnlGrpUnitGrp [i][c], enumAccumulationUnitGrp))
                                        {
                                            arChnlGrpData [i][c] = arChnlGrpData [i][c] / j
                                        }

                                        //���� ����� �������� ������� - ���������� � ������, �.� �������� �� �����
                                        if (arChnlGrpUnitGrp [i][c] == 9)
                                        {
                                            //������� � ����/������� ��������������� ���������� � ���./���./����
                                            arChnlGrpData [i][c] = arChnlGrpData [i][c] * ((dttmEnd - dttmStart) / 1000 / 60 / 60)
                                        }
                                        */

                                        pOLTemplate->ApplyParametrDataDetail (p, c, val);

                                        c ++;
                                    } //���� 'while' �� ���������

                                    val = pOLTemplate->GetParametrData (p);
                                    m_ardblParametrsTotalData [i] += val;
                                    p ++;
                                } //���� 'while' �� ����������

                                FILELOG_IF_WRITE (CONTENT_TEMPLATE)
                                strLog.format ("CalculateTemplate: ��������� ��� ������ �%i: p=%i, c=%i", i, p, c);
                                FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
                                FILELOG_ENDIF

                                if (strTypeSource.find (0, _T ("KTS")) > -1)
                                {
#ifdef PAR_ENERG
                                    //������ ������ ��� ��� - ������� �����������
                                    //�� ���������
                                    if (strTypeSource.find (0, _T ("ELEKTRO")) > -1)
                                    {
#ifdef PAR_ENERG_VER_5
                                        if ((pOLTemplate->IsPosition ("2") && ((pOLTemplate->IsPlace ("blok1")) || (pOLTemplate->IsPlace ("blok2")) || (pOLTemplate->IsPlace ("blok3")) || (pOLTemplate->IsPlace ("blok4")) || (pOLTemplate->IsPlace ("blok5")) || (pOLTemplate->IsPlace ("blok6")))) ||
                                            (pOLTemplate->IsPosition ("8.4") && ((pOLTemplate->IsPlace ("blok1")) || (pOLTemplate->IsPlace ("blok2")) || (pOLTemplate->IsPlace ("blok3")) || (pOLTemplate->IsPlace ("blok4")) || (pOLTemplate->IsPlace ("blok5")) || (pOLTemplate->IsPlace ("blok6")))) ||
                                            (pOLTemplate->IsPosition ("9.4") && (pOLTemplate->IsPlace ("station"))) ||
                                            (pOLTemplate->IsPosition ("10.1") && ((pOLTemplate->IsPlace ("blok1")) || (pOLTemplate->IsPlace ("blok2")) || (pOLTemplate->IsPlace ("blok3")) || (pOLTemplate->IsPlace ("blok4")) || (pOLTemplate->IsPlace ("blok5")) || (pOLTemplate->IsPlace ("blok6")))) ||
                                            (pOLTemplate->IsPosition ("10.2") && ((pOLTemplate->IsPlace ("blok1")) || (pOLTemplate->IsPlace ("blok2")) || (pOLTemplate->IsPlace ("blok3")) || (pOLTemplate->IsPlace ("blok4")) || (pOLTemplate->IsPlace ("blok5")) || (pOLTemplate->IsPlace ("blok6")))) ||
                                            (pOLTemplate->IsPosition ("10.3") && ((pOLTemplate->IsPlace ("blok1")) || (pOLTemplate->IsPlace ("blok2")) || (pOLTemplate->IsPlace ("blok3")) || (pOLTemplate->IsPlace ("blok4")) || (pOLTemplate->IsPlace ("blok5")) || (pOLTemplate->IsPlace ("blok6")))))
                                        {//�������������: �������: X ���� �X (station)
                                            m_ardblParametrsTotalData [i] *= (((double) tmSpanInterval.GetTotalMinutes ()) / 60);
                                        }
#endif
                                        if (pOLTemplate->IsPosition ("10.2") && pOLTemplate->IsPlace ("station"))
                                        {//�������������: �������: 10.2 �� �������
                                            m_ardblParametrsTotalData [i] = 0;
                                            for (c = i - 6; c < i; c ++)
                                            {
                                                m_ardblParametrsTotalData [i] += ((HOnceLineTemplate *) m_arptrOnceLineTemplate [c])->GetParametrData (0)
#ifdef PAR_ENERG_VER_5
                                                 * (((double) tmSpanInterval.GetTotalMinutes ()) / 60)
#endif
                                                 ;
                                                //m_ardblParametrsTotalData [i] += m_ardblParametrsTotalData [c];
                                            }
                                            //m_ardblParametrsTotalData [i] *= 1000000;
                                        }
                                        //if (strAPosition [i] == '11.2' && strAPlace [i] == 'station')
                                        if (pOLTemplate->IsPosition ("11.2") && pOLTemplate->IsPlace ("station"))
                                        {//�������������: �������: 11.2 �� �������
                                            //��������������� �������� ������ (� �������� 0 - ������; 1,2,3 - ������)
                                            c = 1;
                                            double tmp = pOLTemplate->GetParametrDataDetail (0, c ++) * 0.65;
                                            m_ardblParametrsTotalData [i] = 0;
                                            //while (arChnlGrpUserNumber [i][c] != 0)
                                            //��� �������� ����� ���� ������
                                            while (pOLTemplate->GetParametrTypeDetail (0, c).isUses () == 1)
                                            {//������������ �������� �� �������
                                                m_ardblParametrsTotalData [i] += pOLTemplate->GetParametrDataDetail (0, c)
#ifdef PAR_ENERG_VER_5
                                                 * (((double) tmSpanInterval.GetTotalMinutes ()) / 60)
#endif
                                                 ;
                                                c ++;
                                            }
                                            m_ardblParametrsTotalData [i] *= 0.25;
                                            m_ardblParametrsTotalData [i] += tmp;
                                        }

                                        //5 ��� ���������� ������� ???
                                        if (pOLTemplate->IsPosition ("4.2") && ((pOLTemplate->IsPlace ("blok1")) || (pOLTemplate->IsPlace ("blok2")) || (pOLTemplate->IsPlace ("blok3")) || (pOLTemplate->IsPlace ("blok4")) || (pOLTemplate->IsPlace ("blok5"))))
                                        {//�������������: �������: 4.2 ���� �X 
                                                m_ardblParametrsTotalData [i] = pOLTemplate->GetParametrDataDetail (0, 1) / 2 + pOLTemplate->GetParametrDataDetail (0, 2);
                                        }
                                    }
                                    else
                                    {
                                        if (strTypeSource.find (0, _T ("TEPLO")) > -1)
                                        {
                                            if (pOLTemplate->IsPosition ("30") && pOLTemplate->IsPlace ("blok4"))
                                            {//�����: �������: 30 ���� �4
                                                m_ardblParametrsTotalData [i] = (pOLTemplate->GetParametrDataDetail (0, 1) + pOLTemplate->GetParametrDataDetail (0, 2)) / 2;
                                            }

                                            if (strTypeSource.find (0, _T ("KTS_5")) > -1)
	                                        {                    
                                                strATmp = pOLTemplate->GetParametrValueDetail (0, 1);
                                                if ((strATmp.compareNoCase ("116") == 0) ||
                                                    (strATmp.compareNoCase ("117") == 0) ||
                                                    (strATmp.compareNoCase ("196") == 0) ||
                                                    (strATmp.compareNoCase ("197") == 0) ||
                                                    (strATmp.compareNoCase ("293") == 0) ||
                                                    (strATmp.compareNoCase ("294") == 0) ||
                                                    (strATmp.compareNoCase ("453") == 0) ||
                                                    (strATmp.compareNoCase ("454") == 0))
                                                {//�����: 
                                                    m_ardblParametrsTotalData [i] -= 1; 
                                                }

                                                //�.�. �� 337, 338 
                                                //if ((pOLTemplate->IsPlace ("station")) && ((strATmp.compareNoCase ("337") == 0) || (strATmp.CollateNoCase ("338") == 0)))
                                                if ((pOLTemplate->IsPlace ("station")) && ((pOLTemplate->IsPosition ("78")) || (pOLTemplate->IsPosition ("79"))))
                                                {//�����: 
                                                    m_ardblParametrsTotalData [i] *= 240; 
                                                    
                                                    //m_ardblParametrsTotalData [i] *= ((double) 359 / 200); 
                                                    m_ardblParametrsTotalData [i] *= ((double) 4310 / 2400); 
                                                }
                                                strATmp.free ();
                                            }
                                            
                                            if ((pOLTemplate->IsPlace ("station")) && (pOLTemplate->IsPosition ("81")))
                                            {//�����: 
                                                m_ardblParametrsTotalData [i] = ((HEnergData *) objData)->GetTeploVyvod ();
                                            }
                                            
                                            if ((pOLTemplate->IsPlace ("station")) && (pOLTemplate->IsPosition ("82")))
                                            {//�����: 
                                                m_ardblParametrsTotalData [i] = ((HEnergData *) objData)->GetTeploVyvodWithPodpitka ();
                                            }
                                        }
                                        else
                                            ;
                                    }
#endif
                                }
                                else
                                    ;// �� 'KTS'
                                    
                                //�� ����������� �. ���������� ��� ����������
                                //���� � ����������� '���.����', �� �������������� �������� ��������
                                //���� � ����������� '���.����', �� �������������� �������� ��������
                                /*
                                strAMeasure = ; //Mid (70, 3);
                                if (strAMeasure.GetAt (0) == -30 && strAMeasure.GetAt (1) == -21 && strAMeasure.GetAt (2) == -31)
                                {
                                    if (strTypeSource.CollateNoCase ("KTS_5_Teplo") == 0)
                                        //�����: 
                                        m_ardblParametrsTotalData [i] /= 1000;
                                    if (strTypeSource.CollateNoCase ("KTS_5_Elektro") == 0)
                                        //�������������: 
                                        m_ardblParametrsTotalData [i] /= 1000000;
                                }
                                */
                                
                                //������� ��˸��� ��������
                                if (m_ardblParametrsTotalData [i] == 0)
                                    m_iCountNullValue ++;
                                else
                                    ;
                                
                            }//������� �� ����� ������
#ifdef WM_USER_PROGRESS_RULES
                            //PostMessage (::AfxGetMainWnd ()->m_hWnd, WM_USER_PROGRESS_RULES, (WPARAM) (DYNAMIC_DOWNCAST (HThreadCreateTemplate, ::AfxGetThread ()))->m_nThreadID, 0);
							PostMessage (::AfxGetMainWnd ()->m_hWnd, WM_USER_PROGRESS_RULES, (WPARAM) (::AfxGetThread ())->m_nThreadID, 0);
#else
                            printf (".");
#endif
                        } //���� �� 'i'
                        
                        if (i < m_arptrOnceLineTemplate.GetSize ()) ; else iRes = 0; //Success
                    }
                    else
                        ; //���������� � ����������� ����� ������ ???
                }
                else {//������� ������� ����������� � �� ('iRes' �� ��� = -1)
                    FILELOG_IF_WRITE (HERROR)
                    strLog.format ("CalculateTemplate: �� ����������� ����� � ��!");
                    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
                    FILELOG_ENDIF
                }
                
#ifdef WM_USER_PROGRESS_RULES
#else
                printf ("\n���������� %i �����\n", i - 1);
#endif
            
                delete[] objData;
            }
            else
            {
                FILELOG_IF_WRITE (HERROR)
                strLog.format ("�� ������ ������ 'HData'");
                FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
                FILELOG_ENDIF
            }
        }
		else {
            if ((strDataBase.isUses () == 0) && (strTypeSource.isUses () == 0))
				iRes = ERR_PARSE_NULL;
			else
				iRes = ERR_ARGS; //(! (strDataBase.IsEmpty ())) && (! (strTypeSource.IsEmpty ())) = false
		}
    }
    else {//��� ������ ��� ������� (m_arptrOnceLineTemplate.GetSize () = 0)
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("��� ������ ��� �������: TEMPLATE=%i!", m_iIdTemplate);
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
        FILELOG_ENDIF
        
        iRes = ERR_CALC_NULL;
    }

    strATmp.free ();
    
    if (iRes == 0) {
		if (m_iCountNullValue > m_arptrOnceLineTemplate.GetSize () * 0.9) {//90% - ����� ��������� �������� ��� �����������
			FILELOG_IF_WRITE (HDEBUG)
			strLog.format ("CalculateTemplate () m_iCountNullValue = %i �� %i", m_iCountNullValue, m_arptrOnceLineTemplate.GetSize ());
			FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
			FILELOG_ENDIF

            iRes = ERR_NULL_VALUE;
        }
        else
            ;
    }
    else
        ;

	
    FILELOG_IF_WRITE (HDEBUG)
    strLog.format ("����� �� 'CalculateTemplate' � �����: %i", iRes);
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
    FILELOG_ENDIF

    return iRes;
}

int HContentTemplate::WriteContentTemplate (HString strAfullPath) //������ � ��������� ���� (���� ��� ��� ����������/���������)
{
    int i, iResponce = 0;
    HString strABufferOfFile; //, strATmp;
    FILELOG_VARIABLE;
    CFile fWrite;
    CFileException *err = 0;
    
    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
    if (strAfullPath.isUses () == 1)
        strLog.format ("���� � 'WriteContentTemplate' � ����������: %s", MB_HSTRING (strAfullPath));
    else
        strLog.format ("���� � 'WriteContentTemplate' � ������ ����������");

    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
    FILELOG_ENDIF
    
    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
    strLog.format ("����� ��� ������: %i", m_arstrAContent.GetSize ());
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
    FILELOG_ENDIF
    
    for (i = 0; i < m_arstrAContent.GetSize (); i++) {
        strABufferOfFile.append (m_arstrAContent.ElementAt (i));
        strABufferOfFile.appendChar ('\r');
        strABufferOfFile.appendChar ('\n');
    }
    
    strABufferOfFile.appendChar ('\0');
    
    if (strAfullPath.isUses () == 0) {
#ifndef _MBCS
        strAfullPath.format ("%s", MB_HSTRING (TargetFileName (true)));
#else
        strAfullPath.Format ("%s", TargetFileName (true));
#endif
    }
    else {
#ifndef _MBCS
        //strAfullPath.Format ("%s", _WC_TO_MB (m_strWildcard.Left (m_strWildcard.ReverseFind ('\\')));
        strAfullPath.append (TargetFileName (false));
#else
        //strAfullPath.Format ("%s", m_strWildcard.Left (m_strWildcard.ReverseFind ('\\')));
        strAfullPath.append (TargetFileName (false));
#endif
    }
    
    //fullPath.Append (TargetFileName (CTime::GetCurrentTime (), false));
    
    strAfullPath.lwr ();
    try { iResponce = fWrite.Open (WC_HSTRING (strAfullPath), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite, err); }
    catch (CFileException *err) {
    }
    
    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
    strLog.format ("��������� ���� ��� ������: %s", MB_HSTRING (strAfullPath));
	FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
	FILELOG_ENDIF
	
	/*
	if ((iResponce == 0) && (! (err == 0x0)))
	{
	    strATmp.Format ("������ �������� �����: %s", strAfullPath);
	    FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_1);
	    strATmp.Empty ();
    }
    else
    {
    */
    
    if (iResponce == 0 && (! (err == 0))) {
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("������ �������� �����: %s", MB_HSTRING (HString (err->m_strFileName)));
	    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
	    FILELOG_ENDIF
    }
    else {
        iResponce = 0;
    
        try { fWrite.SetLength (0); }
        catch (CFileException *err) {
            FILELOG_IF_WRITE (HERROR)
            strLog.format ("������ ��������� ����� ����� � 0: %s", MB_HSTRING (HString (err->m_strFileName)));
		    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
		    FILELOG_ENDIF
    		
		    iResponce = err->m_lOsError;
        }
        
        //�����  � ����� ������ ���� ������
        try { fWrite.Write (MB_HSTRING (strABufferOfFile), strABufferOfFile.length ()); }
        catch (CFileException *err) {
            FILELOG_IF_WRITE (HERROR)
            strLog.format ("������ ������ � ����: %s", MB_HSTRING (HString (err->m_strFileName)));
		    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
		    FILELOG_ENDIF
    		
		    iResponce = err->m_lOsError;
        }
                
        if (iResponce == 0) {
            try { fWrite.SetLength (strABufferOfFile.length ()); }
            catch (CFileException *err) {
                FILELOG_IF_WRITE (HERROR)
                strLog.format ("������ ��������� ����� ����� � %i: %s", strABufferOfFile.length (), MB_HSTRING (HString (err->m_strFileName)));
		        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
		        FILELOG_ENDIF
        		
		        iResponce = err->m_lOsError;
            }
        
            //����� ����������� ������������� �����
            SYSTEMTIME stmTmp;
            FILETIME ft_, ftlocal_;
    	    
	        //��� ����/����� (������., ������, �����. ���������)
            stmTmp = stmTargetLastWritten ();

            SystemTimeToFileTime (&stmTmp, &ftlocal_);
            
            //��-�� UTC
            LocalFileTimeToFileTime (&ftlocal_, &ft_);

            //�������� ���������			            
            try { iResponce = SetFileTime (fWrite.m_hFile, &ft_, &ft_, &ft_); }
            catch (CFileException *err) {
                FILELOG_IF_WRITE (HERROR)
                strLog.format ("������ ��������� ���������� ����/����� �����: %s", MB_HSTRING (HString (err->m_strFileName)));
		        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
		        FILELOG_ENDIF
        		
		        iResponce = err->m_lOsError;
            }
            
            if (iResponce == (int ) true)
                iResponce = 0;
            else
                ;
        }
        else
            ;
    }

    fWrite.Close ();

    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
    strLog.format ("������� ���� ����� ������: %s", MB_HSTRING (strAfullPath));
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
    FILELOG_ENDIF
    
    strABufferOfFile.free ();

    FILELOG_IF_WRITE (CONTENT_TEMPLATE)
    strLog.format ("����� �� 'WriteContentTemplate'");
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CONTENT_TEMPLATE);
    FILELOG_ENDIF
    
    return 0;
}

SYSTEMTIME HContentTemplate::stmTargetLastWritten (void) {
    SYSTEMTIME stmRes;
    CTimeSpan tmSpanInterval = GetIntervalVariable (m_tmTarget, HDateTimeInterval::DIRECTION_DATETIME::FUTURE);
    CTime tmLastWritten (m_tmTarget + tmSpanInterval);

    ////������ ��� �������� �� ����-���� 26.10.2014 02:00
    //if (! (tmSpanInterval.GetTotalSeconds () < (24 * 60 * 60)))
    //    tmLastWritten -= 60 * 60;
    //else
    //    ;

    //������� �1
    tmLastWritten.GetAsSystemTime (stmRes);

    if (tmSpanInterval.GetDays () > 0) {
        ////������ ��� �������� �� ����-���� 26.10.2014 02:00
        //if (m_tmSpanModify.GetHours () > 0)
        //    stmRes.wHour = (WORD ) m_tmSpanModify.GetHours () - 1;
        //else
            stmRes.wHour = (WORD ) m_tmSpanModify.GetHours ();
        stmRes.wMinute = (WORD ) m_tmSpanModify.GetMinutes ();
        stmRes.wSecond = (WORD ) m_tmSpanModify.GetSeconds ();
    }
    else {
        if (tmSpanInterval.GetHours () > 0) {
            stmRes.wMinute = (WORD ) m_tmSpanModify.GetMinutes ();
            stmRes.wSecond = (WORD ) m_tmSpanModify.GetSeconds ();
        }
        else {
            if (tmSpanInterval.GetMinutes () > 0) {
                stmRes.wSecond = (WORD ) m_tmSpanModify.GetSeconds ();
            }
            else
                ;
        }
    }

    ////������� �2
    //tmLastWritten += m_tmSpanModify;
    //tmLastWritten.GetAsSystemTime (stmRes);
    
    return stmRes;
}

HString HContentTemplate::TargetFileName (BOOL bFullPath) {
    HString strRes, strATmp;
    
    //������ ����������� ���������� ��� ������, �� �� ���� � ������� ???
    if (IsTemplate (m_strDirOutput))
        //������� �������� ������ - ����, ���, ����������
        strRes = HString (m_strDirOutput);
    else {
        //�� �������� ������
        if (! PathIsDirectory (m_strDirOutput.GetBuffer ())) {
            //�� ������ ����
            if (m_strDirOutput.find (0, '\\') > -1) {
                //��������� ���� (�������� �������� ���, ����������)
				HString strAppDirectory; WSA_APPDIRECTORY (strAppDirectory);
                strRes.append (strAppDirectory);
				if (! (strAppDirectory.codeAt (strAppDirectory.length ()) == '\\')) strAppDirectory.appendChar ('\\'); else ;
				strRes.append (m_strDirOutput);
#ifdef _DEBUG
                HString strDirDebug
#ifdef _MBCS
                    ("Debug");
#else
                    (_T ("Debug"));
#endif
                if (strRes.find (0, strDirDebug.GetBuffer ()) > -1)
                    strRes.cut (strRes.find (0, strDirDebug), strDirDebug.length () + 1);
#endif
            }
            else
                // ��� � 'm_strDirOutput' ??? - ��������� ���� (�������� �������� ���, ����������)
                strRes.append (m_strWildcard.right (m_strWildcard.reverseFind ('\\') + 1));
        }
        else
            strRes.append (m_strDirOutput);
            
        if (! (strRes.GetAt (strRes.length () - 1) == '\\'))
            strRes.appendChar ('\\');
        else
            ;
        
        //�������� �������� ���, ����������
        int posStart = m_strWildcard.reverseFind ('\\') + 1,
            countMid = m_strWildcard.reverseFind ('.') - (m_strWildcard.reverseFind ('\\') + 1) + 1;
        strRes.append (m_strWildcard.mid (posStart, countMid));
        
        strATmp.format ("%i", m_stHITTarget.iExt);
    
        while (strATmp.length () < 3)
            strATmp.insert (0, "0");
            
        strRes.append (strATmp);
        
        //strRes.Replace (HString ("*"), HString (strATmp));
        strATmp.free ();
    }

    if (! bFullPath)
        strRes = strRes.right (strRes.length () - strRes.reverseFind ('\\') - 1);
    else
        ;
    
    return strRes;
}
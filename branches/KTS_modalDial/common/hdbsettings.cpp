#include "stdafx.h"

#include <hdbsettings.h>
#include <hdatabasesql.h>

IMPLEMENT_DYNCREATE (HDBSettings, CObject)

EXT_DECLARE_HTHREAD_FILELOG

HString tokenizeTypeSource (int, HString &);
HString tokenizeConnectDB (int, HString &);

HDBSettings::HDBSettings (void) {
}

HDBSettings::~HDBSettings (void) {
}

HString HDBSettings::GetOption (LPCTSTR lpcNameParam) {
    return GetOption (HString (lpcNameParam));
}

HString HDBSettings::GetOption (HString &nameParam) {
    int iRes = 0;
    HString strRes, strQuery;
    FILELOG_VARIABLE
    CDBVariant dbvarData;
    
    CRecordset *pRS = DEBUG_NEW CRecordset (PTR_DB);
    if (pRS) {
        strQuery.format (_T ("SELECT ЗНАЧЕНИЕ FROM [%S]"), _T (TABLENAME_OPTION));
        pRS->m_strFilter.Format (_T ("ПАРАМЕТР='?'"));

        try { iRes = pRS->Open (CRecordset::dynaset, WC_HSTRING (strQuery)); }
        catch (CDBException *err) {
            FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), HERROR);
            iRes = -1;

			err->Delete ();
        }

        if (iRes == TRUE) {
            pRS->m_strFilter.Format (_T ("ПАРАМЕТР='%s'"), nameParam.GetBuffer ());
            
            try { iRes = pRS->Requery (); }
            catch (CDBException *err) {
                //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
                FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), HERROR);
				err->Delete ();
            }

            if (iRes == TRUE) {
                pRS->GetFieldValue (_T ("ЗНАЧЕНИЕ"), dbvarData);
                strRes = HString ((*dbvarData.m_pstring).GetBuffer ());
            }
            else
                ;
        }
        else
            ;
    }
    else {
        strLog.format (_T ("Не инициализорован RS 'Option' для значения %S"), WC_HSTRING (nameParam));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), COMMON_KTS_APP);
        strLog.free ();
    }

    FreeMemoryDBObject ((CObject *&) pRS);

    return strRes;
}

int HDBSettings::GetIdType (int idSource) {
    CString strQuery, strData;
    CDBVariant dbvarData;
    FILELOG_VARIABLE
    int iRes = -1;

    if (PTR_DB) {
        CRecordset *pRS = NULL;
        pRS = DEBUG_NEW CRecordset (PTR_DB);

        if (pRS) {
            strQuery.Format (_T ("SELECT ID_TYPE FROM Source WHERE ID = %i"), idSource);

            try { pRS->Open (AFX_DB_USE_DEFAULT_TYPE, strQuery); }
            catch (CDBException *err) {
                //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
                FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), COMMON_KTS_APP);
				err->Delete ();
            }

            if (pRS->IsOpen ()) {
                if (pRS->GetRowsetSize ()) {
                    //try { pRS->MoveFirst (); }
                    try { pRS->Move (1, SQL_FETCH_FIRST); }
                    catch (CDBException *err) {
                        //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
                        FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), COMMON_KTS_APP);
						err->Delete ();
                    }
                    while (! pRS->IsEOF ()) {
                        //ВНИМАНие! ТОЛЬКо ОДНа записЬ
                        pRS->GetFieldValue (_T ("ID_TYPE"), dbvarData);
                        if (dbvarData.m_dwType == 4)
                            ;
                        else
                            ;

                        iRes = dbvarData.m_iVal;

                        pRS->MoveNext ();
                    }
                }
                else
                    ; //Нет СТРОК ???
            }
            else
                ; //RS НЕ ОТКРыт 

            FreeMemoryDBObject ((CObject *&) pRS);
        }
        else
            ; //Не создан RS
    }
    else
        ; //Нет соединения с БД

    return iRes;
}

HDBSettings::TYPE_DB HDBSettings::GetTypeDB (int idSource) {
    CString strQuery, strData;
    CDBVariant dbvarData;
    FILELOG_VARIABLE
    TYPE_DB enmTypeBDRes = SQL_DB;

    if (PTR_DB) {
        CRecordset *pRS = NULL;
        pRS = DEBUG_NEW CRecordset (PTR_DB);

        if (pRS) {
            strQuery.Format (_T ("SELECT NETWORK FROM Source WHERE ID = %i"), idSource);

            try { pRS->Open (AFX_DB_USE_DEFAULT_TYPE, strQuery); }
            catch (CDBException *err) {
                //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
                FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), COMMON_KTS_APP);
				err->Delete ();
            }

            if (pRS->IsOpen ()) {
                if (pRS->GetRowsetSize ()) {
                    //try { pRS->MoveFirst (); }
                    try { pRS->Move (1, SQL_FETCH_FIRST); }
                    catch (CDBException *err) {
                        //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
                        FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), COMMON_KTS_APP);
						err->Delete ();
                    }
                    while (! pRS->IsEOF ()) {
                        //ВНИМАНие! ТОЛЬКо ОДНа записЬ
                        pRS->GetFieldValue (_T ("NETWORK"), dbvarData);
                        if (dbvarData.m_pstring->CollateNoCase (_T ("EMPTY")) == 0)
                            enmTypeBDRes = FILES_DB;
                        else
                            ;

                        pRS->MoveNext ();
                    }
                }
                else
                    ; //Нет СТРОК ???
            }
            else
                ; //RS НЕ ОТКРыт 
                
            FreeMemoryDBObject ((CObject *&) pRS);
        }
        else
            ; //Не создан RS
    }
    else
        ; //Нет соединения с БД

    return enmTypeBDRes;
}

//Взять для источника с 'idTypeSource' значение поля 'strNameField'
HString HDBSettings::GetSourceField (int idTypeSource, HString &strNameField) {
    HString strQuery, strData, strRes;
    FILELOG_VARIABLE
    CDBVariant dbvarData;

    if (PTR_DB) {
        CRecordset rs (PTR_DB);

        strQuery.format (_T ("SELECT DISTINCT %S FROM Source WHERE ID_TYPE = %i"), WC_HSTRING (strNameField), idTypeSource);
        try { rs.Open (AFX_DB_USE_DEFAULT_TYPE, strQuery.GetBuffer ()); }
        catch (CDBException *err)  {
            //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
            FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), COMMON_KTS_APP);
			err->Delete ();
        }

        //rs.MoveFirst ();

        while (! rs.IsEOF ())
        {//Только ОДНа ЗАПИСь
            rs.GetFieldValue (strNameField.GetBuffer (), dbvarData);
            if (! (dbvarData.m_pstring->IsEmpty ()))
                strData = *dbvarData.m_pstring;

            rs.MoveNext ();
        }

        rs.Close ();
    }
    else
        ;

    return strData;
}

HString HDBSettings::CompileSourceField (int idTypeSource, HString &strNameField, int typesDB) {
    BOOL bRes = true;
    int i = -1, j = -1;
    FILELOG_VARIABLE

    CArray <int> ariID, ariIDAlternative;
    HString strQuery, strTemp, strRes, arstrIDAlternative, strData;
    CDBVariant dbvarData;
    //TYPE_DB enmTypeDB;

    if (PTR_DB) {
        CRecordset rs (PTR_DB);

        //strQuery.Format (_T ("SELECT DISTINCT %s FROM Source WHERE ID_TYPE = %i"), strNameField.GetBuffer (), idTypeSource);
        strQuery.format (_T ("SELECT %S, ID, ID_ALTERNATIVE FROM Source WHERE ID_TYPE = %i"), WC_HSTRING (strNameField), idTypeSource);
        try { bRes = rs.Open (AFX_DB_USE_DEFAULT_TYPE, strQuery.GetBuffer ()); }
        catch (CDBException *err) {
            //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
            FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), COMMON_KTS_APP);
			err->Delete ();
        }

        if (bRes == true) {
            //if (rs.CanScroll ()) rs.MoveFirst (); else ;

            while (! rs.IsEOF ()) {
                rs.GetFieldValue (_T ("ID"), dbvarData);
			    if (GetTypeDB (dbvarData.m_iVal) & typesDB) {
				    ariID.Add (dbvarData.m_iVal);
				    rs.GetFieldValue (_T ("ID_ALTERNATIVE"), dbvarData);
				    if (! (dbvarData.m_pstring->CollateNoCase (_T ("EMPTY")) == 0))
					    ariIDAlternative.Add (atoi (_WC_TO_MB (dbvarData.m_pstring->GetBuffer ())));
				    else
					    ;

				    rs.GetFieldValue (strNameField.GetBuffer (), dbvarData);
				    //ТАКОЙ ЖЕ КОД ниЖЕ
				    if (! (dbvarData.m_pstring->IsEmpty ()))
					    if (! (dbvarData.m_pstring->CollateNoCase (_T ("EMPTY")) == 0))	{
						    if (strNameField.compareNoCase (_T ("DATABASE")) == 0) {
                                //Сначала добавить путь к БД, если ОНА файловая (KTS_TEPLO(ELEKTRO)_5)
							    if (GetTypeDB (ariID.GetAt (ariID.GetSize () - 1)) == FILES_DB)	{
                                    //Файловая БД
								    strData = GetItemFullPath (idTypeSource);
								    if (! (strData.GetAt (strData.length () - 1) == '\\'))
									    strData.appendChar ('\\');
								    else
									    ;

								    strRes.append (strData);
							    }
							    else
								    ;
						    }
						    else
							    ; //Не файловая БД

						    strRes.append ((*dbvarData.m_pstring).GetBuffer ());
					    }
					    else
						    ; //???
				    else
					    ; //???
			    }
			    else
				    ; //БД не совпадает с требуемым ТИПом БД 'typesDB'

                rs.MoveNext ();

                if (! rs.IsEOF ())
                    strRes.appendChar (',');
			    else
				    ;
            }

            strQuery.free ();
            rs.Close ();
        }
        else
            ; //rs.Open == FALSE

        i = 0;
        while (i < ariID.GetSize ()) {
            j = 0;
            while (j < ariIDAlternative.GetSize ()) {
                if ((ariID [i] == ariIDAlternative [j]) || (! (GetTypeDB (ariIDAlternative [j]) & typesDB)))
                    ariIDAlternative.RemoveAt (j);
                else
                    j ++;
            }

            i ++;
        }

        j = 0;
        if (ariIDAlternative.GetSize ()) {
            strRes.appendChar (',');
            
            arstrIDAlternative.appendChar ('(');
            while (j < ariIDAlternative.GetSize ()) {
                strTemp.format (_T ("%i"), ariIDAlternative [j]);
                arstrIDAlternative.append (strTemp);

                j ++;
                if (j < ariIDAlternative.GetSize ())
                    arstrIDAlternative.appendChar (',');
                else
                    ;
            }
            arstrIDAlternative.appendChar (')');

            strQuery.format (_T ("SELECT %S FROM Source WHERE ID IN %S"), WC_HSTRING (strNameField), WC_HSTRING (arstrIDAlternative));
            try { rs.Open (AFX_DB_USE_DEFAULT_TYPE, strQuery.GetBuffer ()); }
            catch (CDBException *err) {
                //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
                FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), COMMON_KTS_APP);
				err->Delete ();
            }

            /*if ((! rs.IsEOF ()) && (! rs.IsBOF ()))
            {*/
     //           try { rs.MoveFirst (); }
     //           catch (CDBException *err) {
     //               //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
     //               FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), COMMON_KTS_APP);
					//err->Delete ();
     //           }

                j = 0;
                //while (j < arstrIDAlternative.GetSize ())
                while (! rs.IsEOF ()) {
                    rs.GetFieldValue (strNameField.GetBuffer (), dbvarData);

                    //ТАКОЙ ЖЕ КОД ВЫше
                    if (! (dbvarData.m_pstring->IsEmpty ()))
                        if (! (dbvarData.m_pstring->CollateNoCase (_T ("EMPTY")) == 0)) {
                            if (strNameField.compareNoCase (_T ("DATABASE")) == 0)
                            {//Сначала добавить путь к БД, если ОНА файловая (KTS_TEPLO(ELEKTRO)_5)
                                if (GetTypeDB (ariIDAlternative [j]) == FILES_DB) {
                                    //Файловая БД
                                    strData = GetItemFullPath (GetIdType (ariIDAlternative [j])); //idTypeSource);
                                    if (! (strData.GetAt (strData.length () - 1) == '\\'))
                                        strData.appendChar ('\\');
                                    else
                                        ;

                                    strRes.append (strData);
                                }
                                else
                                    ;
                            }
                            else
                                ; //Не файловая БД
                            
                            strRes.append ((*dbvarData.m_pstring).GetBuffer ());
                        }
                        else
                            ; //???
                    else
                        ; //???

                    rs.MoveNext ();
                    j ++;

                    if (! rs.IsEOF ())
                        strRes.appendChar (',');
                    else
                        ;
                }
            //}
            //else
            //    ; //НЕТ записей ???

            strQuery.free ();
            rs.Close ();
        }
        else
            ; //НЕТ записей для добавления

        return strRes;
    }
    else
        return HString (""); //Нет соединения с БД
}

HString HDBSettings::CompileTypeSource (int idTypeSource, int typesDB) {
   return CompileSourceField (idTypeSource, HString (_T ("TYPE")), typesDB);
}

HString HDBSettings::CompileConnectDB (int idTypeSource, int typesDB) {
    //TYPE_DB bTypeDB = SQL_DB;

    int i;
    HString strRes;
    HString strNameField;

    if (idTypeSource > 0) {
        //char *arstrNameFields [] = {"SERVER", "DATABASE", "UID", "PASSWORD", ""};
        i = 0;
        while (! (HDatabaseSQL::GetParametrConnectDB (i) == NULL)) {    
            strNameField = CString (HDatabaseSQL::GetParametrConnectDB (i));
            strRes.append (strNameField);
            strRes.appendChar ('=');

            strRes.append (CompileSourceField (idTypeSource, strNameField, typesDB)); //bTypeDB передаЁтся ТОЛЬКо для поля 'DATABASE' - в ОСТАЛЬНых случаях ЛИШНий аргумент

            strNameField.free ();

            if (! (HDatabaseSQL::GetParametrConnectDB (++ i) == NULL))
                strRes.appendChar (';');
            else
                ;
        }
    }
    else
        ;

    return strRes;
}

HString HDBSettings::CompileConnectDB (HString &strTypeSource, int typesDB) {
//НЕ РЕАЛИЗОВАНА ???
    HString strRes;

    BOOL bRes = false;
    int iResSQL = 0;
    CString strAlternativeSource, strQuery;
    FILELOG_VARIABLE
    
    if (PTR_DB) {
        CRecordset rs (PTR_DB);
        
        strQuery.Format (_T ("SELECT ID FROM 0 WHERE 0=%i"), 0);
        try { iResSQL = rs.Open (AFX_DB_USE_DEFAULT_TYPE, strQuery); }
        catch (CDBException *err) {
            //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
            FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), HERROR);
			err->Delete ();
        }
        
        if (iResSQL) {
        }
        else
            ;
    }
    else
        ;
        
    return strRes;
}

BOOL HDBSettings::IsAlternativeSource (int idTypeSource) {
    BOOL bRes = false;
    int iResSQL = 0;
    CString strAlternativeSource, strQuery;
    FILELOG_VARIABLE
    
    if (PTR_DB) {
        CRecordset rs (PTR_DB);
        
        strQuery.Format (_T ("SELECT ID_ALTERNATIVE FROM Source WHERE ID_TYPE = %i"), idTypeSource);
        try { iResSQL = rs.Open (AFX_DB_USE_DEFAULT_TYPE, strQuery); }
        catch (CDBException *err) {
            //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
            FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), HERROR);
			err->Delete ();
        }
        
        if (iResSQL) {
            //try { rs.MoveFirst (); }
            //catch (CDBException *err)
            //{
            //    //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
            //    FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), COMMON_KTS_APP);
			//	  err->Delete ();
            //}
        
            while (! rs.IsEOF ()) {
                rs.GetFieldValue (_T ("ID_ALTERNATIVE"), strAlternativeSource);
                
                rs.MoveNext ();
            }
            
            if (! (strAlternativeSource.CollateNoCase (_T ("EMPTY")) == 0))
                if (! (strAlternativeSource.CollateNoCase (_T ("NULL")) == 0))
                    bRes = true;
                else
                    ;
            else
                ;
        }
        else
            ;
        rs.Close ();
    }
    else
        ;
    
    return bRes;
}

//Полный путь элемента из 'Template'
//дубликат (аналог) TreeCtrlTemplate::GetItemFullPath
HString HDBSettings::GetItemFullPath (int id) {
	HString strResFullPath, itemPath;
    CString strQuery;
    int itemId = -1, iResSQL = -1;
    FILELOG_VARIABLE
    
    if (PTR_DB) {
        HRecordset *pRS = 0x0;
        //pRS = PTR_DB->GetRecordset (this, HString (_T (TABLENAME_TEMPLATE)));
        pRS = DEBUG_NEW HRecordset (PTR_DB);
        
        itemId = id;
        while (itemId) {
            strQuery.Format (_T ("SELECT ID_OWNER, ITEM_PATH FROM [%s] WHERE ID = %i"), _T (TABLENAME_TEMPLATE), itemId);
            try { iResSQL = pRS->Open (AFX_DB_USE_DEFAULT_TYPE, strQuery); }
            catch (CDBException *err) {
                //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION); 	PseudoKTS.exe!HDBSettings::CompileSourceField(int idTypeSource, HString & strNameField, int typesDB)  Строка 347 + 0x2a байт	C++

                FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), HERROR);
				err->Delete ();
            }
        
            if (iResSQL) {
                while (! pRS->IsEOF ()) {
                    pRS->GetFieldValue (_T ("ID_OWNER"), itemId);
                    pRS->GetFieldValue (_T ("ITEM_PATH"), itemPath);
                
                    pRS->MoveNext ();
                }

                if ((! (itemPath.compareNoCase (_T ("EMPTY")) == 0)) && (! (itemPath.compareNoCase (_T ("NULL")) == 0))) {
                    strResFullPath = itemPath + HString (_T ("\\")) + strResFullPath;

                    /*strResFullPath.insert (0, itemPath);
                    strResFullPath.appendChar ('\\');*/
                }
                else
                    ;
            }
            else
                ;

            try { pRS->Close (); }
            catch (CDBException *err) {
                //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
                FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), HERROR);
				err->Delete ();
            }
        }

        if (MODE_APP & HWinAppKTS::MODE_LOCAL)
            strResFullPath = GetOption (_T ("NetSharedSource")) + HString (_T ("\\")) + strResFullPath;
        else
            if (MODE_APP & HWinAppKTS::MODE_REMOTE)
                strResFullPath = GetOption (_T ("NetSharedName")) + HString (_T ("\\")) + strResFullPath;
            else
                ;

        FreeMemoryDBObject ((CObject *&) pRS);
    }
    else
        ;
	
	return strResFullPath;
}

HString tokenizeTypeSource (int iNumAlternative, HString &strTypeSource) {
    int iPos = 0;
    HString strRes;
    //if (strTypeSource.IsEmpty ())
    //    stringData (GetDataOfId (STRING_TYPESOURCE));
    
    if ((iPos > -1) && (iNumAlternative > -1)) {
        iPos = 0;
        while ((iPos > -1) && (iNumAlternative > -1)) {
            strRes = strTypeSource.tokenize (_T (","), iPos);
            iNumAlternative --;
        }
    }
    else
        ;
    
    return strRes;
}

HString tokenizeConnectDB (int iNumAlternative, HString &strConnectDB) {
#if defined _HTHREAD_FILELOG
	FILELOG_VARIABLE
#else
#endif
	int iPosParametr = -1, iPos = -1, iNumAlternativeTemp = -1, iCountParametr = 0;
    //char *arstrNameFields [] = {"SERVER", "DATABASE", "UID", "PASSWORD", ""};
    HString strRes, strResTemp, strParametr;

    if (iNumAlternative > -1) {
#if defined _HTHREAD_FILELOG
        //strATemp.Format (_T ("Вход в '%s::ParseConnectDB' с аргументом: %i"), GetRuntimeClass ()->m_lpszClassName, iNumAlternative);
        strLog.format ("Вход в 'tokenizeConnectDB' с аргументом: %i", iNumAlternative);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_COMMON);
#else
#endif
        iPos = 0;
        iPosParametr = 0;
        strParametr = strConnectDB.tokenize (_T (";"), iPosParametr);
        //while (! (strlen (arstrNameFields [i]) == 0))
        while (strParametr.isUses () == 1) {
            iPos = strParametr.find (0, '=');
            //Обрезаем по следуеЩЕМу за '=' символУ
            iPos ++;
            strRes.append (strParametr.left (iPos));
            strParametr = strParametr.right (strParametr.length () - iPos);
            
            iPos = 0;
            iNumAlternativeTemp = iNumAlternative;
            while ((iPos > -1) && (iNumAlternativeTemp > -1)) {
	            strResTemp.free ();
	            if (iPos < strParametr.length ())
	                if (! (strParametr.GetAt (iPos) == ','))
	                    strResTemp = strParametr.tokenize (_T (","), iPos);
                    else
                        iPos ++;
                else
                    strResTemp.free ();
                    
	            iNumAlternativeTemp --;
	        }
#if defined _HTHREAD_FILELOG
            strLog.format ("'tokenizeConnectDB...' присоединяю к резульТатУ: %s", MB_HSTRING (strResTemp));
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_COMMON);
#else
#endif
			if (strResTemp.isUses () == 1)
				iCountParametr ++;
			else
				;
			
			strRes.append (strResTemp);
    	    
	        strParametr = strConnectDB.tokenize (_T (";"), iPosParametr);
    	    
	        if (strParametr.isUses () == 1)
	            strRes.appendChar (';');
            else
                ;
        }
    }
    else
        ;

	if (iCountParametr == 0)
		strRes.free ();
	else
		;
#if defined _HTHREAD_FILELOG
    strLog.format ("Выход из 'tokenizeConnectDB' с рез-ом: %s, число параметров = %i", MB_HSTRING (strRes), iCountParametr);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_COMMON);
#else
#endif
    return strRes;
}
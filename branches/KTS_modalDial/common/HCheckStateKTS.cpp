#include "StdAfx.h"
#include "HCheckStateKTS.h"

//#include <vector>
//using namespace std;

EXT_DECLARE_HTHREAD_FILELOG

IMPLEMENT_DYNAMIC (HCheckStateKTS, HObjectTemplate)

HCheckStateKTS::HCheckStateKTS (int id, HTREEITEM hItem, HString strWildcard, CTimeSpan tmSpanModify, CTimeSpan tmSpanTask, CTimeSpan tmSpanWork, int idInterval, int iMinExt, int iMaxExt, int iCountPassErrorCreate, BOOL bFullInfo) : HObjectTemplate (id, hItem, strWildcard, tmSpanModify, tmSpanTask, tmSpanWork, idInterval, iMinExt, iMaxExt, bFullInfo) {
    //HObjectTemplate::HObjectTemplate (id, hItem, strWildcard, tmSpanModify, tmSpanTask, tmSpanWork,tmSpanInterval, iMinExt, iMaxExt,  bFullInfo);
    //CStringA strTemp;
    //Почему не сохраняются значения при вызове конструктора ???
	//HCheckStateKTS ();
	
	FILELOG_VARIABLE
	FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
	strLog.format ("Конструктор 'HCheckStateKTS' - %s", MB_HSTRING (strWildcard));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), CHECK_STATE_TEMPLATE);
    FILELOG_ENDIF
	
	//ОСТАЛьные переменные инициализируются в БАЗовом конструкторе ???
	m_iState = STATE_ITEM_NODATA;
	
	//m_evCreateTemplate.SetEvent ();
	m_hEvtOperation = CreateEvent (NULL, true, true, NULL);

	/*for (ID_OPERATION curId = ID_OPERATION::ID_OPERATION_UPDATEDATA; curId < ID_OPERATION::ID_LAST_OPERATION; curId ++)
	    m_arhEvtOperation [curId - PREFIX_IDM_OPERATION] = CreateEvent (NULL, true, true, NULL);*/
	    
    m_iCountPassErrorCreate = iCountPassErrorCreate;
    m_iCountPassError = 0;
}

HCheckStateKTS::~HCheckStateKTS (void) {
    FILELOG_VARIABLE

	HString strFullPathTemplate = GetFullPathTemplate ();

    FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
    strLog.format (_T ("'WaitFor...m_hEvtOperation' для '~HCheckStateKTS' %S"), WC_HSTRING (strFullPathTemplate));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), CHECK_STATE_TEMPLATE);
    FILELOG_ENDIF
    
    //switch (WaitForSingleObject (m_hEvtOperation, 6666))
	switch (WaitForSingleObject (m_hEvtOperation, INFINITE)) {
		case WAIT_ABANDONED:
		case WAIT_TIMEOUT:
		    FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
		    strLog.format ("Запрещено РАЗРУШЕНИЕ для %s", MB_HSTRING (strFullPathTemplate));
		    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), CHECK_STATE_TEMPLATE);
		    FILELOG_ENDIF
			break;
		case WAIT_OBJECT_0:
		    break;
		default: //Все остальные !!!
			;
	}
	
	/*for (ID_OPERATION curId = ID_OPERATION::ID_OPERATION_UPDATEDATA; curId < ID_OPERATION::ID_LAST_OPERATION; curId ++)
	    CloseHandle (m_arhEvtOperation [curId - PREFIX_IDM_OPERATION]);*/
	CLOSE_HANDLE (m_hEvtOperation);
    
    m_arInfoTemplate.RemoveAll ();
    m_arInfoSample.RemoveAll ();
}

void HCheckStateKTS::UpdateVariables (void) {
    m_iState = STATE_ITEM_NODATA;
	
	m_iIdTemplate = -1;
	m_hTreeItem = 0;
	
	m_tmSpanModify = 0;
	m_tmSpanTask = 0;
	m_tmSpanWork = 0;
	//m_tmSpanIntervalDefinition = 0;
	m_iIdInterval = -1;
	
	m_iMinNumberExt = 0;
	m_iMaxNumberExt = 0;
	
	m_tmLastUpdate = 0;
	
	m_iFirstExtError = 0;
    m_iLastExtError = 0;
    
    m_iCountFileFind = 0;
	
	if (m_pHFIFirst)
	    delete m_pHFIFirst;
	
	if (m_pHFILast)
	    delete m_pHFILast;
	    
    m_pHFIFirst = NULL;
    m_pHFILast = NULL;
	
	//m_stHITFirst.iExt = 0;
	//m_stHITLast.iExt = -1;
	
	//m_stHITFirst.tmLastWrite = CTime::GetCurrentTime ();
	//m_stHITLast.tmLastWrite = 0;
}

void HCheckStateKTS::SetState (int n) {
    FILELOG_VARIABLE

	int i, iState = SetStateItem (n), iStateTemp = STATE_ITEM_OK;

	if (m_iState > iState)
	    //Проверка на ухуджение - по ОДНОМу элементу 
		m_iState = iState;
	else {
        //Попытка 'ПОДНЯТЬ' (улучшить) состояние - ПРОВЕРЯТЬ ВСЕ элементы
	    for (i = 0; i < m_arInfoSample.GetSize (); i ++) {
	        if (m_arInfoSample.GetAt (i).iState < iStateTemp)
	            iStateTemp = m_arInfoSample.GetAt (i).iState;
            else
                ;
	    }
	    
	    if (m_iState < iStateTemp)
	        m_iState = iStateTemp;
        else
            ;
	}
}

int HCheckStateKTS::GetStateItem (int n) {
	if (m_arInfoSample.GetSize ())
		return m_arInfoSample.GetAt (n).iState;
	else
		return STATE_ITEM_NODATA;
}

int HCheckStateKTS::SetStateItem (int n) {
    FILELOG_VARIABLE

    /*
	if ((it.tmLastWrite.GetDay () == is.tmLastWrite.GetDay ()) &&
		(it.tmLastWrite.GetMonth () == is.tmLastWrite.GetMonth ()) &&
		(it.tmLastWrite.GetYear () == is.tmLastWrite.GetYear ()) &&
		(it.tmLastWrite.GetHour () == is.tmLastWrite.GetHour ()) &&
		(it.tmLastWrite.GetMinute () == is.tmLastWrite.GetMinute ()))
    */
	CTimeSpan tmSpanDiff;
	//CTimeSpan tmSpanDiff = m_arInfoSample.GetAt (n).tmLastWrite - m_arInfoTemplate.GetAt (n).tmLastWrite;
	//if (m_arInfoTemplate.GetAt (n).tmLastWrite.GetMinute () % 5 == 0)
	/*if (m_tmSpanInterval.GetTotalSeconds () == 300)
	    tmSpanDiff = m_arInfoSample.GetAt (n).tmLastWrite - resetDigitTensSecond (m_arInfoTemplate.GetAt (n).tmLastWrite);
	else
	    tmSpanDiff = m_arInfoSample.GetAt (n).tmLastWrite - resetDigitUnitsMinute (m_arInfoTemplate.GetAt (n).tmLastWrite);*/

    /*strLog.format ("Время: 'Sample' = %i, 'Template' = %i", m_arInfoSample.GetAt (n).tmLastWrite, m_arInfoTemplate.GetAt (n).tmLastWrite);
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);*/

    SYSTEMTIME stmInfoSample, stmInfoTemplate;
    m_arInfoSample.GetAt (n).tmLastWrite.GetAsSystemTime (stmInfoSample);
    m_arInfoTemplate.GetAt (n).tmLastWrite.GetAsSystemTime (stmInfoTemplate);

    tmSpanDiff = m_arInfoSample.GetAt (n).tmLastWrite - resetDigitTensSecond (m_arInfoTemplate.GetAt (n).tmLastWrite);

    if (((stmInfoSample.wYear == 2014) && (stmInfoSample.wMonth == 10) && (stmInfoSample.wDay == 26) &&
        (stmInfoSample.wHour == 3) && (stmInfoSample.wMinute == 0) && (stmInfoSample.wSecond == 0)) &&
        ((stmInfoTemplate.wYear == 2014) && (stmInfoTemplate.wMonth == 10) && (stmInfoTemplate.wDay == 26) &&
        (stmInfoTemplate.wHour == 3) && (stmInfoTemplate.wMinute == 0) && (stmInfoTemplate.wSecond == 0)) &&
        (! (tmSpanDiff == 0x0))) {
        stmInfoSample.wHour = 3;
    }
    else
        ;

	//long lTemp = tmSpanDiff.GetTotalMinutes ();
    if (abs ((long) tmSpanDiff.GetTotalSeconds ()) < 66)
    //if ((abs ((long) tmSpanDiff.GetTotalSeconds ())) == 0)
		m_arInfoSample.GetAt (n).iState = STATE_ITEM_OK;
	else {
		m_arInfoSample.GetAt (n).iState = STATE_ITEM_FAILED;
	}

	return m_arInfoSample.GetAt (n).iState;
}

int HCheckStateKTS::GetState (void) {
	return m_iState;
}

int HCheckStateKTS::GetCountError (void) {
    int i = -1, iError = 0, szInfoSample = m_arInfoSample.GetSize ();

    if (szInfoSample) {
	    for (i = 0; i < szInfoSample; i ++)	{
	        if (m_arInfoSample [i].iState < STATE_ITEM_OK)
	            iError ++;
	        else
                ;
	    }
    	
	    if (iError == 0) {
	        HInfoSample is;
	        CTime tmTemp = CTime::GetCurrentTime ();
            SetTarget (&is, tmTemp, true);
            //SetTarget (&is, tmTmp);

            if (! (m_arInfoSample [0].iExt == is.iExt)) {
                m_arInfoSample [0].iState = STATE_ITEM_FAILED;
                iError ++;
            }
            else
                ;
	    }
	    else
	        ;
    }
    else
        ;

	return iError;
}

BOOL HCheckStateKTS::Update (void) {
#if defined _DEBUG
    CMemoryState memStateBegin, memStateEnd, memStateDiff;
    memStateBegin.Checkpoint ();
#endif

	BOOL bRes = false;
	FILELOG_VARIABLE
	CTimeSpan tmSpanInterval = GetIntervalDeclarative ();
	
	/*strLog.Format (_T ("'WaitFor...m_hEvtOperation' для '~HCheckStateKTS::Update'"), GetFullPathTemplate ());
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_3);*/
	
	switch (WaitForSingleObject (m_hEvtOperation, 0)) {
	//switch (WaitForMultipleObjects (COUNT_OPERATION, m_arhEvtOperation, true, 0)) {
		case WAIT_ABANDONED:
		case WAIT_TIMEOUT:
		    ; //strLog.format ("Запрещен 'Update' для %S", GetFullPathTemplate ().GetBuffer ());
			break;
		case WAIT_OBJECT_0:
			//if (m_tmLastUpdate - m_arInfoSample.GetAt (0).tmLastWrite > m_tmSpanInterval)
	        if ((CTime::GetCurrentTime () - m_tmLastUpdate) > tmSpanInterval) {
                //Начинаем с '0'
#if defined _DEBUG
    /*memStateEnd.Checkpoint ();
    if (memStateDiff.Difference (memStateBegin, memStateEnd) && (memStateDiff.m_lTotalCount > 4096)) {
        TRACE (_T ("HCheckStateKTS::Update () - WAIT_OBJECT_0 - MemoryLeaked!\n"));
        memStateDiff.DumpStatistics ();
    }
    else
        ;*/
#endif

                //m_strWildcard.ReleaseBuffer ();
                m_arInfoTemplate.RemoveAll ();
                m_arInfoSample.RemoveAll ();
                
#if defined _DEBUG
    /*memStateEnd.Checkpoint ();
    if (memStateDiff.Difference (memStateBegin, memStateEnd) && memStateDiff.m_lTotalCount > 4096) {
        TRACE (_T ("HCheckStateKTS::Updete () - RemoveAll () - MemoryLeaked!\n"));
        memStateDiff.DumpStatistics ();
    }
    else
        ;*/
#endif

                m_iFirstExtError = 0, m_iLastExtError = 0;
                m_iCountFileFind = 0;

                if (SetInfo ())
                    SetCheck ();
                else
                    ;

#if defined _DEBUG
    /*memStateEnd.Checkpoint ();
    if (memStateDiff.Difference (memStateBegin, memStateEnd) && memStateDiff.m_lTotalCount > 4096) {
        TRACE (_T ("HCheckStateKTS::Update () - SetInfo(Check) - MemoryLeaked!\n"));
        memStateDiff.DumpStatistics ();
    }
    else
        ;*/
#endif

		        //m_tmLastUpdate = CTime::GetCurrentTime ();
		        
		        FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
		        strLog.format ("Выполнен 'Update' для %s c ID=%i и интервалом создания файла=%i", MB_HSTRING (GetFullPathTemplate ()), GetID (), tmSpanInterval.GetTotalSeconds ());
		        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CHECK_STATE_TEMPLATE);
		        FILELOG_ENDIF
		        
		        bRes = true;
	        }
	        else
	            ;
            break;
        default:
            ;
	}
	
	strLog.free ();
	
#if defined _DEBUG
    /*memStateEnd.Checkpoint ();
    if (memStateDiff.Difference (memStateBegin, memStateEnd) && memStateDiff.m_lTotalCount > 4096) {
        TRACE (_T ("HCheckStateKTS::Updete () - Выход - MemoryLeaked!\n"));
        memStateDiff.DumpStatistics ();
    }
    else
        ;*/
#endif

	return bRes;
}

//ОбновлЁн ли ШАБЛОН ???
BOOL HCheckStateKTS::IsUpdate (void) {
    BOOL bRes = true;

    if (m_iIdInterval > 0) {
	    HString arStrPermissionTemplate [] = {"запреЩЁн", "разреШЁн", "не оПределён"};
        FILELOG_VARIABLE
        CTime tmTemp, tmEntry = CTime::GetCurrentTime (), tmExit;
        CTimeSpan tmSpanInterval = GetIntervalDeclarative ();
        int cError = GetCountError (), iPermissionTemplate = 2;

		if (tmSpanInterval < 0) {
			tmSpanInterval = GetIntervalVariable (tmEntry, HDateTimeInterval::DIRECTION_DATETIME::PAST);
		}
		else ;
    
        /*strLog.Format (_T ("Вход в 'HCheckStateKTS::IsUpdate' для ID=%i"), GetID ());
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_3);*/
    
        switch (WaitForSingleObject (m_hEvtOperation, 0x0)) {
        //switch (WaitForMultipleObjects (COUNT_OPERATION, m_arhEvtOperation, true, 0)) {
		    case WAIT_ABANDONED:
		    case WAIT_TIMEOUT:
			    iPermissionTemplate = 0;
			    break;
		    case WAIT_OBJECT_0:
		        if (m_arInfoSample.GetSize ()) {
				    iPermissionTemplate = 1;
                
                    tmTemp = CTime::GetCurrentTime ();

					strLog.format ("HCheckStateKTS::IsUpdate: ID=%i, разница = %i", GetID (), (tmTemp - m_tmLastUpdate).GetTotalSeconds ());
					FILELOG_WRITE_WITHDT (strLog.GetBuffer (), /*HDEBUG*/CHECK_STATE_TEMPLATE_ISUPDATE)
                                
                    //if ((! (is.iExt == m_arInfoTemplate.GetAt (0).iExt)) || ((tmTemp - m_tmLastUpdate) > tmSpanInterval) || (cError > 0))
                    if (((tmTemp - m_tmLastUpdate) > tmSpanInterval) || (cError > 0))
                        bRes = false;
                    else
                        ;
                }
                else
                    ;
		        break;
            default:
                ;
        }

	    strLog.format ("'Update' %s для %s, ошибок = %i, IsUpdate = %i", MB_HSTRING (arStrPermissionTemplate [iPermissionTemplate]), MB_HSTRING (GetFullPathTemplate ()), cError, bRes);
    
        tmExit = CTime::GetCurrentTime ();
    
        FILELOG_IF_WRITE (/*HDEBUG*/CHECK_STATE_TEMPLATE_ISUPDATE)
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), /*HDEBUG*/CHECK_STATE_TEMPLATE_ISUPDATE);
        FILELOG_ENDIF
    
        if ((tmExit - tmEntry).GetTotalSeconds () > 1) {
            FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE_ISUPDATE)
            strLog.format ("Время выполнения 'HCheckStateKTS::IsUpdate' составило %i секунд(ы)! для ID=%i", (tmExit - tmEntry).GetTotalSeconds (), m_iIdTemplate);
            FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CHECK_STATE_TEMPLATE_ISUPDATE);
            //MessageBox (AfxGetApp ()->m_pMainWnd->GetSafeHwnd (), strLog, _T("Внимание!"), MB_ICONINFORMATION);
            FILELOG_ENDIF
        }
        else
            ;
    }
    else
        ;
    
    return bRes;
}

void HCheckStateKTS::ClearTimeUpdate(void) {
    m_tmLastUpdate = 0x0;
}

HString HCheckStateKTS::GetDateInfoSample (int n) {
    if ((n > -1) && (n < m_arInfoSample.GetSize ()))
		return HString (m_arInfoSample.GetAt (n).tmLastWrite.Format (_T ("%d.%m.%y")).GetBuffer ());
	else
		return HString (_T (""));
}

HString HCheckStateKTS::GetTimeInfoSample (int n) {
	if (m_arInfoSample.GetSize ())
		//return m_arInfoSample.GetAt (n).tmLastWrite.Format ("%H:%M:%S");
		return HString (m_arInfoSample.GetAt (n).tmLastWrite.Format ("%H:%M").GetBuffer ());
	else
		return HString (_T (""));
}

void HCheckStateKTS::ClearDTInfoTemplate (int n) {
    SetDTInfoTemplate (n, 0x0);
}

void HCheckStateKTS::SetDTInfoTemplate (int n, CTime dtNew) {
	if (m_arInfoTemplate.GetSize ())
		m_arInfoTemplate.GetAt (n).tmLastWrite = dtNew;
}

void HCheckStateKTS::CopyInfoTemplateItem (int iSrc, int iDest) {
    m_arInfoTemplate.GetAt (iDest) = m_arInfoTemplate.GetAt (iSrc);
}

void HCheckStateKTS::CopyInfoSampleToTemplateItem  (int iSrc, int iDest) {
    m_arInfoTemplate.GetAt (iDest) = m_arInfoSample.GetAt (iSrc);
}

HString HCheckStateKTS::GetDateInfoTemplate (int n) {
    HString strRes;
    if (m_arInfoTemplate.GetAt (n).tmLastWrite > 0)
    {
        CString strTmp = m_arInfoTemplate.GetAt (n).tmLastWrite.Format ("%d.%m.%y");
        strRes = strTmp.GetBuffer ();
        strTmp.Empty ();
    }
    else
        strRes = _T ("00.00.00");

    return strRes;
}

HString HCheckStateKTS::GetTimeInfoTemplate (int n) {
    HString strRes;

    //return m_arInfoTemplate.GetAt (n).tmLastWrite.Format ("%H:%M:%S");
    if (m_arInfoTemplate.GetAt (n).tmLastWrite > 0) {
        CString strTmp = m_arInfoTemplate.GetAt (n).tmLastWrite.Format ("%H:%M");
        strRes = strTmp.GetBuffer ();
        strTmp.Empty ();
    }
    else
        strRes = _T ("-1:-1");

    return strRes;
}

HString HCheckStateKTS::GetExtInfoSample (int n) {
    HString strExt;
    if (m_arInfoSample.GetAt (n).iExt > -1)
        strExt.format (_T ("%i"), m_arInfoSample.GetAt (n).iExt);
    else
        strExt.format (_T ("%S"), _T ("eng"));
    
    while (strExt.length () < 3)
        strExt.insert (0, "0");
        
    return strExt;
}

//Поиск ИНДЕКСа в массиве по НОМЕРу расширениЯ файлА
int HCheckStateKTS::GetPosItemOfExt (LPWSTR ext) {
    int iExt = _wtoi (ext);
    if (iExt > -1)
        return GetPosItemOfExt (iExt);
    else
        return -1;
}

//Поиск ИНДЕКСа в массиве по НОМЕРу расширениЯ файлА
int HCheckStateKTS::GetPosItemOfExt (int ext) {
    int i, iItemRes = -1;

    //m_arInfoSample (m_arInfoTemplate).GetSize == GetMaxExt () ???

    for (i = 0; i <= GetMaxExt (); i ++) {
        if (ext == m_arInfoSample [i].iExt) {
	        iItemRes = i;
	        break;
        }
        else ;
    }

    return iItemRes;
}

CTime HCheckStateKTS::GetDTModifyInfoSample (int n) {
    return m_arInfoSample.GetAt (n).tmLastWrite;
}

void HCheckStateKTS::SetCheck (void) {
    FILELOG_VARIABLE

	HInfoTemplate it;
	HInfoSample is;

    CArray <HInfoTemplate> m_arInfoTemplateTemp;
	m_arInfoTemplateTemp.Copy (m_arInfoTemplate);
	m_arInfoTemplate.RemoveAll ();

    int i, j;

    it.iExt = -1;
    it.tmLastWrite = 0;

    //Выравнивание размеров массивов образца и реально найденных файлов ПУСТЫМи элементами
    j = 0;
	while (m_arInfoTemplateTemp.GetSize () < m_arInfoSample.GetSize ()) {
		m_arInfoTemplateTemp.Add (it);

		j ++;
    }

    for (i = m_iMinNumberExt; i < m_iMaxNumberExt + 1; i ++) {
		is = m_arInfoSample.GetAt (i - m_iMinNumberExt);

        j = 0;
        if (is.iExt > -1) {
		    while ((j < m_arInfoTemplateTemp.GetSize ()) && (! (m_arInfoTemplateTemp.GetAt (j).iExt == is.iExt)))
			    j ++;
        }
        else
            ;

		if (j < m_arInfoTemplateTemp.GetSize ()) {
			it = m_arInfoTemplateTemp.GetAt (j);
		}
		else {
            //Не найден файл с расширением 'is.iExt' ???
		    it.iExt = -1;
		    it.tmLastWrite = 0;
		}

		m_arInfoTemplate.Add (it);

        /*strLog.format ("Template: 'Расширение' = %i, 'Время' = %i", it.iExt, it.tmLastWrite);
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);*/
    }

	m_arInfoTemplateTemp.RemoveAll ();
    
	m_iState = STATE_ITEM_OK;
    
	//for (i = 0; i < m_arInfoTemplate.GetSize (); i ++)
	//for (i = 0; i < m_arInfoSample.GetSize (); i ++)
	for (i = m_iMinNumberExt; i < m_iMaxNumberExt + 1; i ++) {
		SetState (i - m_iMinNumberExt);
	}

    //strLog.format ("Размеры: 'Template' = %i, 'Sample' = %i", m_arInfoTemplate.GetSize (), m_arInfoSample.GetSize ());
    /*strLog.format ("Ошибки: %i", GetCountError ());
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);*/
}

int *HCheckStateKTS::FillArrayPseudoPosSelectedItem (int &sz) {
    int *piRes = NULL;
    FILELOG_VARIABLE

    FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
    strLog.format ("Вход в 'HCheckStateKTS::FillArrayPseudoPosSelectedItem'");
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CHECK_STATE_TEMPLATE);
    FILELOG_ENDIF

    int i = -1;
    vector <int> vecPosError;

    for (i = 0; i < m_arInfoSample.GetSize (); i ++) {
        if (m_arInfoSample.GetAt (i).iState < STATE_ITEM_OK)
            vecPosError.push_back (i + 1);
    }

    sz = vecPosError.size ();

    FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
    strLog.format (_T ("Размер ВЕКТОРа = %i'"), sz);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), CHECK_STATE_TEMPLATE);
    FILELOG_ENDIF

    piRes = DEBUG_NEW int [sz + 1];
    memset (piRes, 0x0, (sz + 1) * sizeof (int));

    for (i = 0; i < sz; i ++) {
        piRes [i] = vecPosError.at (i);
        
        FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
        strLog.format ("'HCheckStateKTS::FillArrayPseudoPosSelectedItem' ПсевдоНомер [%i] = %i", i, piRes [i]);
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CHECK_STATE_TEMPLATE);
        FILELOG_ENDIF
    }

    FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
    strLog.format ("Выход из 'HCheckStateKTS::FillArrayPseudoPosSelectedItem' = %i'", sz);
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CHECK_STATE_TEMPLATE);
    FILELOG_ENDIF
        
    vecPosError.clear ();
    
    return piRes;
}

int HCheckStateKTS::FillArrayStringPseudoSelectedItem (HString &strExt) {
    int i, iRes = 0; //Success
    
    for (i = 0; i < m_arInfoSample.GetSize (); i ++) {
        if (m_arInfoSample.GetAt (i).iState < STATE_ITEM_OK) {
            strExt.append (GetExtInfoSample (i));
            strExt.appendChar (';');
        }
        else ;
    }
    
    //Удаление КРАЙнего ';'
    if (strExt.length () > 0)
        strExt.cut (strExt.length () - 1, 1);
    else ;
    
    return iRes;
}
#include "stdafx.h"
#include "HObjectTemplate.h"

// HObjectTemplate.cpp : implementation file
// HObjectTemplate

extern CTime resetDigitTensSecond (CTime );
//EXT_DECLARE_HFILELOG
EXT_DECLARE_HTHREAD_FILELOG

IMPLEMENT_DYNCREATE (HObjectTemplate, CObject)

HObjectTemplate::HObjectTemplate () : CObject () {
    m_iIdTemplate = -1;
    m_hTreeItem = 0x0;
    m_strWildcard = _T ("");

	m_tmSpanModify = 0;
    m_tmSpanTask = 0;
    m_tmSpanWork = 0;
    //m_tmSpanIntervalDefinition = 0;
    m_iIdInterval = -1;

	m_iCountFileFind = 0;
    
    m_iMinNumberExt = 0;
    m_iMaxNumberExt = 0;
    
    m_iFirstExtError = 0;
    m_iLastExtError = 0;
    m_pHFIFirst = NULL;
    m_pHFILast = NULL;
}

HObjectTemplate::HObjectTemplate(int id, HTREEITEM hItem, HString strWildcard, CTimeSpan tmSpanModify, CTimeSpan tmSpanTask, CTimeSpan tmSpanWork, int idInterval, int iMinNumberExt, int iMaxNumberExt, BOOL bFullInfo) : CObject ()
{
    m_iIdTemplate = id;
    m_hTreeItem = hItem;
    m_strWildcard = strWildcard;

	m_tmSpanModify = CTimeSpan (resetDigitTensSecond (tmSpanModify.GetTotalSeconds ()).GetTime ());
    m_tmSpanTask = CTimeSpan (resetDigitTensSecond (tmSpanTask.GetTotalSeconds ()).GetTime ());
    m_tmSpanWork = tmSpanWork;
    //m_tmSpanIntervalDefinition = 0; //tmSpanInterval;
    m_iIdInterval = idInterval;

	m_iCountFileFind = 0;
    
    m_iMinNumberExt = iMinNumberExt;
    m_iMaxNumberExt = iMaxNumberExt;
    
    m_iFirstExtError = 0;
    m_iLastExtError = 0;
    m_pHFIFirst = NULL;
    m_pHFILast = NULL;
}

HObjectTemplate::~HObjectTemplate()
{
    m_strWildcard.free ();
    
    if (m_pHFIFirst)
        delete m_pHFIFirst;
    if (m_pHFILast)
        delete m_pHFILast;
}

SYSTEMTIME HObjectTemplate::stmLastWritten (void)
{
    return m_pHFILast->stmLastWritten ();
}

int HObjectTemplate::LastExt (void)
{
    return m_pHFILast->FileExtAsNumber ();
}

void HObjectTemplate::SetExtError (void)
{
    int iCountFile = GetMaxExt () - GetMinExt () + 1;
    CTimeSpan tmSpanInterval = GetIntervalDeclarative ();
	HInfoTemplate hit;
	SetTarget (&hit, CTime::GetCurrentTime (), true);

    m_iLastExtError = hit.iExt - m_pHFILast->FileExtAsNumber ();

	hit.iExt ++;
	if (hit.iExt > GetMaxExt ())
	{
	    hit.iExt = GetMinExt ();
	    hit.tmLastWrite -= (((hit.iExt - GetMaxExt ()) + iCountFile) * tmSpanInterval.GetTotalSeconds ());
	}
	else    
        /*while (iCountFile --)
	        hit.tmLastWrite -= m_tmSpanInterval;*/
        hit.tmLastWrite -= (iCountFile * tmSpanInterval.GetTotalSeconds ());

    m_iFirstExtError = m_pHFIFirst->FileExtAsNumber () - hit.iExt;
    
    if (m_iFirstExtError < 0 || m_iLastExtError < 0)
        iCountFile = 0;
}

int HObjectTemplate::GetCountFileFind (void) {
	return m_iCountFileFind;
	//return m_iMaxNumberExt + 1;
}

BOOL HObjectTemplate::SetInfo (BOOL bFullInfo) {
    CFileFind ff;
	BOOL bFindFile = false, bRes = true;
	CTimeSpan tmSpanIntervalDefinition = 0;
	HFileInfo *pHFITmp = NULL, *pHFIPrev = NULL;
	HString strTmp; //???
	FILELOG_VARIABLE
	
	FILELOG_IF_WRITE (OBJECT_TEMPLATE)
	strLog.format ("Вход в 'SetInfo' с аргументом: %i", (int ) bFullInfo);               
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), OBJECT_TEMPLATE);
    FILELOG_ENDIF
	
	bFindFile = ff.FindFile (m_strWildcard.GetBuffer ());
	
    while (bFindFile) {
        bFindFile = ff.FindNextFile ();

        if (ff.IsDirectory ())
             continue;
        else
            ;
             
        m_iCountFileFind ++;
             
        if (! (pHFITmp == NULL)) {
            delete pHFITmp;
            pHFITmp = NULL;
        }
        else
            ;
        
        HFILEINFO (pHFITmp, ff.GetFilePath ().GetBuffer ());
        if (pHFITmp == NULL) {
            bRes = false;
            break;
        }
        else
            ;
                
        if ((m_pHFIFirst == NULL) && (m_pHFILast == NULL)) {
            HFILEINFO (m_pHFIFirst, ff.GetFilePath ().GetBuffer ());
            HFILEINFO (m_pHFILast, ff.GetFilePath ().GetBuffer ());
            if ((m_pHFIFirst == NULL) || (m_pHFILast == NULL)) {
                bRes = false;
                break;
            }
            else
                ;
        }
        else
            ;
                
        if ((pHFITmp->tmLastWritten () - CTime::GetCurrentTime ()).GetTotalHours () > 1) {
            //ЗАЩИТА от дата/время модификации, превышающего ТЕКУЩЕЕ
			return false;
		}
        else
            ;

		if (! bFullInfo) {
		    //Макс. НОМЕР расширений
		    if (m_iMaxNumberExt < pHFITmp->FileExtAsNumber ())
		        m_iMaxNumberExt = pHFITmp->FileExtAsNumber ();
            else
                ;
    		
	        //Мин. НОМЕР расширений (Д,Б. '000')
		    if (m_iMinNumberExt > pHFITmp->FileExtAsNumber ())
		        m_iMinNumberExt = pHFITmp->FileExtAsNumber ();
            else
                ;
		}
        else
            ;

		//Сохраняем дата/время, расширение крайнего (самого молодого) сформированного файла
	    if (m_pHFILast->tmLastWritten () < pHFITmp->tmLastWritten ()) {
	        /*pHFITmp->FileName (strTmp);
	        strTmp.Append (CString (" моложе, чем "));
	        m_pHFILast->FileName (strTmp);
            FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), LEVEL_INFO_3);*/
            strTmp.free ();
	    
	        HFILEINFO (m_pHFILast, ff.GetFilePath ().GetBuffer ());
	        if (m_pHFILast == NULL) {
	            bRes = false;
	            break;
	        }
	        else
	            ;
	    }
        else
            ;
	    		
	    //Сохраняем дата/время, расширение первого (самого старого) сформированного файла
	    if (m_pHFIFirst->tmLastWritten () > pHFITmp->tmLastWritten ()) {
	        FILELOG_IF_WRITE (OBJECT_TEMPLATE)
	        pHFITmp->FileName (strTmp);
	        strTmp.append (" старше, чем ");
	        m_pHFILast->FileName (strTmp);
            FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), OBJECT_TEMPLATE);
            FILELOG_ENDIF
            strTmp.free ();

	        HFILEINFO (m_pHFIFirst, ff.GetFilePath ().GetBuffer ());
	        if (m_pHFIFirst == NULL) {
	            bRes = false;
	            break;
	        }
	        else
	            ;
	    }
        else
            ;
	    		
		if (! bFullInfo) {
	        //Определяем ИНТЕРВАЛ формирования файлов
	        if (pHFIPrev == NULL) {
	            HFILEINFO (pHFIPrev, ff.GetFilePath ().GetBuffer ());
	            if (pHFIPrev == NULL) {
	                bRes = false;
	                break;
	            }
	            else
	                ;
	        }
	        else {
		        //Интервал дата/время формирования 2-х файлов с расширениями, отличающимися на 1 (напр.: 'TEMPLATE.125' и 'TEMPLATE.126')
		        CTimeSpan tmSpanDiff = pHFITmp->tmLastWritten () - pHFIPrev->tmLastWritten ();
    				
		        //Секунды НЕ интересны
		        if (tmSpanDiff.GetSeconds () % 60) {
		            if (tmSpanDiff.GetSeconds () > 30)
                        tmSpanDiff += (60 - tmSpanDiff.GetSeconds ());
		            else
		                tmSpanDiff -= tmSpanDiff.GetSeconds ();
		        }
                else
                    ;
    		    
		        //Минуты НЕ интересны
		        if (tmSpanDiff.GetMinutes () % 30) {
		            if (tmSpanDiff.GetMinutes () > 30)
		                tmSpanDiff -= (tmSpanDiff.GetMinutes () % 30) * 60;
		            else
		                tmSpanDiff += (30 - (tmSpanDiff.GetMinutes () % 30)) * 60;
		        }
                else
                    ;
    			
		        //m_iMinuteCurrentlyInterval = timeDiff.GetMinutes ();
		        if (tmSpanIntervalDefinition > 0) {
		            if (tmSpanIntervalDefinition == tmSpanDiff)
		                tmSpanIntervalDefinition = tmSpanDiff;
		            else ; //Не СОВПАДАЕТ интервал ???
		        }
		        else
		            tmSpanIntervalDefinition = tmSpanDiff;
		            
		        pHFIPrev->CopyOf (pHFITmp);
	        } //else (pHFIPrev == NULL)
		}
        else
            ; //! bFullInfo
    } //while

    if (pHFITmp)
        delete pHFITmp;
    else ;
        
    if (pHFIPrev)
        delete pHFIPrev;
    else
        ;
    
    ff.Close ();
    
    if (m_pHFIFirst && m_pHFILast && bRes) {
        if ((m_pHFIFirst->FileExtAsNumber () == m_pHFILast->FileExtAsNumber ()) && (m_pHFIFirst->tmLastWritten () == m_pHFILast->tmLastWritten ())) {		
		    //MessageBox (AfxGetApp ()->m_pMainWnd->GetSafeHwnd (), _T (""), _T ("Внимание!"), MB_ICONINFORMATION);
    		
		    //Вариант №1 Ничего не делать
		    bRes = false;
    		
		    //Вариант №2 Попробовать ЕЩё раз, БЕСКОНЕЧНАЯ РЕКУРСИЯ ???
		    //Вариант №2 Попробовать ЕЩё раз, БЕСКОНЕЧНАЯ РЕКУРСИЯ ???
		    /*if (! SetInfo (false))		    
		        return false;*/
        }
        else {
            if ((! bFullInfo) && (tmSpanIntervalDefinition > 0)) {
                if (SetIdInterval (tmSpanIntervalDefinition) == 0)
                    ; //Установка значения 'm_iIdInterval' - успех
                else
                    ; //???
            }
            else
                ;
        }
    }
    else
        ; //Ни ОДНого прохода по циклУ 'while'
    
    /*
	if (! bFullInfo)
	    if (m_iMaxNumberExt < m_iCountFileFind - 1)
		    m_iMaxNumberExt = m_iCountFileFind - 1;
    */
    
    //SetExtError ();
    
    FILELOG_IF_WRITE (OBJECT_TEMPLATE)
    strTmp.format ("Выход из 'SetInfo' с кодом:%i", bRes);
    FILELOG_WRITE_WITHDT (strTmp.GetBuffer (), OBJECT_TEMPLATE);
    FILELOG_ENDIF
    strTmp.free ();
    
	return bRes;
}

void HObjectTemplate::SetTarget (HInfoSample *pIS, CTime tmCurrent, BOOL bCheckState)
//void HObjectTemplate::SetTarget (HInfoSample *pIS, CTime tmCurrent)
{
    HInfoTemplate it;
    SetTarget (&it, tmCurrent, bCheckState);
    //SetTarget (&it, tmCurrent);
    
    pIS->iExt = it.iExt;
    pIS->tmLastWrite = it.tmLastWrite;
    pIS->iState = STATE_ITEM_NODATA;
}

void HObjectTemplate::SetTarget (HInfoTemplate *pIT, CTime tmCurrent, BOOL bCheckState)
//void HObjectTemplate::SetTarget (HInfoTemplate *pIT, CTime tmCurrent)
{
    //CString strCurrentTime, strLastWriteTime, strSpanInterval, strSpanDiff;
    CTimeSpan tmSpanDiff;
    LONGLONG lCountInterval, lRemains;
    CString strTemp;
    SYSTEMTIME stm;

    tmCurrent.GetAsSystemTime (stm);

    //РАБОТа
	//ОТЛАДКа
	/*
	//tmCurrent = CTime (2011, 2, 1, 1, 1, 1, 0);
    	
    int h, m, s;
    //30 мин
    //m_tmSpanInterval = CTime (1977, 1, 1, 0, 30, 0) - CTime (1977, 1, 1, 0, 0, 0); // 3600, 
    //1 час
    //m_tmSpanInterval = CTime (1977, 1, 1, 1, 0, 0) - CTime (1977, 1, 1, 0, 0, 0); // 3600, 
    //1 сутки
    //m_tmSpanInterval = CTime (1977, 1, 2, 0, 0, 0) - CTime (1977, 1, 1, 0, 0, 0); // 3600, 
    //Случайно
    m_tmSpanInterval = CTime (1977, 1, 1, 13, 47, 31) - CTime (1977, 1, 1, 0, 0, 0); // 3600, 
    strSpanInterval = m_tmSpanInterval.Format ("%H:%M:%S");

    m_tmLastWrite = CTime (2011, 12, 12, 1, 30, 54);
    strLastWriteTime = m_tmLastWrite.Format ("%d.%m.%Y %H:%M:%S");

    for (h = 0; h < 24; h ++)
    {
        for (m = 0; m < 60; m ++)
        {
            for (s = 0; s < 60; s ++)
            {
                tmCurrent  = CTime (2011, 12, 13, h, m, s);
    */
				//Секунды НЕ интересны
	            //tmCurrent -= tmCurrent.GetSecond ();
	//            strCurrentTime = tmCurrent.Format ("%d.%m.%Y %H:%M:%S");

	            //Учитываем смещение запуска задачи, её ВЫПОЛНЕНия, и ИСКУССТВЕННО записываемого дата/время модификации (В СЕКУНДАХ)

	            //Определяем РАСШИРЕНИЕ по крайнему (самому молодому)
	            //tmSpanDiff = tmCurrent - m_tmLastWrite;

	            //Определяем РАСШИРЕНИЕ по первому (самому старому)
	            //tmSpanDiff = tmCurrent - m_tmFirstWrite;
	            //tmSpanDiff = tmCurrent - (m_tmFirstWrite + ((m_tmSpanTask.GetTotalSeconds () - m_tmSpanModify.GetTotalSeconds ()) + m_tmSpanWork.GetTotalSeconds ()));
	            //tmSpanDiff = tmCurrent - (m_tmFirstWrite + ((m_tmSpanTask - m_tmSpanModify) + m_tmSpanWork));

	            int iDiffExtError = m_iFirstExtError;
                if (iDiffExtError < 0)
                {
                    MessageBox (AfxGetApp ()->m_pMainWnd->GetSafeHwnd (), _T (""), _T ("Внимание!"), MB_ICONINFORMATION);

                    pIT->iExt = -1;
                    pIT->tmLastWrite = 0;
                }
                else
                {
                    CTimeSpan tmSpanInterval = GetIntervalDeclarative ();

	                if (bCheckState == TRUE)
	                //if (true)
	                {
	                    //tmCurrent -= (m_tmSpanTask - m_tmSpanModify + m_tmSpanWork);

	                    //tmCurrent = tmCurrent - (m_tmSpanTask - m_tmSpanModify + m_tmSpanWork);

	                    //tmCurrent = tmCurrent - m_tmSpanTask + m_tmSpanModify - m_tmSpanWork;
	                    tmCurrent -= m_tmSpanTask;
	                    tmCurrent += m_tmSpanModify;
	                    tmCurrent -= m_tmSpanWork;

	                    if (m_iIdTemplate == 143) // ???
	                        if (tmCurrent.GetMinute () > tmSpanInterval.GetTotalMinutes ())
	                            tmCurrent -= tmSpanInterval;
                            else
                                ;
                        else
                            ;
                    }
                    else
                        ;

	                //if (m_pHFIFirst->tmLastWritten ().GetMinute () % 5 == 0)
	                /*if (m_tmSpanInterval.GetTotalSeconds () == 300)
	                    tmSpanDiff = tmCurrent - resetDigitTensSecond (m_pHFIFirst->tmLastWritten ());
	                else
	                    tmSpanDiff = tmCurrent - resetDigitUnitsMinute (m_pHFIFirst->tmLastWritten ());*/

                    //Отладка ???
                    tmCurrent.GetAsSystemTime (stm);

                    tmSpanDiff = tmCurrent - resetDigitTensSecond (m_pHFIFirst->tmLastWritten ());

                    //Отладка ???
                    tmCurrent.GetAsSystemTime (stm);

                    /*while (iDiffExtError--)
                        tmSpanDiff += m_tmSpanInterval;*/
                    tmSpanDiff += iDiffExtError * tmSpanInterval.GetTotalSeconds ();

	                lCountInterval = tmSpanDiff.GetTotalSeconds () / tmSpanInterval.GetTotalSeconds ();
	                lRemains = tmSpanDiff.GetTotalSeconds () % tmSpanInterval.GetTotalSeconds ();

                    //if (lCountInterval % (m_iMaxNumberExt - m_iMinNumberExt + 1))
                    //if (m_iFirstExt == m_iMaxNumberExt)
                    //    pIS->iExt = m_iFirstExt + (lCountInterval % (m_iMaxNumberExt - m_iMinNumberExt));
                    //else
                        //pIS->iExt = m_iFirstExt - 1;
                        pIT->iExt = m_pHFIFirst->FileExtAsNumber () + (lCountInterval % (m_iMaxNumberExt - m_iMinNumberExt + 1));

	                if (pIT->iExt > m_iMaxNumberExt)
                        pIT->iExt = (m_pHFIFirst->FileExtAsNumber () + (lCountInterval % (m_iMaxNumberExt - m_iMinNumberExt + 1))) - (m_iMaxNumberExt - m_iMinNumberExt + 1);
                    else
	                    if (pIT->iExt < m_iMinNumberExt)
	                        pIT->iExt = pIT->iExt + (m_iMaxNumberExt - m_iMinNumberExt + 1);
                        else
                            ;

	                //tmCurrent -= ((m_tmSpanTask.GetTotalSeconds () - m_tmSpanModify.GetTotalSeconds ()) + m_tmSpanWork.GetTotalSeconds ());
	                tmCurrent -= lRemains;

                    //26.10.2014
                    if (! (tmSpanInterval < 86400))
                        tmCurrent += 60 * 60;
                    else
                        ;

	                pIT->tmLastWrite = tmCurrent;
                }
	/*
            }
        }
    }
    */
}

CTimeSpan HObjectTemplate::GetIntervalVariable (CTime tmOrigin, short direction) {
    return CTimeSpan (PTR_DATETIMEINTERVAL->GetInterval (m_iIdInterval, tmOrigin, direction));
}

//Нахождение ближайшего к ПОЛУНОЧи времени модификации ШАБЛОНа
CTimeSpan HObjectTemplate::GetSpanModify (void)
{
    CTime tmModify = m_pHFIFirst->tmLastWritten ();
    CTimeSpan tmSpanResult (0, tmModify.GetHour (), tmModify.GetMinute (), tmModify.GetSecond ()),
            tmSpanInterval = GetIntervalDeclarative ();
    
    while ((tmSpanResult - tmSpanInterval) > -1)
    {
        tmSpanResult -= tmSpanInterval;
    }
        
    return tmSpanResult;
}

#ifdef PTR_DB
int HObjectTemplate::InitializeRS (TCHAR *nameRS)
{
    FILELOG_VARIABLE
    int iRes = 0;
    void *pRefRecordset = NULL;
    
    m_pmapDBRS.Lookup (nameRS, pRefRecordset);

    if (pRefRecordset) {
        //ERROR - ПОВТОРная попытка инициализации ???
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("Попытка ПОВТОРно инициализировать ТАБЛицу=%s", MB_HSTRING (HString (nameRS)));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF
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

void HObjectTemplate::ReleaseRS (TCHAR *nameRS, BOOL bClose, BOOL bDelete) {
    PTR_DB->ReleaseRS (this, HRECORDSET (m_pmapDBRS [nameRS]), bClose, bDelete);
    m_pmapDBRS.RemoveKey (nameRS);
}
#endif
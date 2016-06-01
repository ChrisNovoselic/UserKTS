#include "stdafx.h"

EXT_DECLARE_HTHREAD_FILELOG

extern int GetCountDayInYearMounth (int , int );

HDateTimeInterval::HDateTimeInterval (void) {
	m_mapIntervalValue.InitHashTable (119);
	m_mapIntervalName.InitHashTable (119);

	::GetTimeZoneInformation (&m_datetimeZoneInfo);
}

HDateTimeInterval::~HDateTimeInterval (void) {
    //int iKey = -1, iVal = -1;
    //POSITION iPos = m_mapInterval.GetStartPosition ();
    //while (! (iPos == NULL))
    //{
    //    m_mapInterval.GetNextAssoc (iPos, iKey, iVal);
    //}
    
    m_mapIntervalValue.RemoveAll ();
    m_mapIntervalName.RemoveAll ();
}

int HDateTimeInterval::AddInterval (int id, HString &id_name, long val) {
    int iRes = 0; //Success
    m_mapIntervalValue.SetAt (id, val);
    m_mapIntervalName.SetAt (id, id_name);
    return iRes;
}

//Все идентификаторы СТРОГО из таблицы БД-настройки 'time'
long HDateTimeInterval::GetInterval (int id) {
    FILELOG_VARIABLE
    long lRetVal = -1;

    if (m_mapIntervalValue.Lookup (id, (long &) lRetVal) == TRUE) {
        /*HString id_name;
        m_mapIntervalName.Lookup (id, (HString &) id_name);
        if (id_name.compareNoCase ("DAY_01_ACCUM") == 0) {
            int id_day = -1;

            int idKey = -1; HString strValName;
            POSITION iPos = m_mapIntervalName.GetStartPosition ();
            while (! (iPos == NULL)) {
                m_mapIntervalName.GetNextAssoc (iPos, idKey, strValName);
                if (strValName.compareNoCase ("DAY_01")) {
                    id_day = idKey;
                    break;
                }
                else
                    ;
            }

            m_mapIntervalValue.Lookup (idKey, (long &) lRetVal);
        }
        else
            lRetVal = -1;*/
    }
    else {
        FILELOG_IF_WRITE (CONTENT_TEMPLATE)
        strLog.format ("Ошибка взятия интервала (ID = %i)", id);
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
        FILELOG_ENDIF

        lRetVal = -1;
    }
    
    return lRetVal;
}

//Все идентификаторы СТРОГО из таблицы БД-настройки 'time'
long HDateTimeInterval::GetInterval (HString &idName) {
    FILELOG_VARIABLE
    long lRetVal = -1;

    int iKey = -1; HString idNameCurrent;
    POSITION iPos = m_mapIntervalName.GetStartPosition ();
    while (! (iPos == NULL)) {
        m_mapIntervalName.GetNextAssoc (iPos, iKey, idNameCurrent);
        if (idNameCurrent.compareWithCase (idName) == 0)
            break;
        else
            ;
        //Признак - элемент не найден
        iKey = -1; 
    }

    //if ((! (iPos == 0x0)) && (m_mapIntervalValue.Lookup (iKey, (long &) lRetVal) == TRUE)) {
    if ((! (iKey < 0)) && (m_mapIntervalValue.Lookup (iKey, (long &) lRetVal) == TRUE)) {
    }
    else {
        //FILELOG_IF_WRITE (HERROR)
        strLog.format ("Ошибка взятия интервала (Имя = %s)", MB_HSTRING (idName));
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
        //FILELOG_ENDIF

        lRetVal = -1;
    }
    
    return lRetVal;
}

long HDateTimeInterval::GetInterval (int id, CTime &tmOrigin, short direction, int itmOriginUpdate) {
    long lRes = -1;
	//CTime tmBegin;
	//Здесь изменяем при необходимости дата/время
	//"Плавающий ПЕРИОД" возвращаем из 'PTR_DATETIMEINTERVAL'
    HString strIdIntervalName;
    if (m_mapIntervalName.Lookup (id, strIdIntervalName) == TRUE) {
        if (! (itmOriginUpdate == 0)) {
            //Для 'HContentTemplate::CalculateTemplate'
            if (strIdIntervalName.compareNoCase ("DAY_01_ACCUM") == 0) {
                //Накапливаемые значения
			    //tmBegin = CTime (tmOrigin.GetYear (), tmOrigin.GetMonth (), 1, tmOrigin.GetHour (), tmOrigin.GetMinute (), tmOrigin.GetSecond ());
			    /*if (GetCountDayInYearMounth (tmOrigin.GetYear (), tmOrigin.GetMonth ()) == tmOrigin.GetDay ()) {
				    //Крайний день МЕСЯЦа - начало, первый день МЕСЯЦа - окончание
				    tmBegin = CTime (tmOrigin.GetTime () - (tmOrigin.GetDay () + 0) * CTimeSpan (PTR_DATETIMEINTERVAL->GetInterval (m_iIdInterval, tmOrigin, direction)).GetTotalSeconds ());
				    tmSpanRes = tmOrigin - tmBegin;
				    tmOrigin = tmBegin;
			    }
			    else*/ {
				    lRes = (tmOrigin.GetDay () + 0) * CTimeSpan (GetInterval (id)).GetTotalSeconds ();
				    tmOrigin = CTime (tmOrigin.GetTime () - (tmOrigin.GetDay () - 1) * CTimeSpan (GetInterval (id)).GetTotalSeconds ());
			    }
            }
            else
                lRes = GetInterval (id);
        }
        else {
            lRes = GetInterval (id);

			if (lRes < 0) {
				if (strIdIntervalName.compareNoCase ("MONTH_1") == 0) {
					lRes = GetCountDayInYearMounth (tmOrigin.GetYear (), tmOrigin.GetMonth ()) * 24 * 60 * 60;
				}
				else ;
			}
			else ;
		}
    }
    else
        ;

	return lRes;
}

int HDateTimeInterval::GetIdInterval (long totalSeconds) {
    int iKey = -1; long lVal = -1;
    POSITION iPos = m_mapIntervalValue.GetStartPosition ();
    while (! (iPos == NULL)) {
        m_mapIntervalValue.GetNextAssoc (iPos, iKey, lVal);
        if (lVal == totalSeconds)
            break;
        else
            ;
    }
    
    if (iKey > 0)
        return iKey;
    else
        return -1;
}

//Имя по секундам
HString HDateTimeInterval::GetNameInterval (long totalSeconds) {
	return GetNameInterval (GetIdInterval (totalSeconds));
}

//Имя по идентификатору
HString HDateTimeInterval::GetNameInterval (int id) {
	HString strRes;
	m_mapIntervalName.Lookup (id, strRes);

	return strRes;
}
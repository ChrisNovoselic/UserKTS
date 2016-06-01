#pragma once
//#include "stdafx.h"

#define DIRECTION_PAST -1
#define DIRECTION_FUTURE 1

enum INDEX_TIME {SECOND_01 = -3, SECOND_05 = -2, SECOND_10 = -1, SECOND_15, MINUTE_01, MINUTE_03, MINUTE_05, MINUTE_30, HOUR_01, DAY_01, MONTH_01, YEAR_01, COUNT_INDEX_TIME};

class HDateTimeInterval : public CObject {
public:
    //enum ID_DATETIME_INTERVAL {SECOND_15, SECOND_30, MINUTE_01, MUNUTE_03, MINUTE_05, MINUTE_10, MINUTE_15, MINUTE_30, HOUR_01, HOUR_01_ACCUM, HOUR_02, HOUR_03, HOUR_04, HOUR_06, HOUR_08, HOUR_12, DAY_01_ACCUM, DAY_07, DAY_10, MONTH_01, MONTH_01_ACCUM, YEAR_01};

    HDateTimeInterval(void);
    ~HDateTimeInterval(void);

    enum DIRECTION_DATETIME {PAST = -1, FUTURE = 1};

    int AddInterval (int , HString &, long );
    long GetInterval (int ); //Секунды по идентификатору
    long GetInterval (HString & ); //Секунды по ИМЕНи идентификатору
    //long GetInterval (int , HString &); //Секунды по идентификатору + возврат имени
    long GetInterval (int , CTime &, short, int = 0); //Секунды по идентификатору для времени в направлении

    int GetIdInterval (long ); //Идентификатор по секундам
	HString GetNameInterval (long ); //Имя по секундам
	HString GetNameInterval (int ); //Имя по идентификатору

	inline LONGLONG FileTimeBias (void) { return m_datetimeZoneInfo.Bias * 60 * pow (10.0, 7); }
	inline LONGLONG tm64tBias (void) { return m_datetimeZoneInfo.Bias * 60; }

    int IsInitialize (void) { return m_mapIntervalValue.GetSize (); }
    
private:
	TIME_ZONE_INFORMATION m_datetimeZoneInfo;
    //Карта идентификатор_интервала - длительность_интервала
    CMap <int, int, long, long> m_mapIntervalValue;
    CMap <int, int, HString, HString> m_mapIntervalName;
};

ENUM_INCREMENT (INDEX_TIME)

#define DECLARE_HDATETIMEINTERVAL HDateTimeInterval *g_ptrDateTimeInterval = DEBUG_NEW HDateTimeInterval ();
#define EXT_DECLARE_HDATETIMEINTERVAL extern HDateTimeInterval *g_ptrDateTimeInterval;
#define PTR_DATETIMEINTERVAL g_ptrDateTimeInterval

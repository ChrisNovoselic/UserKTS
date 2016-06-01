#pragma once
#include "hwinthread.h"

#include <initguid.h>
//#import "C:\Temp\energ6\getdata.exe"

//#include <getdata.h>
#include <getdata2.h>

#include <HEnerg6Data.h>

#define COUNT_QUERY_TIME MONTH_01

class HThreadGetData : public HWinThread {
public:
    HThreadGetData (void);
    virtual ~HThreadGetData (void);

    enum ID_DATAITEM {/*STR_CONNECT_DB = 286, INT_SOURCE_ID,*/ TIME64T_QUERY_MIN_05 = 386, TIME64T_QUERY_MIN_30,
															PTR_IMP_MIN_05 = 486, PTR_IMP_MIN_30,


                                                            PTR_IMP_MIN_05_ENG2,
															PTR_VAL_MIN_05, PTR_VAL_MIN_30,
															COUNT_ID_DATAITEM};

	//enum QUERY_TYPE {VALUE, IMPULS, COUNT_QUERY_TYPE};
	//enum QUERY_TIME {SEC_15, MIN_03, MIN_05, MIN_30, HOUR_01, DAY_01, MONTH_01, YEAR_01, COUNT_QUERY_TIME};
	enum VALUE_TYPE {PREVIOUS, CURRENT, NEXT, COUNT_VALUE_TYPE};	

	//enum INDEX_DATETIME_TYPE {REAL, CALC, COUNT_INDEX_DATETIME_TYPE};

    enum STATE_DATA {UNKNOWN, WAIT, READY, SAVE, COUNT_STATE_DATA};

    int InitInstance (void);
    int ExitInstance (void);

	virtual void Stop (void) /*PURE;*/ { m_iStop = 1; }

    PTR_HANDLER_ONTHREADMESSAGE OnResumeRun;
    PTR_HANDLER_ONTHREADMESSAGE OnConnectToServer;
	PTR_HANDLER_ONTHREADMESSAGE OnMessageTimer; //Обрабатывает сообщения от 'AfxGetApp ()'
	PTR_HANDLER_ONTHREADMESSAGE OnMessageReadyData; //Обрабатывает ЛОКАЛьные сообщения
    PTR_HANDLER_ONTHREADMESSAGE OnMessageWriteData; //Обрабатывает сообщения от 'HThreadWriteData' (отправленные данные записаны - их можно считывать)
    PTR_HANDLER_ONTHREADMESSAGE OnMessageUserNumberPTICControl;
    PTR_HANDLER_ONTHREADMESSAGE OnResetThreadResponce;

	int GetData (INDEX_TIME idQueryTime, WORD *ptrRes) { ptrRes = m_arImp [idQueryTime]; return 512; }
	int GetData (INDEX_TIME, double *) {}

    inline int GetUserNumberPTICControl (void) { return m_iUserNumberPTICControl; }

private:
	//struct INDEX_DATETIMEINTERVAL {
	//	//LPWSTR pwchName;
	//	long periodSeconds;
	//	long ofYear; long ofMonth; long ofDay;
	//	long ofHour; int of30Min; short of05Min;

	//	//INDEX_DATETIMEINTERVAL (LPWSTR name) {
	//	INDEX_DATETIMEINTERVAL (void) {
	//		//pwchName = name;
	//		periodSeconds = -1;
	//		ofYear = 1; ofMonth = 1; ofDay = 1;
	//		ofHour = 1; of30Min = 1; of05Min = 1;
	//	}

	//	/*void operator ++ (int) {
	//	}*/
	//};

    BOOL QueryNumberPTICControl ();

//public для 'HThreadGUIControl'
public:
    struct DATA_RESPONSE {
		long idComClient;
		/*double dblValQuery;*/	long lValScale;	__time64_t tm64tQuery;
		double arDblSummaryValues [COUNT_QUERY_TIME] [COUNT_VALUE_TYPE];
		double arDblValues /*[COUNT_QUERY_TYPE]*/ [COUNT_QUERY_TIME] [COUNT_VALUE_TYPE];
		WORD arWordImpulses /*[COUNT_QUERY_TYPE]*/ [COUNT_QUERY_TIME] [COUNT_VALUE_TYPE];
		//INDEX_DATETIMEINTERVAL indxdttmCounter [COUNT_QUERY_TIME] [COUNT_VALUE_TYPE];
        WORD indxdttmCounter [COUNT_QUERY_TIME] [COUNT_QUERY_TIME] [COUNT_VALUE_TYPE];
        STATE_DATA stateData [COUNT_QUERY_TIME] [COUNT_VALUE_TYPE];

		DATA_RESPONSE (void);

        void IncNumberInterval (INDEX_TIME );
	};

private:
    HEnerg6Data *m_ptrEnerg6Data;
    Ie6data2 *m_ptre6Data;
    map <int, long> m_ariIdTable;
	__time64_t m_tm64tQuery;
	int m_iStop;
	map <int, map <int, DATA_RESPONSE *>> m_mapData;
	WORD m_arImp [COUNT_QUERY_TIME] [512];

	//INDEX_DATETIMEINTERVAL m_indxdttmCounter [COUNT_QUERY_TIME] [COUNT_INDEX_DATETIME_TYPE];

    WORD m_iUserNumberPTICControl;

    int PostDataToControl (INDEX_TIME, BOOL);

	DECLARE_MESSAGE_MAP()
};

ENUM_INCREMENT (HThreadGetData::VALUE_TYPE)

//ENUM_INCREMENT (HThreadGetData::INDEX_DATETIME_TYPE)
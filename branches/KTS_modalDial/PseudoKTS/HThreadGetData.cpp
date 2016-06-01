#include "stdafx.h"

#include "PseudoKTS.h" //PseudoKTS::GetPtrThread

#include <HThreadDirectoryWatched.h> //HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED

#include <henerg5data.h>

#include "HThreadGetData.h"

EXT_DECLARE_HTHREAD_FILELOG

EXT_DECLARE_HDATETIMEINTERVAL

extern HString tokenizeTypeSource (int, HString &);
extern HString tokenizeConnectDB (int, HString &);

extern int GetCountDayInYearMounth (int , int );

void roundDblToPrecision (double &, int );

HThreadGetData::HThreadGetData (void) : m_ptre6Data (0x0),
                                        m_ptrEnerg6Data (0x0),
                                        m_iUserNumberPTICControl (0x0),
										m_iStop (0x0) {
	memset (m_arImp, 0x0, COUNT_QUERY_TIME * 512 * sizeof (WORD));
	m_tm64tQuery = 0x0;
}

HThreadGetData::~HThreadGetData(void) {
    for (map <int, map <int, DATA_RESPONSE *>>::iterator iteratorUSDData = m_mapData.begin (); ! (iteratorUSDData == m_mapData.end ()); ++ iteratorUSDData) {
		for (map <int, DATA_RESPONSE *>::iterator iteratorData = (*iteratorUSDData).second.begin (); ! (iteratorData == (*iteratorUSDData).second.end ()); ++ iteratorData) {
		    delete (*iteratorData).second;
	    }
	}
	m_mapData.clear ();

    delete m_ptrEnerg6Data;
}

BEGIN_MESSAGE_MAP (HThreadGetData, HWinThread)
    ON_THREAD_MESSAGE (WM_USER_THREAD_RESUME, &OnResumeRun)
    ON_THREAD_MESSAGE (WM_USER_CONNECT_TO_SERVER, &OnConnectToServer)
	ON_THREAD_MESSAGE (WM_USER_TIMER, &OnMessageTimer)
    ON_THREAD_MESSAGE (WM_USER_READY_DATA, &OnMessageReadyData)
    ON_THREAD_MESSAGE (WM_USER_NUMBER_PTIC_CONTROL, &OnMessageUserNumberPTICControl)
    ON_THREAD_MESSAGE (WM_USER_WRITEDATA, &OnMessageWriteData)
    ON_THREAD_MESSAGE (WM_USER_RESET_THREAD_RESPONCE, &OnResetThreadResponce)
END_MESSAGE_MAP()

HThreadGetData::DATA_RESPONSE::DATA_RESPONSE (void) {
	idComClient = -1;
	/*dblValQuery = 0.0;*/ lValScale = -1; tm64tQuery = 0.0;
    memset (arDblSummaryValues, 0, COUNT_QUERY_TIME * COUNT_VALUE_TYPE * sizeof (double));
    memset (arDblValues, 0, COUNT_QUERY_TIME * COUNT_VALUE_TYPE * sizeof (double));
    memset (arWordImpulses, 0, COUNT_QUERY_TIME * COUNT_VALUE_TYPE * sizeof (WORD));
    //memset (indxdttmCounter, 1, COUNT_QUERY_TIME * COUNT_QUERY_TIME * COUNT_VALUE_TYPE * sizeof (WORD));
			
    int i, j, k;
	for (INDEX_TIME i = SECOND_15; i < COUNT_QUERY_TIME; i ++) {
		for (INDEX_TIME j = SECOND_15; j < COUNT_QUERY_TIME; j ++) {
			for (VALUE_TYPE k = PREVIOUS; k < COUNT_VALUE_TYPE; k ++) {
				//arDblSummaryValues [j] [k] = 0.0;
				//arDblValues /*[i]*/ [j] [k] = 0.0;
				//arWordImpulses /*[i]*/ [j] [k] = 0;

                indxdttmCounter [i] [j] [k] = 1;

                //stateData [j] [k] = WAIT;
			}
		}
	}
}

void HThreadGetData::DATA_RESPONSE::IncNumberInterval (INDEX_TIME type_tm) {
    arDblSummaryValues [type_tm] [PREVIOUS] = arDblSummaryValues [type_tm] [CURRENT];
    arDblSummaryValues [type_tm] [CURRENT] = arDblSummaryValues [type_tm] [NEXT];
    arDblSummaryValues [type_tm] [NEXT] = 0.0;

    arDblValues [type_tm] [PREVIOUS] = arDblValues [type_tm] [CURRENT];
    arDblValues [type_tm] [CURRENT] = arDblValues [type_tm] [NEXT];
    arDblValues [type_tm] [NEXT] = 0.0;

    arWordImpulses [type_tm] [PREVIOUS] = arWordImpulses [type_tm] [CURRENT];
    arWordImpulses [type_tm] [CURRENT] = arWordImpulses [type_tm] [NEXT];
    arWordImpulses [type_tm] [NEXT] = 0;

    for (INDEX_TIME id_type_tm = SECOND_15; id_type_tm < COUNT_QUERY_TIME; id_type_tm++) {
        indxdttmCounter [id_type_tm] [type_tm] [PREVIOUS] = indxdttmCounter [id_type_tm] [type_tm] [CURRENT];
        indxdttmCounter [id_type_tm] [type_tm] [CURRENT] = indxdttmCounter [id_type_tm] [type_tm] [NEXT];
        indxdttmCounter [id_type_tm] [type_tm] [NEXT] = 1;
    }
}

int HThreadGetData::InitInstance (void) {
    int iRes = -1;
	iRes = HWinThread::InitInstance ();

    FILELOG_VARIABLE

    if (iRes == 1) {
		/*__time64_t tm64tNow, tm64t15Sec, tm64t05Min, tm64t30Min, tm64t01Hour;
		time (&tm64tNow);
		tm64t15Sec = tm64tNow - tm64tNow % PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("SECOND_15")));
		tm64t05Min = tm64tNow - tm64tNow % PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("MINUTE_05")));
		tm64t30Min = tm64tNow - tm64tNow % PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("MINUTE_30")));
		tm64t01Hour = tm64tNow - tm64tNow % PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("HOUR_01")));

		m_indxdttmCounter [SECOND_15] [REAL].of05Min = (tm64t15Sec - tm64t05Min) / PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("SECOND_15"))) + 1;
		m_indxdttmCounter [SECOND_15] [REAL].of30Min = (tm64t15Sec - tm64t30Min) / PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("SECOND_15"))) + 1;
		m_indxdttmCounter [SECOND_15] [REAL].ofHour = (tm64t15Sec - tm64t01Hour) / PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("SECOND_15"))) + 1;

		m_indxdttmCounter [MINUTE_05] [REAL].of30Min = (tm64t05Min - tm64t30Min) / PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("MINUTE_05"))) + 1;
		m_indxdttmCounter [MINUTE_05] [REAL].ofHour = (tm64t05Min - tm64t01Hour) / PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("MINUTE_05"))) + 1;*/

		//Вариант №1 инициализации COM-серверов
		//HRESULT hCom = CoInitializeEx (NULL, COINIT_MULTITHREADED);
		//Вариант №2 инициализации COM-серверов
		// Инициализация библиотек OLE
		if (! AfxOleInit ())	{
			strLog.format (_T ("Ошибка при вызове 'AfxOleInit ()' = %S"), IDP_OLE_INIT_FAILED);
			FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HERROR);
			return FALSE;
		}
		else
			;

		CLSID clsid;
		HRESULT hResComServer = 0x0;
		//IUnknown *pIUnknown = 0x0;
		////IDispatch *pIDispatch = 0x0; 

		hResComServer = CLSIDFromProgID (OLESTR ("getData2.e6data2"), &clsid);
		//Получение COM-сервера Вариант №1
		hResComServer = CoCreateInstance (clsid, NULL, CLSCTX_LOCAL_SERVER, IID_Ie6data2, (LPVOID *) &m_ptre6Data);
		//Получение COM-сервера Вариант №2
		//IClassFactory *pClassFactory = 0x0;
		//hResComServer = CoGetClassObject (clsid, CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**) &pClassFactory);
		////hResComServer = pClassFactory->CreateInstance (NULL, IID_IUnknown, (void**) &pIUnknown);
		//////hResComServer = pClassFactory->CreateInstance (NULL, IID_IDispatch, (void**) &pIDispatch);
		////////hResComServer = pClassFactory->CreateInstance (NULL, IID_Ie6data, (void**) &pe6Data);
		//pClassFactory->Release ();
		////hResComServer = pIUnknown->QueryInterface (IID_Ie6data, (void **) &pe6Data);
		//////hResComServer = pIDispatch->QueryInterface (IID_Ie6data, (void **) &pe6Data);

        //Вызов методов COM-сервера
	}
	else
		;

    return iRes;
}

int HThreadGetData::ExitInstance (void) {
    HRESULT hResComServer = 0x0;
    map <int, long>::iterator iterIdTable =  m_ariIdTable.begin ();
    if (! (iterIdTable == m_ariIdTable.end ())) {
        do { m_ptre6Data->pauseTable ((*iterIdTable).second); m_ptre6Data->CloseTable ((*iterIdTable).second); } while (! (++ iterIdTable == m_ariIdTable.end ()));
    }
    else
        ;
    m_ariIdTable.clear ();

    //Освобождение объекта
    hResComServer = m_ptre6Data->Release ();

    //delete m_ptre6Data;
    
    AfxOleTerm (FALSE);
    
    return HWinThread::ExitInstance ();
}

afx_msg void HThreadGetData::OnResumeRun (WPARAM wp, LPARAM ) {
    HWinThread *ptrThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GUI_FILESDB);
    if (! (ptrThread == 0x0))
        ::PostThreadMessage (ptrThread->m_nThreadID, WM_USER_QUERY_CONNECT_TO_SERVER, m_nThreadID, 0);
    else
        ;
}

afx_msg void HThreadGetData::OnConnectToServer (WPARAM wp, LPARAM lp) {
	FILELOG_VARIABLE

    COleVariant oleVarPar;
    HRESULT hResComServer = 0x0;
    HString strConnectDB, strarServerNames, strServerName, strVarPar;
    int iRes = 0, iError = 0;

    map <int, long>::iterator iterIdTable =  m_ariIdTable.begin ();
    if (! (iterIdTable == m_ariIdTable.end ())) {
        do { hResComServer = m_ptre6Data->CloseTable ((*iterIdTable).second); } while (! (++ iterIdTable == m_ariIdTable.end ()));
    }
    else
        ;
    m_ariIdTable.clear ();

    //strConnectDB = stringData (GetDataOfId (STR_CONNECT_DB));
    strarServerNames = PTR_SETTINGS->CompileSourceField ((int) wp, HString (_T ("SERVER_NAME")), HDBSettings::SQL_DB);

    int iPos = 0, iNumSource = 0;
    if (strarServerNames.isUses ()) {
        while (iPos > -1) {
            strServerName = strarServerNames.tokenize (_T (","), iPos);
            if (strServerName.isUses () && (iNumSource == (int ) lp)) {
                oleVarPar =  strServerName.GetBuffer ();
                try { hResComServer = m_ptre6Data->ConnectToServer (oleVarPar, 5666); }
	            catch (COleException *err) {
		            iError = err->ReportError ();
	            }
				oleVarPar.Clear ();
                break;
            }
            else
                ;

            iNumSource ++;
        }

		try { hResComServer = m_ptre6Data->CurrentServer (oleVarPar); }
	    catch (COleException *err) {
		    iError = err->ReportError ();
	    }

		if ((iError == 0) && (hResComServer == S_OK) && (strServerName.compareNoCase (oleVarPar.bstrVal) == 0)) {
			/*oleVarPar =  (L"30-ти мин, мгновенные");
			try { hResComServer = m_ptre6Data->CreateTable (oleVarPar, &m_iIdTable); }
			catch (COleException *err) {
				iError = err->ReportError ();
			}*/

			if ((iError == 0) && (hResComServer == S_OK)) {
				strConnectDB = PTR_SETTINGS->CompileConnectDB ((int ) wp, HDBSettings::SQL_DB);

				if (m_ptrEnerg6Data)
					delete m_ptrEnerg6Data;
				else
					;

                HEnerg5Data *ptrEnerg5Data = 0x0;
                HString strConnectDBEnerg5Data;
                HWinThread *ptrThread = 0x0; int suspense = 0;
                DWORD dwWaitRes = WaitForSingleObject (((CPseudoKTSApp *) AfxGetApp ())->m_hEnterDirectoryWatched, 6666L);
                /*while ((ptrThread == 0x0) && (suspense < 666)) {
                    ptrThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_DIRECTORYWATCHED);
                    suspense += 66;
                    Sleep (66);
                }*/
                if (dwWaitRes == 0)
                    ptrThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_DIRECTORYWATCHED);
                else
                    ;

                if (!(ptrThread == 0x0)) {
                    strConnectDBEnerg5Data.format ("DATABASE=%s;", MB_HSTRING (ptrThread->stringDataOfId (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED)));
                    ptrEnerg5Data = DEBUG_NEW HEnerg5Data (MB_HSTRING (strConnectDBEnerg5Data));
                }
                else {
                    //Потока 'HThreadDirectoryWatched' еще не существует
                    strLog.format ("Потока 'HThreadDirectoryWatched' еще не существует...");
                }

                if (! (ptrEnerg5Data == 0x0))
                {
                    m_ptrEnerg6Data = DEBUG_NEW HEnerg6Data (MB_HSTRING (tokenizeConnectDB (lp, strConnectDB)));

				    RecordChannelInfoTotal *ptrRecCIT = 0x0;
				    if (PTR_DB_DATA (m_ptrEnerg6Data)->IsConnectDB ()) {
					    //int numUSD = -1;
                        m_ptrEnerg6Data->SetClassVal (15);
					    m_ptrEnerg6Data->GetInfoTotalOfChannel ();
					    for (int num = 1; ! (num > 512); num ++) {
						    ptrRecCIT = m_ptrEnerg6Data->GetRecordCIT (num);
						    if (ptrRecCIT) {
                                ptrEnerg5Data->GetInfoTotalOfChannel (num);
                                //TRACE ("Кан. №%i: VER6 {KI=%i, KU=%i, KR=%f, Razm=%i}; VER5 {KI=%f, KU=%f, KR=%f, Razm=%i}\n", num,
                                //        ptrRecCIT->iKI_6, ptrRecCIT->iKU_6, ptrRecCIT->rci.sKR, ptrRecCIT->Razm_6,
                                //        ptrEnerg5Data->GetRecordCIT (0)->rci.sKI, ptrEnerg5Data->GetRecordCIT (0)->rci.sKU, ptrEnerg5Data->GetRecordCIT (0)->rci.sKR, ptrEnerg5Data->GetRecordCIT (0)->rci.Razm_5);
                                ptrRecCIT->rci = ptrEnerg5Data->GetRecordCIT (0)->rci;

                                if (ptrRecCIT->iOwnerNumberUSD > 0) {
                                    map <int , long>::iterator iterIdTable = m_ariIdTable.find (ptrRecCIT->iOwnerNumberUSD);
                                    if (iterIdTable == m_ariIdTable.end ()) {
                                        //Добавим новую таблицу
                                        m_ariIdTable [ptrRecCIT->iOwnerNumberUSD] = -1;
                                        strVarPar.format (L"30-ти мин, мгновенные, УСД №%i", ptrRecCIT->iOwnerNumberUSD);
                                        oleVarPar = WC_HSTRING (strVarPar);
                                        try { hResComServer = m_ptre6Data->CreateTable (oleVarPar, &m_ariIdTable [ptrRecCIT->iOwnerNumberUSD]); }
			                            catch (COleException *err) {
				                            iError = err->ReportError ();
			                            }
                                    }
                                    else
                                        ;
							        m_mapData [ptrRecCIT->iOwnerNumberUSD] [num] = DEBUG_NEW DATA_RESPONSE;
							        hResComServer = m_ptre6Data->addObj (m_ariIdTable [ptrRecCIT->iOwnerNumberUSD], ptrRecCIT->idVTI_SEC_15, true, &m_mapData [ptrRecCIT->iOwnerNumberUSD] [num]->idComClient);
                                }
                                else
                                    ;
						    }
						    else
							    ;
					    }
					    PTR_DB_DATA (m_ptrEnerg6Data)->Close ();

                        if (! (GetUserNumberPTICControl () > 0))
                            //Запросить номер канала для обновления окна 'DlgGUIControl'
                            if (((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GUI_CONTROL)) ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_GUI_CONTROL)->PostThreadMessage (WM_USER_QUERY_NUMBER_PTIC_CONTROL, m_nThreadID, 0); else ;
                        else ;
				    }
				    else {
					    //Не удалось создать соединение с SQLServer-ом
					    strLog.format ("Не удалось создать соединение с SQLServer-ом: %s", MB_HSTRING (strConnectDB));

					    delete m_ptrEnerg6Data;
					    m_ptrEnerg6Data = 0x0;
				    }

                    delete ptrEnerg5Data;
                }
                else
                    ; //ptrEnerg5Data = 0x0 ??? => не получен указатель на THREAD_DIRECTORYWATCHED ???
			}
			else
				//Не удалось создать таблицу для запрса данных от СДД
				strLog.format (_T ("%S"), _T ("Не удалось создать таблицу для запрса данных от СДД"));
		}
		else
			//Нет соединения к СДД
			strLog.format (_T ("%S"), _T ("Нет соединения к СДД"));
	}
    else
		//Нет НИ ОДНого имени СДД для подключения
        strLog.format (_T ("%S"), _T ("Нет НИ ОДНого имени СДД для подключения"));

	if ((! (iError == 0)) || (! (hResComServer == S_OK)) || (m_ptrEnerg6Data == 0x0)) {
		strLog.insert (0, _T ("HThreadGetData::OnConnectToServer - "));
		FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);

		//Сообщить контролирующему потоку, что этот поток завершается
        //::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_USER_THREAD_GUI_CLOSE, hResComServer, 0);
        //Завершить поток
		//PostThreadMessage (WM_QUIT, hResComServer, 0);
	}
	else
		;
}

LONGLONG getNumberInterval (__time64_t tm64t, LONGLONG interval = 300, LONGLONG ofInterval = 30 * 60) {
    LONGLONG lRemained = tm64t % ofInterval, lRes = 1;

    if (lRemained == 0)
        lRes = ofInterval / interval;
    else
        if (lRemained % interval)
            lRes = (lRemained / interval) + 1;
        else
            lRes = lRemained / interval;

    return lRes;
}

void roundDblToPrecision (double &val, int precision) {
    double pow10 = pow (10.0, precision);

    val = ((LONGLONG) (val * pow10)) / pow10;
}

afx_msg void HThreadGetData::OnMessageTimer (WPARAM wp, LPARAM lp) {
    //m_pDlgGUI->TimeIndicators ();

    FILELOG_VARIABLE

    SYSTEMTIME stmEntry, stmExit;
	DATE dtQuery; long lScaleQuery;
	__time64_t tm64tReal = 0x0, tm64tQuery = 0; short remaindSec5 = 0; //Для округления к '5 sec' tm64tQuery
	//VALUE_TYPE valueType [COUNT_QUERY_TIME]; for (QUERY_TIME idValueType = SECOND_15; idValueType < COUNT_QUERY_TIME; idValueType ++) valueType [idValueType] = CURRENT;
    STATE_DATA stateData [COUNT_QUERY_TIME]; /*Предполагаем, что ВСЕ данные НЕ готовы*/ for (INDEX_TIME indxTime = MINUTE_05; indxTime < COUNT_QUERY_TIME; indxTime ++) stateData [indxTime] = UNKNOWN;
    RecordChannelInfoTotal *ptrRecCIT = 0x0;
	HRESULT hResComServer = 0x0;
    int iCountPassed = 0; //Количество проходов опроса 15-ти сек. значений
    DATA_RESPONSE dataResponce; //для отправки сообщения 'HThreadGUIControl'

    map <int, map <int, DATA_RESPONSE *>>::iterator iteratorUSDData;
    map <int, DATA_RESPONSE *>::iterator iteratorData;

    enum INDEX_ERROR {CIT_NULL = 1, TIME_NULL, TIME_REQUERY, COUNT_INDEX_ERROR};
    int array_error [COUNT_PASSED_SEC_15] [COUNT_INDEX_ERROR];
    memset (array_error, 0x0, sizeof (int) * COUNT_PASSED_SEC_15 * COUNT_INDEX_ERROR);

    time (&tm64tReal);
    tm64tReal -= PTR_DATETIMEINTERVAL->tm64tBias ();
	GetLocalTime (&stmEntry);

#if defined _DEBUG
	DATA_RESPONSE *ptrDataResponse = 0x0;
	SYSTEMTIME stmReal, stmQuery; /*DBTIMESTAMP dbtstmpQuery;*/
    CTime (tm64tReal).GetAsSystemTime (stmReal);
	double dblRes = 0.0;
#else
#endif

	if (m_mapData.size ()) {
		strLog.format (_T ("%Sразмер = %i"), _T ("Обработка сообщения 'WM_TIMER'..."), m_mapData.size ());
		FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);
		//TRACE ("HThreadGetData::OnMessageTimer: Вход... (m_mapData.size () = %i)\n", m_mapData.size ());

		switch (wp) {
			case 5:
                /*for (iteratorUSDData = m_mapData.begin (); (m_iStop == 0) && (! (iteratorUSDData == m_mapData.end ())); ++ iteratorUSDData) {
                    hResComServer = m_ptre6Data->restartTable (m_ariIdTable [(*iteratorUSDData).first]);

                    Sleep (166);
                }*/

                iCountPassed = 0;
                while (iCountPassed < COUNT_PASSED_SEC_15) {

                    strLog.format (_T ("%S(PASSED = %i)"), _T ("Обработка сообщения 'WM_TIMER'..."), iCountPassed);
		            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);
		            //TRACE ("HThreadGetData::OnMessageTimer: Вход... (m_mapData.size () = %i)\n", m_mapData.size ());

				    for (iteratorUSDData = m_mapData.begin (); (m_iStop == 0) && (! (iteratorUSDData == m_mapData.end ())); ++ iteratorUSDData) {
                        for (iteratorData = (*iteratorUSDData).second.begin (); (m_iStop == 0) && (! (iteratorData == (*iteratorUSDData).second.end ())); ++ iteratorData) {
					        ptrRecCIT = m_ptrEnerg6Data->GetRecordCIT ((*iteratorData).first);
                            if (ptrRecCIT) {
                                (*iteratorData).second->arDblValues [SECOND_15] [PREVIOUS] = (*iteratorData).second->arDblValues [SECOND_15] [CURRENT];
                                (*iteratorData).second->arWordImpulses [SECOND_15] [PREVIOUS] = (*iteratorData).second->arWordImpulses [SECOND_15] [CURRENT];

                                //TRACE ("Запрс данных getData2.exe: для %i\n", m_ariIdTable [(*iteratorUSDData).first]);

                                hResComServer = m_ptre6Data->objValR (m_ariIdTable [(*iteratorUSDData).first],
															            (*iteratorData).second->idComClient, //m_mapData [(*iteratorData).first]->idComClient,
															            &dtQuery,
															            &(*iteratorData).second->lValScale,
															            &(*iteratorData).second->arDblValues /*[VALUE]*/ [SECOND_15] [CURRENT]);

					            tm64tQuery = static_cast <__time64_t> ((dtQuery - 25569) * 86400);
                                tm64tQuery -= 3600;

                                /*strLog.format ("Опрос канала №%i (%i) {%f, %i}", (*iteratorData).first, (*iteratorData).second->idComClient, (float) dtQuery, tm64tQuery);
		                        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);*/
#if defined _DEBUG
					            //CTime (tm64tQuery).GetAsSystemTime (stmReal);
					            CTime (tm64tQuery).GetAsSystemTime (stmQuery);
#else
#endif
                                if (abs (tm64tReal - tm64tQuery) < (5 * 60)) {
						            //if (! ((*iteratorData).second->arDblValues [SECOND_15] [CURRENT] == 0.0)) {
							            if (! (tm64tQuery == (*iteratorData).second->tm64tQuery)) {
                                            for (INDEX_TIME indxTime = MINUTE_05; indxTime < COUNT_QUERY_TIME; indxTime ++) { if (stateData [indxTime] == UNKNOWN) stateData [indxTime] = READY; else ; }

                                            //TRACE ("В наличии посылка за %i", (*iteratorData).second->tm64tQuery); TRACE (" = %i", (int) lPrevNumberInterval);
                                            //TRACE (", будет получена за %i", tm64tQuery/* + 15*/); TRACE (" = %i\n", (int) lNextNumberInterval);

                                            LONGLONG lPrevNumberInterval = -1, lNextNumberInterval = -1;

                                            lPrevNumberInterval = getNumberInterval ((*iteratorData).second->tm64tQuery, 1 * 60, 30 * 60),
                                            lNextNumberInterval = getNumberInterval (tm64tQuery/* + 15*/, 1 * 60, 30 * 60);
                                            if (((*iteratorData).second->tm64tQuery) && (! (lPrevNumberInterval == lNextNumberInterval))) {
                                                (*iteratorData).second->stateData [MINUTE_01] [CURRENT] = READY;

                                                (*iteratorData).second->IncNumberInterval (MINUTE_01);

                                                lPrevNumberInterval = getNumberInterval ((*iteratorData).second->tm64tQuery, 3 * 60, 30 * 60),
                                                lNextNumberInterval = getNumberInterval (tm64tQuery/* + 15*/, 3 * 60, 30 * 60);
                                                if (((*iteratorData).second->tm64tQuery) && (! (lPrevNumberInterval == lNextNumberInterval))) {
                                                    (*iteratorData).second->stateData [MINUTE_03] [CURRENT] = READY;

                                                    (*iteratorData).second->IncNumberInterval (MINUTE_03);
                                                }
                                                else ;

                                                lPrevNumberInterval = getNumberInterval ((*iteratorData).second->tm64tQuery, 5 * 60, 30 * 60);
                                                lNextNumberInterval = getNumberInterval (tm64tQuery/* + 15*/, 5 * 60, 30 * 60);
                                                if (((*iteratorData).second->tm64tQuery) && (! (lPrevNumberInterval == lNextNumberInterval))) {
                                                    (*iteratorData).second->stateData [MINUTE_05] [CURRENT] = READY;

                                                    (*iteratorData).second->IncNumberInterval (MINUTE_05);

                                                    if ((! (getNumberInterval ((*iteratorData).second->tm64tQuery, 30 * 60, 60 * 60) == getNumberInterval (tm64tQuery, 30 * 60, 60 * 60)))) {
                                                        (*iteratorData).second->stateData [MINUTE_30] [CURRENT] = READY;

                                                        (*iteratorData).second->IncNumberInterval (MINUTE_30);

                                                        if ((! (getNumberInterval ((*iteratorData).second->tm64tQuery, 60 * 60, 24 * 60 * 60) == getNumberInterval (tm64tQuery, 60 * 60, 24 * 60 * 60)))) {
                                                            (*iteratorData).second->stateData [HOUR_01] [CURRENT] = READY;
                                                
                                                            (*iteratorData).second->IncNumberInterval (HOUR_01);

                                                            /*if ((! (getNumberInterval ((*iteratorData).second->tm64tQuery, 24 * 60 * 60, GetCountDayInYearMounth (stmQuery.wYear, stmQuery.wMonth) * 24 * 60 * 60) == getNumberInterval (tm64tQuery, 24 * 60 * 60, GetCountDayInYearMounth (stmQuery.wYear, stmQuery.wMonth) * 24 * 60 * 60)))) {
                                                                (*iteratorData).second->IncNumberInterval (DAY_01);
                                                            }
                                                            else */;
                                                        }
                                                        else ;
                                                    }
                                                    else ;
                                                }
                                                else
                                                    /*(*iteratorData).second->stateData [MINUTE_05] [CURRENT] = WAIT*/;
                                            }
                                            else
                                                ;

							                roundDblToPrecision ((*iteratorData).second->arDblValues [SECOND_15] [CURRENT], 4);
                                            
                                            (*iteratorData).second->tm64tQuery = tm64tQuery;

							                //ОбщЕЕ - ПОЗДНее время опроса
                                            if (m_tm64tQuery == 0)
								                m_tm64tQuery = tm64tQuery;
							                else
								                if ((m_tm64tQuery - tm64tQuery) < 0)
									                m_tm64tQuery = tm64tQuery;
								                else ;

                                            (*iteratorData).second->arWordImpulses [SECOND_15] [CURRENT] = floor (((((*iteratorData).second->arDblValues [SECOND_15] [CURRENT] - ptrRecCIT->rci.sKR) * ptrRecCIT->iKI_6 / ptrRecCIT->iKU_6)) + 0.5);

                                            (*iteratorData).second->stateData [SECOND_15] [CURRENT] = READY;

							                for (INDEX_TIME id_type_tm = MINUTE_01; id_type_tm < COUNT_QUERY_TIME; id_type_tm ++) {
                                                (*iteratorData).second->indxdttmCounter [SECOND_15] [id_type_tm] [CURRENT] ++;
                                                (*iteratorData).second->arDblSummaryValues [id_type_tm] [CURRENT] += (*iteratorData).second->arDblValues [SECOND_15] [CURRENT];

                                                if (m_ptrEnerg6Data->IsAccumulationMeasure (ptrRecCIT->idUnitGrp) == 0) {
								                    //Среднее значение
								                    (*iteratorData).second->arDblValues [id_type_tm] [CURRENT] = (*iteratorData).second->arDblSummaryValues [id_type_tm] [CURRENT] /
                                                                                                                         ((*iteratorData).second->indxdttmCounter [SECOND_15] [id_type_tm] [CURRENT] - 1);
							                    }
							                    else {
								                    (*iteratorData).second->arDblValues [id_type_tm] [CURRENT] = (*iteratorData).second->arDblSummaryValues [id_type_tm] [CURRENT];
							                    }

                                                (*iteratorData).second->arWordImpulses [id_type_tm] [CURRENT] += (*iteratorData).second->arWordImpulses [SECOND_15] [CURRENT];
                                                //(*iteratorData).second->arWordImpulses [id_type_tm] [CURRENT] += floor (((((*iteratorData).second->arDblValues [id_type_tm] [CURRENT] - ptrRecCIT->rci.sKR) * ptrRecCIT->iKI_6 / ptrRecCIT->iKU_6)) + 0.5);
                                            }

                                            (*iteratorData).second->stateData [SECOND_15] [PREVIOUS] = SAVE;

                                            for (INDEX_TIME indxTime = MINUTE_05; indxTime < COUNT_QUERY_TIME; indxTime ++) { if (stateData [indxTime] > (*iteratorData).second->stateData [indxTime] [CURRENT]) stateData [indxTime] = (*iteratorData).second->stateData [indxTime] [CURRENT]; else ; }

                                            if ((*iteratorData).first == m_iUserNumberPTICControl) {
                                                if (GetThreadIDResponce () > 0) {
                                                    dataResponce = *(*iteratorData).second;
                                                    ::PostThreadMessage (GetThreadIDResponce (), WM_USER_SEC_15_THREADGETDATA, (WPARAM) &dataResponce, 0);

                                                    strLog.format (_T ("Отправлено WM_USER_SEC_15_THREADGETDATA для %i"), GetThreadIDResponce ());
                                                    FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);
                                                }
                                                else
                                                    //Нет идентификатора потока для отправки сообщения
                                                    QueryThreadIdResponce (CPseudoKTSApp::THREAD_GUI_CONTROL);
                                            }
                                            else ; //Не искомый номер канала
                                        }
                                        else {
                                            //Время опроса совпадает с временем получения предыдущего значения
                                            array_error [iCountPassed] [TIME_REQUERY] ++;
                                        }
						            //}
						            //else ; //Условие на '0.0' значения
                                }
                                else
                                {
                                    //Некорректное ВРЕМя - возможно канал имеет состояние <НЕТ ДАННЫХ>                                    
                                    //strLog.format ("Разница между текущим и полученным дата/время для %i = %i", iCountPassed, abs (tm64tReal - tm64tQuery));
                                    //FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
                                    array_error [iCountPassed] [TIME_NULL] ++;
                                }
					        }
					        else
						        //Канала с номером '.first' не существует
						        array_error [iCountPassed] [CIT_NULL] ++;

                            //Sleep (16);
                        } //for
				    } //for

                    for (int indxError = CIT_NULL; indxError < COUNT_INDEX_ERROR; indxError ++) array_error [iCountPassed] [0] += array_error [iCountPassed] [indxError];

                    strLog.format ("Ошибки при опросе (PASSED=%i, %i): CIT_NULL = %i, TIME_NULL = %i, TIME_REQUERY = %i.", iCountPassed, array_error [iCountPassed] [0],
                                                                                                                                array_error [iCountPassed] [CIT_NULL],
                                                                                                                                array_error [iCountPassed] [TIME_NULL],
                                                                                                                                array_error [iCountPassed] [TIME_REQUERY]);
                    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);

                    iCountPassed ++;
                } //while COUNT_PASSED

				/*for (iteratorUSDData = m_mapData.begin (); (m_iStop == 0) && (! (iteratorUSDData == m_mapData.end ())); ++ iteratorUSDData) {
                    hResComServer = m_ptre6Data->pauseTable (m_ariIdTable [(*iteratorUSDData).first]); 
                }*/

                remaindSec5 = m_tm64tQuery % 5;
				if (remaindSec5 > 2)
					m_tm64tQuery += (5 - remaindSec5);
				else
					m_tm64tQuery -= remaindSec5;

				//strLog.format ("HThreadGetData::OnMessageTimer: Остаток [MINUTE_05] = %i", m_tm64tQuery % PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("MINUTE_05"))));
				//FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG /*THREAD_GETDATA*/);
				//TRACE ("HThreadGetData::OnMessageTimer: Остаток [MINUTE_05] = %i\n", m_tm64tQuery % PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("MINUTE_05"))));

				//m_tm64tQuery = CTime (dtQuery).GetTime ();
				//if (abs (PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("MINUTE_05"))) - m_tm64tQuery % PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("MINUTE_05")))) < 6)
				//if (m_tm64tQuery % PTR_DATETIMEINTERVAL->GetInterval (HString (_T ("MINUTE_05"))) == 0)
                for (INDEX_TIME indxTime = MINUTE_05; indxTime < COUNT_QUERY_TIME; indxTime ++) {
                    if ((stateData [indxTime] == READY) && (! (stateData [indxTime] == UNKNOWN)))
                        PostThreadMessage (WM_USER_READY_DATA, indxTime, 0);
				    else
                        ;
                }

				break;
            case 10:
                break;
            case 15:
                break;
            case 30:
                for (iteratorUSDData = m_mapData.begin (); (m_iStop == 0) && (! (iteratorUSDData == m_mapData.end ())); ++ iteratorUSDData) {
                    hResComServer = m_ptre6Data->restartTable (m_ariIdTable [(*iteratorUSDData).first]); 
                }
                break;
            case 1 * 60:
				break;
			case 3 * 60:
				break;
			case 5 * 60:
				break;
			case 30 * 60:
				break;
			case 1 * 60 * 60:
				break;
			default: ;
		}
	}
	else
		;

	GetLocalTime (&stmExit);
}

afx_msg void HThreadGetData::OnResetThreadResponce (WPARAM , LPARAM ) {
    m_iUserNumberPTICControl = 0x0;
    
    HWinThread::OnResetThreadResponce (0x0, 0x0);
}

void roundTime64_t (__time64_t &tm64t, LONGLONG interval) {
    LONGLONG lRemained = tm64t % interval;
    if (lRemained > ceil ((double) interval / 2))
        tm64t += (interval - lRemained);
    else
        tm64t -= lRemained;
}

//Обрабатывает ЛОКАЛьные сообщения
afx_msg void HThreadGetData::OnMessageReadyData (WPARAM wp, LPARAM ) {
	if (m_iStop == 0x0) {
        FILELOG_VARIABLE

        INDEX_TIME indxTime = (INDEX_TIME) wp;
        int minutes = -1,
            id_TIME64T_QUERY = -1, id_PTR_IMP = -1;

	    RecordChannelInfoTotal *ptrRecCIT = 0x0;
        //DATA_RESPONSE *dataResponse = 0x0;

	    /*switch (WaitForSingleObject (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::GETDATA_05_MIN), 6)) {
		    case WAIT_ABANDONED:
		    case WAIT_TIMEOUT:
			    break;
		    case WAIT_OBJECT_0:
			    ResetEvent (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::GETDATA_05_MIN));
			    break;
		    default: ;
	    }*/

	    for (map <int, map <int , DATA_RESPONSE *>>::iterator iteratorUSDData = m_mapData.begin (); ! (iteratorUSDData == m_mapData.end ()); ++ iteratorUSDData) {
            for (map <int , DATA_RESPONSE *>::iterator iteratorData = (*iteratorUSDData).second.begin (); (m_iStop == 0) && (! (iteratorData == (*iteratorUSDData).second.end ())); ++ iteratorData) {
                (*iteratorData).second->stateData [indxTime] [PREVIOUS] = SAVE;
                (*iteratorData).second->stateData [indxTime] [CURRENT] = /*SAVE*/WAIT;

		        m_arImp [indxTime] [(*iteratorData).first - 1] = (*iteratorData).second->arWordImpulses [indxTime] [PREVIOUS];
            }
	    }

	    //По хорошему необходимо сделать копию (::PostThreadMessage (idThreadWriteData, &m_arImp [indxTime]))
	    __time64_t tm64tQuery = m_tm64tQuery + PTR_DATETIMEINTERVAL->tm64tBias ();
        switch (indxTime) {
            case MINUTE_05:
                minutes = 5;
                id_TIME64T_QUERY = TIME64T_QUERY_MIN_05, id_PTR_IMP = PTR_IMP_MIN_05;
                break;
            case MINUTE_30:
                minutes = 30;
                id_TIME64T_QUERY = TIME64T_QUERY_MIN_30, id_PTR_IMP = PTR_IMP_MIN_30;
                break;
            default: ;
        }

        if (minutes > 0) {
            roundTime64_t (tm64tQuery, minutes * 60);
	        AddData (id_TIME64T_QUERY, &tm64tQuery, sizeof (__time64_t));
	        AddData (id_PTR_IMP, m_arImp [indxTime], 512 * sizeof (WORD));
	        //SetEvent (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::GETDATA_05_MIN));

	        memset (m_arImp [indxTime], 0x0, 512 * sizeof (WORD));
        }
        else
            ;

        //Для формы 'Контроль значений'
        PostDataToControl (indxTime, 0);
        PostDataToControl (indxTime, 1);

        //Остаток до окончания 30-ти мин. интервала
        strLog.format ("HThreadGetData::OnMessage05MinData: остаток [%s] = %i", MB_HSTRING (PTR_DATETIMEINTERVAL->GetNameInterval (indxTime)), m_tm64tQuery % PTR_DATETIMEINTERVAL->GetInterval (indxTime));
	    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG /*THREAD_GETDATA*/);
    }
    else
        ;
} 

//Обрабатывает сообщения от 'HThreadWriteData' (отправленные данные записаны - их можно считывать)
afx_msg void HThreadGetData::OnMessageWriteData (WPARAM wp, LPARAM) {
    int id_TIME64T_QUERY = -1, id_PTR_IMP = -1;

    switch (wp) {
        case MINUTE_05:
            id_TIME64T_QUERY = TIME64T_QUERY_MIN_05, id_PTR_IMP = PTR_IMP_MIN_05;
            break;
        case MINUTE_30:
            id_TIME64T_QUERY = TIME64T_QUERY_MIN_30, id_PTR_IMP = PTR_IMP_MIN_30;
            break;
        default: ;
    }

    DelData (id_TIME64T_QUERY); DelData (id_PTR_IMP);

    if (m_iStop == 0x0) PostDataToControl ((INDEX_TIME) wp, TRUE); else ;
}

afx_msg void HThreadGetData::OnMessageUserNumberPTICControl (WPARAM wp, LPARAM ) {
    FILELOG_VARIABLE

    if (m_iStop == 0) {
        m_iUserNumberPTICControl = (WORD) wp;

        strLog.format ("HThreadGetData::OnMessageUserNumberPTICControl: получен ПТИК №=%i", m_iUserNumberPTICControl);
	    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG /*THREAD_GETDATA*/);

        if (GetThreadIDResponce () > 0) {
            if (m_iUserNumberPTICControl > 0) {
                if (! (m_ptrEnerg6Data == 0x0)) {
                    RecordChannelInfoTotal *ptrCIT = m_ptrEnerg6Data->GetRecordCIT (m_iUserNumberPTICControl);
                    if (! (ptrCIT == 0x0)) {
                        //strLog.format ("HThreadGetData::OnMessageUserNumberPTICControl: возвращен ptrCIT...");
	                    //FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);

                        map <int, map <int , DATA_RESPONSE *>>::iterator iteratorUSDData = m_mapData.find (ptrCIT->iOwnerNumberUSD);

                        //strLog.format ("HThreadGetData::OnMessageUserNumberPTICControl: ptrCIT->iOwnerNumberUSD = %i", ptrCIT->iOwnerNumberUSD);
	                    //FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);

                        if (! (iteratorUSDData == m_mapData.end ())) {
                            //strLog.format ("HThreadGetData::OnMessageUserNumberPTICControl: найдено УСД...");
	                        //FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);

                            map <int, DATA_RESPONSE *>::iterator iteratorData = (*iteratorUSDData).second.find (m_iUserNumberPTICControl);
                            if (! (iteratorData == (*iteratorUSDData).second.end ())) {
                                ::PostThreadMessage (GetThreadIDResponce (), WM_USER_SEC_15_THREADGETDATA, (WPARAM) (*iteratorData).second, 0);

                                //Для формы 'Контроль значений'
                                for (INDEX_TIME indxTime = MINUTE_05; indxTime < DAY_01; indxTime ++) {
                                    PostDataToControl (indxTime, 0); //Отправить НЕ свои данные
                                    PostDataToControl (indxTime, 1); //Отправить СВОИ данные
                                }
                            }
                            else
                                ; // Не найдены для отправления данных по каналу с № 'm_iUserNumberPTICControl'
                        }
                        else
                            ; // Не найден УСД для отправления данных по каналу с № 'm_iUserNumberPTICControl'
                    }
                    else
                    {
                        FILELOG_IF_WRITE (HERROR)
	                    strLog.format ("HThreadGetData::OnMessageUserNumberPTICControl; ptrCIT == 0x0; канал №%i", m_iUserNumberPTICControl);
	                    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
	                    FILELOG_ENDIF

                        ::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_QUIT, 0, 0);
                    }
                }
                else {
                    FILELOG_IF_WRITE (HERROR)
	                strLog.format ("HThreadGetData::OnMessageUserNumberPTICControl; m_ptrEnerg6Data == 0x0; канал №%i", m_iUserNumberPTICControl);
	                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
	                FILELOG_ENDIF

                    ::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_QUIT, 0, 0);
                }
            } else {
                QueryNumberPTICControl ();
            }
        }
        else
            //Нет идентификатора потока для отправки сообщения
            QueryThreadIdResponce (CPseudoKTSApp::THREAD_GUI_CONTROL);
    }
    else
        ;
}

int HThreadGetData::PostDataToControl (INDEX_TIME indxTime, BOOL bPseudo) {
    FILELOG_VARIABLE

    int iRes = 0,
        minutes = 0,
        id_message;

    switch (indxTime) {
        case MINUTE_05:
            minutes = 5;
            break;
        case MINUTE_30:
            minutes = 30;
            break;
        case HOUR_01:
            minutes = 60;
            break;
        default: ;
    }

    if (((CPseudoKTSApp *) AfxGetApp ())->m_bDestroyUIThreads == FALSE) {
        if (GetThreadIDResponce () > 0) {
            if (m_iUserNumberPTICControl > 0) {
                HString strConnectDBEnerg5Data;
                HWinThread *ptrThread = ((CPseudoKTSApp *) AfxGetApp ())->GetPtrThread (CPseudoKTSApp::THREAD_DIRECTORYWATCHED);
                strConnectDBEnerg5Data.format ("DATABASE=%s;", MB_HSTRING (ptrThread->stringDataOfId (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED)));
                if (bPseudo == TRUE) {
                    id_message = WM_USER_ENG_PSEUDO_THREADGETDATA;
                }
                else
                    if (bPseudo == FALSE) {
                        strConnectDBEnerg5Data.replace ("ENG5", "ENG2");
                        id_message = WM_USER_ENG_ORIGIN_THREADGETDATA;
                    }
                    else
                        ;
                HEnerg5Data *ptrEnerg5Data = DEBUG_NEW HEnerg5Data (MB_HSTRING (strConnectDBEnerg5Data));

                void *ptrData = DEBUG_NEW BYTE [2 * sizeof (void *)];
                *((WORD **) ptrData + 0) = DEBUG_NEW WORD [1];
                *((double **) ptrData + 1) = DEBUG_NEW double [1];

                HString strUserNumberPTICControl; strUserNumberPTICControl.appendNumber ((DWORD) m_iUserNumberPTICControl);
                *(*((WORD **) ptrData + 0)) = ptrEnerg5Data->GetImpulses (MB_HSTRING (strUserNumberPTICControl), indxTime);

                if (! (*(*((WORD **) ptrData + 0)) < 0)) {
                    /*iRes = */ptrEnerg5Data->GetInfoTotalOfChannel (m_iUserNumberPTICControl);

                    *(*((double **) ptrData + 1)) = ptrEnerg5Data->GetValueOfImpulses (m_iUserNumberPTICControl, *(*((WORD **) ptrData + 0)), &ptrEnerg5Data->GetRecordCIT (0)->rci, minutes * 60);

                    ::PostThreadMessage (GetThreadIDResponce (), id_message, (WPARAM) indxTime, (LPARAM) ptrData);

                    FILELOG_IF_WRITE (HDEBUG)
	                strLog.format ("HThreadGetData::PostDataToControl () - to thread with id = %i", GetThreadIDResponce ());
	                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);
	                FILELOG_ENDIF
                }
                else {
                    delete *((WORD **) ptrData + 0);
                    delete *((double **) ptrData + 1);

                    delete (void *) ptrData;
                }

                delete ptrEnerg5Data;
            } else { //Нет номера канала
                QueryNumberPTICControl ();
            }
        } else { //Нет идентификатора потока для отправки сообщения
            //Запрос для получения такого идентификатора
            QueryThreadIdResponce (CPseudoKTSApp::THREAD_GUI_CONTROL);
        }
    }
    else
        iRes = -1;

    return iRes;
}

BOOL HThreadGetData::QueryNumberPTICControl () {
    return ::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_USER_QUERY_NUMBER_PTIC_CONTROL, m_nThreadID, 0x0);
}
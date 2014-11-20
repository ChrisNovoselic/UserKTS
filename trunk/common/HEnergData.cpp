#include "stdafx.h"
#include "HEnergData.h"

EXT_DECLARE_HTHREAD_FILELOG

//#define MAP_INIT(map_obj, rec_name) map_obj.insert (* (DEBUG_NEW std::pair <int, rec_name *> (0, DEBUG_NEW rec_name ())));

HEnergData::HEnergData (void) : HData () {
    //m_ptrRecCIT = DEBUG_NEW RecordChannelInfoTotal;
    //m_ptrRecGIT = DEBUG_NEW RecordGroupInfoTotal;

    //m_mapCIT [0] = DEBUG_NEW RecordChannelInfoTotal ();
    //m_mapGIT [0] = DEBUG_NEW RecordGroupInfoTotal ();

    InitializeCriticalSection (&m_CriticalSection);

    //Утечка памяти _crtBreakAlloc - переменная, назначив значение которой, остановимся при отладке
    // в тот момент, когда произойдет NNN-ое выделение памяти, где NNN - значение переменной _crtBreakAlloc
    //http://www.rsdn.ru/article/vcpp/leaks.xml 
    //try { MAP_INIT (m_mapCIT, RecordChannelInfoTotal) }
    //catch (...) {
    //}
    //MAP_INIT (m_mapGIT, RecordGroupInfoTotal)

    m_mapCIT [0] = new RecordChannelInfoTotal ();
    m_mapGIT [0] = new RecordGroupInfoTotal ();
}

HEnergData::~HEnergData(void) {
    //delete &m_ptrRecCIT [511];
    //delete &m_ptrRecGIT [511];

    DeleteCriticalSection (&m_CriticalSection);

    for (map <int, RecordChannelInfoTotal *>::const_iterator iteratorCIT = m_mapCIT.begin (); iteratorCIT != m_mapCIT.end (); iteratorCIT ++) {
        delete (*iteratorCIT).second;
    }
    m_mapCIT.clear ();

    for (map <int, RecordGroupInfoTotal *>::const_iterator iteratorGIT = m_mapGIT.begin (); iteratorGIT != m_mapGIT.end (); iteratorGIT ++) {
        delete (*iteratorGIT).second;
    }
    m_mapGIT.clear ();
}

int HEnergData::SetValueParametrVyvod (CTime tm, int period) {
    char *m_arEnergyUserNumberParametrVyvod [] = { "465", "466", "2", "4", "-1", "-1",
                                                "467", "468", "18", "20", "-1", "-1",
                                                "469", "470", "6", "8", "-1", "-1",
                                                "471", "472", "22", "24", "-1", "-1",
                                                "473", "474", "-1", "-1", "-1", "-1",
                                                "-1", "-1", "-1", "-1", "-1", "-1",
                                                "245", "-1", "241"};
    
    /*int i, j, k;
    TCHAR *key;
    
    int num = -1;
    HMapStringAToInt *pMapStringAToInt = NULL;
    HMapStringAToPtr *pMapStringAToPtr = NULL;
    
    CStringArray arStrKeyVyvod, arStrKeyMeasure, arStrKeyDirection;
    
    arStrKeyVyvod.Add (CString ("FIRST"));
    arStrKeyVyvod.Add (CString ("SECOND"));
    arStrKeyVyvod.Add (CString ("THIRD"));
    arStrKeyVyvod.Add (CString ("FOURTH"));
    arStrKeyVyvod.Add (CString ("FIFTH"));
    arStrKeyVyvod.Add (CString ("PERVMAY"));
    arStrKeyVyvod.Add (CString ("FEEDING"));
    
    arStrKeyMeasure.Add (CString ("CONSUMPTION"));
    arStrKeyMeasure.Add (CString ("TEMPERATURE"));
    arStrKeyMeasure.Add (CString ("PRESSURE"));
    
    arStrKeyDirection.Add (CString ("DIRECT"));
    arStrKeyDirection.Add (CString ("REVERSE"));
    
    for (i = 0; i < arStrKeyVyvod.GetSize (); i++)
    {
        pMapStringAToPtr = new HMapStringAToPtr;
        for (j = 0; j < arStrKeyMeasure.GetSize (); j++)
        {
            pMapStringAToInt = new HMapStringAToInt;
            for (k = 0; k < arStrKeyDirection.GetSize (); k++)
            {
                key = CString (arStrKeyDirection.GetAt (k)).GetBuffer ();
                num = m_arEnergyUserNumberParametrVyvod [j + k * 2 + i * 6], pMapStringAToInt->SetAt (key, num);
            }
            key = CString (arStrKeyMeasure.GetAt (j)).GetBuffer ();
            pMapStringAToPtr->SetAt (key, pMapStringAToInt);
        }
        key = CString (arStrKeyVyvod.GetAt (i)).GetBuffer ();
        m_mapParametrVyvod.SetAt (key, pMapStringAToPtr);
    }*/

    for (int curId = CONSUMPTION_DIRECT_FIRST; curId < LAST_INDEX; curId ++)
        m_arEnergyDataParametrVyvod [curId] = GetValueOfChannel (m_arEnergyUserNumberParametrVyvod [curId], tm, period);
    
    return 0; //Success
}

double HEnergData::GetTeploVyvod (void)
{
    int i = 0;
    double dblRes, dblNumerator = 0, dblDenumerator = 0;
    
    for (i = 0; i <= COUNT_VYVOD; i ++)
        dblNumerator += m_arEnergyDataParametrVyvod [CONSUMPTION_DIRECT_FIRST + i * COUNT_PARAMETR_VYVOD] * m_arEnergyDataParametrVyvod [TEMPERATURE_DIRECT_FIRST + i * COUNT_PARAMETR_VYVOD] - m_arEnergyDataParametrVyvod [CONSUMPTION_REVERSE_FIRST + i * COUNT_PARAMETR_VYVOD] * m_arEnergyDataParametrVyvod [TEMPERATURE_REVERSE_FIRST + i * COUNT_PARAMETR_VYVOD];
    
    dblNumerator -= m_arEnergyDataParametrVyvod [CONSUMPTION_DIRECT_FEEDING] * m_arEnergyDataParametrVyvod [TEMPERATURE_WATER];
    
    dblDenumerator = 1000;
    
    dblRes = dblNumerator / dblDenumerator;
    
    if (abs (dblRes) > 6666666666666)
        dblRes = 0;
    else
        ;
    
    return dblRes;
}

double HEnergData::GetTeploVyvodWithPodpitka (void)
{
    int i = 0;
    double dblRes, dblNumerator = 0, dblDenumerator = 0;
    
    //for (i = 0; i < COUNT_VYVOD; i ++)
    for (i = 0; i <= COUNT_VYVOD; i ++)
        dblNumerator += m_arEnergyDataParametrVyvod [CONSUMPTION_REVERSE_FIRST + i * COUNT_PARAMETR_VYVOD] * m_arEnergyDataParametrVyvod [TEMPERATURE_REVERSE_FIRST + i * COUNT_PARAMETR_VYVOD];
    
    for (i = 0; i <= COUNT_VYVOD; i ++)
        dblDenumerator += m_arEnergyDataParametrVyvod [CONSUMPTION_REVERSE_FIRST + i * COUNT_PARAMETR_VYVOD];
    
    dblRes = dblNumerator / dblDenumerator;
    
    dblRes -= m_arEnergyDataParametrVyvod [TEMPERATURE_WATER];
    
    dblRes *= m_arEnergyDataParametrVyvod [CONSUMPTION_DIRECT_FEEDING];
    
    dblRes /= 1000;
    
    return dblRes;
}

double HEnergData::Entalpia (double p, double t)
{
   return (49.4 + 402.5 * t / 100 + 4.767 * pow ((t / 100), 2) +
                0.0333 * pow (t / 100, 6) +
                (-9.25 + 1.67 * t / 100 + 7.36 * pow (10.0, -3) * pow (t / 100, 6) -
                0.008 * pow ((1 / (t / 100 + 0.5)), 5)) * ((50 - p * 0.0980665) / 10) +
                (-0.073 + 0.079 * t / 100 + 6.8 * pow (10.0, -4) * pow ((t / 100), 6)) * pow (((50 - p * 0.0980665) / 10), 2) +
                3.39 * pow (10.0, -8) * pow (1 / 100, 12.0) * pow ((50 - p * 0.0980665) / 10, 4)) / 4.1868;
}

//
//Function Entalpia(p As Single, t As Single) As Single
//    Enatalpia = (49.4 + 402.5 * t / 100 + 4.767 * (t / 100) ^ 2 + _
//                0.0333 * (t / 100) ^ 6 + _
//                (-9.25 + 1.67 * t / 100 + 7.36 * 10 ^ -3 * (t / 100) ^ 6 - _
//                0.008 * (1 / (t / 100 + 0.5)) ^ 5) * ((50 - p * 0.0980665) / 10) + _
//                (-0.073 + 0.079 * t / 100 + 6.8 * 10 ^ -4 * (t / 100) ^ 6) * ((50 - p * 0.0980665) / 10) ^ 2 + _
//                3.39 * 10 ^ -8 * (1 / 100) ^ 12 * ((50 - p * 0.0980665) / 10) ^ 4) / 4.1868
//
//End Function

double HEnergData::GetValueOfImpulses (WORD num, double imp, RecordChannelInfoTotal *prchit, LONGLONG seconds) {
    RecordChannelInfo rchi;

    rchi = prchit->rci;
    
    return GetValueOfImpulses (num, imp, &rchi, seconds);
}

double HEnergData::GetValueOfImpulses (WORD num, double imp, RecordChannelInfo *prchi, LONGLONG seconds) {
    double dblRes = 0.0;

    if (prchi) {
        dblRes = imp;

        dblRes *= prchi->sKI;

	    if ((num < 465) || (num > 474))
	            dblRes *= prchi->sKU;

        if (! (IsAccumulationMeasure (int (prchi->Razm_5)) == 1))
            //Среднее значение
            dblRes = dblRes / ((double) seconds / 3600); //(period / 60) = количество часов
        else ;

        dblRes /= 240; //В каждом часе РОВНО 240 15-ти сек. интервалов

        switch (prchi->Razm_5) {
	        case 101: // Температура
	            if (dblRes != 0.0)
	                dblRes = dblRes + prchi->sKR;
	            break;
	        case 111: // мм.рт.ст.
	            dblRes += + prchi->sKR;
	            break;
	        case 1: // Elektro
	            if (prchi->sKR) {
	                dblRes *=  240;
	                dblRes = dblRes / prchi->sKR;
	            }
                else
                    ;
	            break;
	        default:
	            ;
	    }
    	
	    /*
	    switch (prchi->Razm)
	    {
	        case 39:
	            dblRes /= 240;
	            break;
	        case 100: // %
	            dblRes = dblRes / 240;
	            dblRes = dblRes / ((double) period / 60);
	            break;
	        case 101: // Температура
	            dblRes = dblRes / 240;
	            dblRes = dblRes / ((double) period / 60);
	            if (dblRes != 0.0)
	                dblRes = dblRes + prchi->sKR;
	            break;
	        case 106: // кПа
	            dblRes = dblRes / 240;
	            dblRes = dblRes / ((double) period / 60);
	            break;
	        case 110: // кгс/кв.см
	            dblRes = dblRes / 240;
	            dblRes = dblRes / ((double) period / 60);
	            break;
	        case 111: // мм.рт.ст.
	            dblRes = dblRes / 240;
	            dblRes = dblRes / ((double) period / 60);
	            dblRes +=  + prchi->sKR;
	            break;
	        case 1: // Elektro
	            if (prchi->sKR)
	                dblRes = dblRes / prchi->sKR;
	            else
                    dblRes = dblRes / 240;
	            break;
	        default:
	            ;
	    }
	    */
    }
    else
        ; //Нет данных о канале

    return dblRes;
}

#define GET_RECORD_IT(n, rec_name, map_obj) \
    void *ptrRes = 0x0; \
    FILELOG_VARIABLE \
    EnterCriticalSection (&m_CriticalSection); \
    try { \
        std::map <int, rec_name>::iterator it = map_obj.find (n); \
        if (!(it == map_obj.end ())) ptrRes = (*it).second; else ; \
    } \
    catch (CException *e) { \
        strLog.format ("catch: HEnergData::GET_RECORD_IT: map_obj.find (n); n = %i;", n); \
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR); \
    } \
    LeaveCriticalSection (&m_CriticalSection); \
    return (rec_name) ptrRes;

RecordChannelInfoTotal *HEnergData::GetRecordCIT (int num) {
    GET_RECORD_IT(num, RecordChannelInfoTotal *, m_mapCIT)
}

RecordGroupInfoTotal *HEnergData::GetRecordGIT (int num) {
    GET_RECORD_IT(num, RecordGroupInfoTotal *, m_mapGIT)
}
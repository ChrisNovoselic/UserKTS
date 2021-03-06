#include "stdafx.h"
#include "HTornado1Data.h"

EXT_DECLARE_HTHREAD_FILELOG

HTornado1Data::HTornado1Data (void) : HTornadoData ()
{
}

HTornado1Data::HTornado1Data (CStringA con) : HTornadoData (con) {
    FILELOG_VARIABLE

    InitAccumulationId ();
}

HTornado1Data::~HTornado1Data(void) {
}

//use config
//select rsc.* from dbo.real_sig_cfg rsc, dbo.signals_v sv where rsc.id=sv.id and sv.name like '%N4' + 'X%'
//(ID, MEA_ID)
//1 - Содержание кислорода в дымовых газах (AVG: 0)
//4 - Температура (AVG: 0)
//48 - Давление (AVG: 0)
//21 - Расход газа (SUM: 1)
//45 - Расход пара, питательной воды, продувки (SUM: 1)
//48 - Вакуум в конденсаторе (AVG: 0)
//52 - Атмосферное давление (AVG: 0)

//use history
//select * from dbo.states_real_his_0 where id=1563 and last_changed_at between '07/02/2013 00:00:01' and '07/03/2013'
//(id, value, last_changed_at, tmdelta)

double HTornado1Data::GetValueOfChannel (char *pchSignal, CTime tmBegin, int period) {
    //tmBegin -= 24 *60 * 60; //Для ОТЛАДКи на СУТКи сек. НАЗАД
    double dblRes = 0.0;
    CDBVariant dbData;
    int iRes = 0, idVTI = -1, idUnitGrp = -1, iCountInHours = -1, iCountHours = -1;

    //"SELECT optname, int_value_002 FROM sys_options WHERE optname = 'grinvich' OR 'oper_interval'"
    //sys_options.optname = 'grinvich' int_value_002 = 7
    //sys_options.optname = 'oper_interval' int_value_002 = 60
    tmBegin -= 7 * 60 * 60;
    
    CTime tmEnd = tmBegin + (period * 60), dttmEndTemp;
    //tmEnd -= 1; //КАК в 'TORNADo'
    //tmBegin += 1; ////ОДНа СЕКУНДа к НАЧальНОМу дата/время КАК у ЛЮДей - НЕУжелИ исправилисЬ к 6-му эн./,бл. ??? Прибавляю в другом месте

    FILELOG_VARIABLE
    CStringA strASQL, strATmp, strAtmBegin, strAtmEnd;
    strAtmBegin = tmBegin.Format ("%m/%d/%Y %H:%M:%S");
    strAtmEnd = tmEnd.Format ("%m/%d/%Y %H:%M:%S");
    
    /*strATmp.Format ("HTornado6Data::GetValueOfChannel' на входе %s", pchSignal);
    FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_3);*/
    
    if (! (strlen (pchSignal) == 0)) {
        strASQL.Append ("SELECT rsc.* FROM dbo.real_sig_cfg rsc, dbo.signals_v sv WHERE rsc.id=sv.id AND sv.name LIKE '%");

        //if (strlen (pchSignal) > 6)
        //    strATmp.Format ("'%s%%' ", pchSignal);
        //else
        //    strATmp.Format ("'%s' ", pchSignal);

        strATmp.Format ("%s", pchSignal);

        strASQL.Append (strATmp);
        strATmp.Empty ();

        strASQL.Append ("X%'");

        TRY_RS_OPEN (strASQL)
        strASQL.Empty ();

        if (iRes == 0) {
            if (m_pRS->CanScroll ()) {
                /*try { m_pRS->MoveFirst (); }
                catch (CDBException *err) {
                    TRACE (err->m_strError);
					err->Delete ();
                }*/
            }
            else
                ;
  
            while (! m_pRS->IsEOF ())
            {//ВНИМАНИЕ! Только ОДНа запись!!!
                m_pRS->GetFieldValue ((short) 0, dbData); //dbo.real_sig_cfg.ID
                idVTI = _wtoi ((*dbData.m_pstringW).GetBuffer ());

                m_pRS->GetFieldValue ((short) 1, dbData); //dbo.real_sig_cfg.MEA_ID
                idUnitGrp = _wtoi ((*dbData.m_pstringW).GetBuffer ());
            
                m_pRS->MoveNext ();
            }
        }
        else
            ;
            
        m_pRS->Close ();
        
        if (idVTI > -1) {
            iCountInHours = 0;
            iCountHours = 0;
            
            //strATmp.Format ("ID cигнала %s = %i", pchSignal, idVTI);
            //FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_3);

            dttmEndTemp = tmBegin;
            while (! (tmEnd - dttmEndTemp < 60 * 60)) {
                strASQL = "SELECT AVG(value), COUNT (*) FROM dbo.states_real_his_0 WHERE id=";

                strATmp.Format ("%i", idVTI);
                strASQL.Append (strATmp);
                strATmp.Empty ();

                strASQL.Append (" AND last_changed_at BETWEEN '");
                strAtmBegin = (dttmEndTemp + 1).Format ("%m/%d/%Y %H:%M:%S");
                strASQL.Append (strAtmBegin);
                strASQL.Append ("' AND '");
                dttmEndTemp += 60 * 60;
                strAtmEnd = dttmEndTemp.Format ("%m/%d/%Y %H:%M:%S");
                strASQL.Append (strAtmEnd);
                strASQL.Append ("'");

                iRes = 0; //Success бУДЕМ надеяться на ПОЛОЖИТЕЛЬНый исход
                TRY_RS_OPEN (strASQL)
                strASQL.Empty ();
            
                /*strATmp.Format ("Результат запросА %i", iRes);
                FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_3);*/
                    
                if (iRes == 0) {
                    if (m_pRS->CanScroll ()) {
                        try { m_pRS->MoveFirst (); }
                        catch (CDBException *err) {
						    strLog.format ("HTornadoData::GetValueOfChannel = %s", MB_HSTRING (HString (err->m_strError.GetBuffer ())));
						    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);

						    THROW (err);
                        }
                    }
                    else
                        ;
      
                    while (! m_pRS->IsEOF ())
                    {//ВНИМАНИЕ! Только ОДНа запись!!!
                        m_pRS->GetFieldValue ((short) 0, dbData); //dbo.tep_allcalcs_oper.value

                        switch (dbData.m_dwType) {
                            case DBVT_DOUBLE:
                                dblRes += dbData.m_dblVal;
                                break;
                            case DBVT_SINGLE:
                                dblRes += (double) dbData.m_fltVal;
                                break;
                            default:
                                //dblRes = -666.666;
                                dblRes = 0.0;
                        }

                        m_pRS->GetFieldValue (1, dbData); //COUNT (*)
                        iCountInHours = dbData.m_iVal;

					    /*strLog.format ("HTornadoData::GetValueOfChannel Значение = %f (COUNT = %i), тип = %i", dblRes, iCount, dbData.m_dwType);
                        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);*/

                        m_pRS->MoveNext ();
                    }

                    //if (! IsAccumulationMeasure (idUnitGrp))
                    //    dblRes /= iCount;
                }
                else
                    ;
                
                m_pRS->Close ();

                iCountHours ++;
            }
                        
            //iRes = IsAccumulationMeasure (idUnitGrp);
            iRes = IsAccumulationMeasure (pchSignal);
            switch (iRes) {
                case 0: //AVG
                    dblRes /= iCountHours;
                    break;
                case 1:
                    break;
                case -1:
                default:
                    ; //??? ОШИБКа
            }
        }
        else {
            strLog.format ("Не найден ID cигнала %s", pchSignal);
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_TORNADO6);
        }
    }
    else
    {
        strLog.format ("Наименование cигнала ПУСТой");
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_TORNADO6);
    }
    
    /*strATmp.Format ("HTornado6Data::GetValueOfChannel' на вЫходе %f", dblRes);
    FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_3);*/
    strATmp.Empty ();
        
    return dblRes;
}

#include "stdafx.h"
#include "HTornadoData.h"

EXT_DECLARE_HTHREAD_FILELOG

HTornadoData::HTornadoData(void) : HData () {
}

HTornadoData::HTornadoData (CStringA con) : HData () {
    FILELOG_VARIABLE

	FILELOG_IF_WRITE (HERROR)
    strLog.format ("Конструктор 'HTornadoData'. Резкльтат подключения к БД (%s) = %i", con.GetBuffer (), SetConnectDB (con));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
    FILELOG_ENDIF
}

HTornadoData::~HTornadoData(void) {
}

int HTornadoData::SetConnectDB (CStringA con) {
	FILELOG_VARIABLE

	int iRes = m_pDBData->Connect (con);

	if (! (iRes == 0)) {
        //Не удалось подключиться к БД
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("HTornadoData::SetConnectDB: Не удалось подключиться к БД (%s)", con.GetBuffer ());
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF
    }
    else
        m_pRS = m_pDBData->GetRecordset ();

	return iRes;
}

int HTornadoData::ClearGetValue (void) {
    FILELOG_VARIABLE

    int iRes = 0;
    CStringA strAQuery, strATmp;

    //УДАЛение 'VIEW' tep1
    TRY_DB_DROP ("view", "view_tep1_date")

    return iRes;
}

int HTornadoData::PrepareGetValue (CTime tmBegin, int period)
{
    int iRes = 0;

    CTime tmEnd = tmBegin + (period * 60);
    tmBegin += 1; //ОДНа СЕКУНДа
    FILELOG_VARIABLE
    CStringA strAQuery, strAtmBegin, strAtmEnd;
    strAtmBegin = tmBegin.Format ("%m/%d/%Y %H:%M:%S");
    strAtmEnd = tmEnd.Format ("%m/%d/%Y %H:%M:%S");
    
    //Создание 'VIEW' без ОГРАНИЧений 'reports_tep1'
    strAQuery.Append ("create view view_tep1 as select * from reports_tep1");
    try { m_pDBData->ExecuteSQL (CString (strAQuery).GetBuffer ()); }
    catch (CDBException *err) {
        strLog.format ("Не выполнен запрос: %s\n%s", MB_HSTRING (HString (strAQuery.GetBuffer ())), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_TORNADO);

		err->Delete ();

        iRes = -1;
    }

    strAQuery.Empty ();

    if (iRes == 0) {
        //Создание 'TABLE' с условием по ДАТА/ВРЕМЯ по 'tep1'
        strAQuery.Append ("declare @dt_begin datetime, @dt_end datetime ");
        strAQuery.Append ("select @dt_begin = convert (datetime, '");
        strAQuery.Append (strAtmBegin);
        strAQuery.Append ("') ");
        strAQuery.Append ("select @dt_end = convert (datetime, '");
        strAQuery.Append (strAtmEnd);
        strAQuery.Append ("') ");
        strAQuery.Append ("select * ");
        strAQuery.Append ("into table_tep1_date ");
        strAQuery.Append ("from view_tep1 ");
        strAQuery.Append ("where calcdate between ");
        strAQuery.Append ("@dt_begin and @dt_end");
        try { m_pDBData->ExecuteSQL (CString (strAQuery).GetBuffer ()); }
        catch (CDBException *err) {
            strLog.format ("Не выполнен запрос: %s\n%s", MB_HSTRING (HString (strAQuery.GetBuffer ())), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_TORNADO);

			err->Delete ();

            iRes = -1;
        }

        strAQuery.Empty ();

        if (iRes == 0) {
            //Создание 'VIEW' без ОГРАНИЧений по 'table_tep1_date'
            strAQuery.Append ("create view view_tep1_date as select * from table_tep1_date");
            try { m_pDBData->ExecuteSQL (CString (strAQuery).GetBuffer ()); }
            catch (CDBException *err) {
                strLog.format ("Не выполнен запрос: %s\n%s", MB_HSTRING (HString (strAQuery.GetBuffer ())), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);

                iRes = -1;

				err->Delete ();
            }
        }
        else
            ;

        //УДАЛение 'TABLE' table_tep1_date
        TRY_DB_DROP ("table", "table_tep1_date")

        //УДАЛение 'VIEW' view_tep1
        TRY_DB_DROP ("view", "view_tep1")
    }
    else
        ; //ЕЩЁ не созДан НИ ОДИН обЪект

    return iRes;
}

double HTornadoData::GetValueOfChannel (char *pchSignal, CTime tmBegin, int period) {
    //tmBegin -= 24 *60 * 60; //Для ОТЛАДКи на СУТКи сек. НАЗАД
    double dblRes = 0;
    CDBVariant dbData;
    int iRes = 0, idVTI = -1, idUnitGrp = -1, iCount = -1,
        offsetIndexField = 1; //Для неиндексированного поля = 0, для индексированного = 1
    CStringA dtFields [] = {"dtend", "dt"};

    //"SELECT optname, int_value_002 FROM sys_options WHERE optname = 'grinvich' OR 'oper_interval'"
    //sys_options.optname = 'grinvich' int_value_002 = 7
    //sys_options.optname = 'oper_interval' int_value_002 = 60
    tmBegin -= (6 + offsetIndexField) * 60 * 60;

    CTime tmEnd = tmBegin + (period * 60);
    //tmEnd -= 1; //КАК в 'TORNADo'
    tmBegin += 1; ////ОДНа СЕКУНДа к НАЧальНОМу дата/время КАК у ЛЮДей - НЕУжелИ исправилисЬ к 6-му эн./,бл. ???
    FILELOG_VARIABLE
    CStringA strASQL, strATmp, strAtmBegin, strAtmEnd;
    strAtmBegin = tmBegin.Format ("%m/%d/%Y %H:%M:%S");
    strAtmEnd = tmEnd.Format ("%m/%d/%Y %H:%M:%S");

    /*strATmp.Format ("HTornado6Data::GetValueOfChannel' на входе %s", pchSignal);
    FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_3);*/

    if (! (strlen (pchSignal) == 0))
    {
        //Этот запрос не РАБОТАеТ ??? => НА ДвЕ частИ
        //strASQL.Append ("SELECT ");
        //switch (IsAccumulationMeasure (pchSignal))
        //{
        //    case 0:
        //        strATmp = "AVG";
        //        break;
        //    case 1:
        //        strATmp = "SUM";
        //        break;
        //    case -1:
        //        strATmp = "SUM"; //??? ОШИБКа
        //        break;
        //    default:
        //        ;
        //}
        //
        //strASQL.Append (strATmp);
        //strATmp.Empty ();
        //        
        //strASQL.Append ("(tao.value), COUNT(*) ");
        //strASQL.Append ("FROM dbo.tep_allcalcs_oper tao, dbo.tep_allcalcs ta ");
        //strASQL.Append ("WHERE tao.id=ta.id and ta.name = ");
        //
        //strATmp.Format ("'%s' ", pchSignal);
        //strASQL.Append (strATmp);
        //strATmp.Empty ();
        //
        //strASQL.Append ("AND tao.dt between '");
        //strASQL.Append (strAtmBegin);
        //strASQL.Append ("' AND '");
        //strASQL.Append (strAtmEnd);
        //strASQL.Append ("'");
        //        
        //TRY_RS_OPEN (strASQL)
        //strASQL.Empty ();
        //        
        //if (iRes == 0)
        //{
        //    try { m_pRS->MoveFirst (); }
        //    catch (CDBException *err) {
        //        TRACE (err->m_strError);
		//		  err->Delete ();
        //    }
  
        //    while (! m_pRS->IsEOF ())
        //    {//ВНИМАНИЕ! Только ОДНа запись!!!                
        //        m_pRS->GetFieldValue ((short) 0, dbData); //dbo.tep_allcalcs_oper.value
        //        dblRes = dbData.m_dblVal;
        //        
        //        m_pRS->GetFieldValue (1, dbData); //COUNT (*)
        //        iCount = dbData.m_iVal;
        //        
        //        m_pRS->MoveNext ();
        //    }
        //
        //    m_pRS->Close ();
        //    
        //    //if (! (IsAccumulationMeasure (idUnitGrp) == 1))
        //    //    dblRes /= iCount;
        //}
        //else
        //    ;

        strASQL.Append ("SELECT id FROM dbo.tep_allcalcs WHERE name like ");

        if (strlen (pchSignal) > 6)
            strATmp.Format ("'%s%%' ", pchSignal);
        else
            strATmp.Format ("'%s' ", pchSignal);

        strASQL.Append (strATmp);
        strATmp.Empty ();

        TRY_RS_OPEN (strASQL)
        strASQL.Empty ();

        if (iRes == 0)
        {
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
                m_pRS->GetFieldValue ((short) 0, dbData); //dbo.tep_allcalcs.id
                idVTI = dbData.m_iVal;

                m_pRS->MoveNext ();
            }
        }
        else
            ;

        m_pRS->Close ();

        if (idVTI > -1) {
            //strATmp.Format ("ID cигнала %s = %i", pchSignal, idVTI);
            //FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_3);

            strASQL.Append ("SELECT ");

            iRes = IsAccumulationMeasure (pchSignal);
            switch (iRes)
            {
                case 0:
                    strATmp = "AVG";
                    break;
                case 1:
                    strATmp = "SUM";
                    break;
                case -1:
                    //strATmp = "SUM"; //??? ОШИБКа
                    //break;
                default:
                    strATmp = "SUM"; //??? ОШИБКа
            }

            strASQL.Append (strATmp);
            //strATmp.Empty ();

            //strASQL.Append ("(tao.value * so.int_value_002)/SUM(so.int_value_002), COUNT(*) ");
            strASQL.Append ("(tao.value), COUNT(*) ");

            //strASQL.Append ("FROM dbo.tep_allcalcs_oper tao, dbo.sys_options so ");
            strASQL.Append ("FROM dbo.tep_allcalcs_oper tao ");

            strASQL.Append ("WHERE ");
            //strASQL.Append ("so.optname = 'oper_interval' AND ");
            strASQL.Append ("tao.id = ");

            strATmp.Format ("%i ", idVTI);
            strASQL.Append (strATmp);
            strATmp.Empty ();

            //strASQL.Append ("AND tao.dtend between '");
            strASQL.Append ("AND ");
            strASQL.Append (dtFields [offsetIndexField]);
            strASQL.Append (" between '");
            strASQL.Append (strAtmBegin);
            strASQL.Append ("' AND '");
            strASQL.Append (strAtmEnd);
            strASQL.Append ("'");

            /*strATmp.Format ("Запрос для получения значения: %s", strASQL.GetBuffer ());
            FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_3);*/

            iRes = 0; //Success бУДЕМ надеяться на ПОЛОЖИТЕЛЬНый исход
            TRY_RS_OPEN (strASQL)
            strASQL.Empty ();

            /*strATmp.Format ("Результат запросА %i", iRes);
            FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_3);*/

            if (iRes == 0)
            {
                if (m_pRS->CanScroll ()) {
                    try { m_pRS->MoveFirst (); }
                    catch (CDBException *err) {
						strLog.format ("HTornado6Data::GetValueOfChannel = %s", MB_HSTRING (HString (err->m_strError.GetBuffer ())));
						FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);

						THROW (err);
                    }
                }
                else
                    ;

                while (! m_pRS->IsEOF ())
                {//ВНИМАНИЕ! Только ОДНа запись!!!
                    m_pRS->GetFieldValue ((short) 0, dbData); //dbo.tep_allcalcs_oper.value
                    //dblRes = (double) dbData.m_fltVal;
                    //dblRes = dbData.m_dblVal;

                    switch (dbData.m_dwType) {
                        case DBVT_DOUBLE:
                            dblRes = dbData.m_dblVal;
                            break;
                        case DBVT_SINGLE:
                            dblRes = (double) dbData.m_fltVal;
                            break;
                        default:
                            //dblRes = -666.666;
                            dblRes = 0.0;
                    }

                    m_pRS->GetFieldValue (1, dbData); //COUNT (*)
                    iCount = dbData.m_iVal;

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

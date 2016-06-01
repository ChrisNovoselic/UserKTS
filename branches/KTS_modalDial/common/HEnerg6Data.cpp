#include "stdafx.h"
#include "HEnerg6Data.h"

EXT_DECLARE_HTHREAD_FILELOG

extern int randRange (int , int);

HEnerg6Data::HEnerg6Data (void) : HEnergData () {
}

HEnerg6Data::HEnerg6Data (CStringA con) : HEnergData (), m_lIdVTIList (0) {
    FILELOG_VARIABLE

    while ((m_lIdVTIList < 100000) || (m_lIdVTIList > 1000000)) {
        m_lIdVTIList = randRange (100000, 1000000);
        m_lIdVTIList += GetCurrentThreadId ();
    }

    m_idClassVal = -1;

    InitAccumulationId ();

    FILELOG_IF_WRITE (HCOMMON)
    strLog.format ("����������� HEnerg6Data @idReq = %i, ��������� ���������� � �� = %i", m_lIdVTIList, SetConnectDB (con));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HCOMMON);
    FILELOG_ENDIF
}

HEnerg6Data::~HEnerg6Data(void) {
    FILELOG_VARIABLE

    FILELOG_IF_WRITE (HCOMMON)
    strLog.format ("���������� HEnerg6Data @idReq = %i", m_lIdVTIList);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HCOMMON);
    FILELOG_ENDIF
        
    m_lIdVTIList = 0;
}

int HEnerg6Data::SetConnectDB (CStringA con) {
	int iRes = 0; //Success
    
    CStringA strASQL;
    FILELOG_VARIABLE

    iRes = m_pDBData->Connect (con);
	if (! (iRes == 0)) {
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("HEnerg6Data::SetConnectDB: �� ������� ������������ � �� (%s)", con.GetBuffer ());
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF
    }
    else {
        m_pRS = m_pDBData->GetRecordset ();
        
        strASQL.Empty ();
        strASQL.Format ("EXEC ep_AskVTIdata @cmd='Clear', @idReq=%i", m_lIdVTIList);
        try { m_pDBData->ExecuteSQL (CString (strASQL).GetBuffer ()); }
        catch (CDBException *err) {
            FILELOG_IF_WRITE (HERROR)
            strLog.format ("�� ��������� �������� ���������: %s.\n%s", MB_HSTRING (HString (strASQL.GetBuffer ())), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
            FILELOG_ENDIF

			err->Delete ();
        }
    }

    return iRes;
}

void HEnerg6Data::DisconnectDB (void) {
	HString strASQL;
    FILELOG_VARIABLE
    
    strASQL.format ("EXEC ep_AskVTIdata @cmd='Clear', @idReq=%i", m_lIdVTIList);
    try { m_pDBData->ExecuteSQL (strASQL.GetBuffer ()); }
    catch (CDBException *err) {
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("�� ��������� �������� ���������: %s.\n%s", MB_HSTRING (strASQL), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF

		err->Delete ();
    }

	HData::DisconnectDB ();
}

//�������� ���� �� 'id'
// 'tmBegin' - ����/����� ������� 'period' - �� ����� ������ ����������� (���), 'periodCalc' - ��������� ������� (������ �������������, ���)
double HEnerg6Data::GetValue (CTime tmBegin, int period, int periodCalc, int id, int idungrp) {
    int iRes = 0;
    double dblRes = 0;
    FILELOG_VARIABLE

    CDBVariant dbData;

    HString strASQL, strATmp;

    //strASQL.Empty ();
    //strASQL.Format ("EXEC e6work.dbo.ep_AskVTIdata @cmd='Clear', @idReq=%i", m_lIdVTIList);
    //try { m_pDBData->ExecuteSQL (CString (strASQL).GetBuffer ()); }
    //catch (CDBException *err)
    //{
    //    FILELOG_IF_WRITE (HERROR)
    //    strALog.Format ("�� ��������� �������� ���������: %s.\n%s", strASQL, err->m_strError);
    //    FILELOG_WRITE_WITHDT (strALog.GetBuffer (), HERROR);
    //    FILELOG_ENDIF
	//	  err->Delete ();
    //}

    strASQL.format ("EXEC e6work.dbo.ep_AskVTIdata @cmd='List', @idVTI=%i, @TimeStart='", id);
    //2 ���. ������� �� ������ ������ (�.�. �������� 1-�� ��������)
    strATmp = (tmBegin + 2 * 60).Format ("%d/%m/%Y %H:%M:%S',");
    strASQL.append (strATmp);
    strATmp.free ();
    strASQL.append (" @TimeEnd='");
    //2 ���. ������� �� ������ ������ (�.�. �������� 1-�� ��������)
    strATmp = (tmBegin + (period * 60) + (2 * 60)).Format ("%d/%m/%Y %H:%M:%S',");
    strASQL.append (strATmp);
    strATmp.free ();
    strASQL.append (" @idReq=");
    strATmp.format ("%i", m_lIdVTIList);
    strASQL.append (strATmp);
    strATmp.free ();
    try { m_pDBData->ExecuteSQL (strASQL.GetBuffer ()); }
    catch (CDBException *err) {
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("�� ��������� �������� ���������: %s.\n%s", MB_HSTRING (strASQL), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF

		err->Delete ();
    }

    /*FILELOG_IF_WRITE (HDATA_ENERG6)
    strLog.format ("HEnerg6Data::GetValue () - ��������� �������� ���������\n%s", MB_HSTRING (strASQL));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG6);
    FILELOG_ENDIF*/

    strASQL.free ();

    //������� �1
    /*strASQL.append ("SELECT	dl.idVTI AS 'idVTI', ");
    strASQL.append ("dl.TimeRTC as 'RTC', ");
    strASQL.append ("dl.TimeSQL as 'SQL', ");
    strASQL.append ("CONVERT(varchar(31),dl.TimeSQL,113) as \CharTime\, ");
    strASQL.append ("dl.TimeIdx AS 'TimeIdx', ");
    strASQL.append ("dl.Scale AS 'Scale', ");
    strASQL.append ("dl.DataFull AS 'DataFull', ");
    strASQL.append ("dl.idState AS 'idState', ");
    strASQL.append ("dl.ValueFl AS 'ValueFl', ");
    strASQL.append ("dl.ValueInt AS 'ValueInt' ");*/
    //������� �2
    strASQL.append ("SELECT SUM(dl.ValueFl) AS 'sumValueFl', ");
    strASQL.append ("COUNT(*) AS 'countVal' ");

    strASQL.append ("FROM VTIdataList dl ");
    strASQL.append ("WHERE idREQ = ");
    strATmp.format ("%i", m_lIdVTIList);
    strASQL.append (strATmp);
    strATmp.free ();
    strASQL.append (" and idVTI = ");
    strATmp.format ("%i", id);
    strASQL.append (strATmp);
    strATmp.free ();

    try { m_pRS->Open (CRecordset::dynaset,  strASQL.GetBuffer (), CRecordset::useBookmarks | CRecordset::noDirtyFieldCheck); }
    catch (CDBException *err) {
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("�� �������� ������: %s.\n%s", strASQL, err->m_strError);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF

		err->Delete ();
    }

    int iCountVal = 0;
    
    /*FILELOG_IF_WRITE (HDATA_ENERG6)
    strLog.format ("HEnerg6Data::GetValue () - �������� ������:\n%s", MB_HSTRING (strASQL));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG6);
    FILELOG_ENDIF*/

    if (m_pRS->CanScroll ()) {
        /*try { m_pRS->MoveFirst (); }
        catch (CDBException *err) {
            FILELOG_IF_WRITE (HERROR)
            strALog.Format ("HEnerg6Data:: �� ��������� MoveFirst ()");
            FILELOG_WRITE_WITHDT (strALog.GetBuffer (), HERROR);
            FILELOG_ENDIF

			err->Delete ();
        }*/
    }
    else
        ;

    //������� �1
    /*while (! m_pRS->IsEOF ()) {
        strATmp = "ValueFl";
        try { m_pRS->GetFieldValue (WC_HSTRING (strATmp), dbData); }
        catch (CDBException *err) {
            FILELOG_IF_WRITE (HERROR)
            strLog.format ("HEnerg6Data:: �� ��������� m_pRS->GetFieldValue () ��� idREQ = %i, idVTI = %i", m_lIdVTIList, id);
            FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
            FILELOG_ENDIF

			err->Delete ();
        }
        dblRes += dbData.m_dblVal; 

        m_pRS->MoveNext ();
        iCountVal ++;
    }*/
    //������� �2
    while (! m_pRS->IsEOF ()) {
        strATmp = "sumValueFl";
        m_pRS->GetFieldValue (WC_HSTRING (strATmp), dbData);
        dblRes = dbData.m_dblVal;

        strATmp = "countVal";
        m_pRS->GetFieldValue (strATmp.GetBuffer (), dbData);
        iCountVal = dbData.m_iVal; 

        m_pRS->MoveNext ();
    }

    m_pRS->Close ();

    //�.�. ����������� 30-�� ���.
    //dblRes = dblRes / 2;

	//if ((iCountVal > 0) && ((period / periodCalc) == iCountVal)) {
    if ((iCountVal > 0) && (! ((period / periodCalc) < iCountVal))) {
		//���� ����� �� ������������� - ������� ������� ��������
		if (! (HData::IsAccumulationMeasure (idungrp) == 1))
			if (iCountVal)
				dblRes = dblRes / iCountVal;
			else
				;
		else {//�� ����� �������
			//dblRes = dblRes / (period / 60);
		}
	}
	else
		dblRes = 0.0;

    //���� ����� �������� ������� - ���������� � ������, �.� �������� �� �����
    if (idungrp == 9) {
        //������� � ����/������� ��������������� ���������� � ���./���./���� - ��� ��� ASP ???
        //dblRes = dblRes * (((double) period) / 1000 / 60 / 60);

        dblRes = dblRes / 2;
    }
    else ;

    /*FILELOG_IF_WRITE (HDATA_ENERG6)
    strLog.format ("HEnerg6Data::GetValue () - ����� (���-�� ��������) = %i", iCountVal);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG6);
    FILELOG_ENDIF*/

    return dblRes;
}

double HEnerg6Data::GetValueOfGroup (char *pchNum, CTime tmBegin, int period) {
    int num = atoi (pchNum), indxNum = 0;

    //tmBegin -= 24 *60 * 60; //��� ������� �� ����� ���. �����
    FILELOG_VARIABLE

    double dblRes = 0;
    int iRes = 0;

    if (num > 0) {
        GetInfoTotalOfGroup (num);

        dblRes = GetValue (tmBegin, period,  m_mapGIT [indxNum]->timeintcoeffdef / 60,  m_mapGIT [indxNum]->idVTI,  m_mapGIT [indxNum]->idUnitGrp);
    }
    else {
        FILELOG_IF_WRITE (HDATA_ENERG6)
        strLog.format ("������ � �������: %i ���!", num);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG6);
        FILELOG_ENDIF
    }

    return dblRes;
}

//������ ��� '�����'
//SELECT vl.idvti, vl.idUnitGrp, dut.timeintcoeffdef, ltl.OutDivCoeff as KoeffI, ltl.OutMultCoeff as KoeffU, ltl.OutAddCoeff as KoeffR, ltl.OutUnit
//FROM e6pr.PR_pticlist pl, e6pr.PR_vtilist vl, e6pr.PR_pticoutlist pol, e6pr.PR_dictunitstime dut, e6pr.pr_LinTIMList ltl
//WHERE vl.idvti = pol.idvti and pol.idptic = pl.idptic and dut.idunittime = vl.idunittime and ltl.idptic = pl.idptic
//and vl.idclassval = 15
//and vl.idclassvti = 10
//and vl.idunittime = 20
//and pl.PTICuserNumber = 9

//������ ��� '�������������'
//SELECT vl.idVTI, pl.idUnitGrp, , dut.TimeIntCoeffDef, etl.CounterDivCoeff, etl.KoeffI, etl.KoeffU, etl.KoeffR, etl.OutUnit
//FROM e6pr.PR_PTIClist pl, e6pr.PR_vtilist vl, e6pr.PR_pticoutlist po, e6pr.pr_EngTIIList etl, dbo.DictUnitsTime dut 
//WHERE vl.idvti = po.idvti and po.idptic = pl.idptic and dut.idUnitTime = vl.idUnitTime and etl.idptic = pl.idptic 
//and vl.idClassVal = 25
//and vl.idclassvti = 10
//and vl.idUnitTime = 20
//and pl.PTICuserNumber = 9

int HEnerg6Data::GetInfoTotalOfChannel (int num) {
    FILELOG_VARIABLE

    CDBVariant dbData;
    CStringA strASQL, strATmp;
    int indxNum = 0;
    std::map <int, RecordChannelInfoTotal *>::iterator it;

#if defined _DEBUG
	CString strOutput;
#else
#endif

    if (num > 0) {
    }
    else {
        /*delete m_ptrRecCIT;
        delete m_ptrRecGIT;

        m_ptrRecCIT = DEBUG_NEW RecordChannelInfoTotal [512];
        m_ptrRecGIT = DEBUG_NEW RecordGroupInfoTotal [512];*/

        for (map <int, RecordChannelInfoTotal *>::const_iterator iteratorCIT = m_mapCIT.begin (); iteratorCIT != m_mapCIT.end (); iteratorCIT ++) {
            delete (*iteratorCIT).second;
        }
        m_mapCIT.clear ();
    }

    switch (m_idClassVal) {
        case 15: //�����
            /*strASQL.Append ("SELECT pl.PTICuserNumber, vl.idVTI, vl.idUnitGrp, dut.TimeIntCoeffDef as timeintcoeffdef, ltl.OutDivCoeff as KoeffI, ltl.OutMultCoeff as KoeffU, ltl.OutAddCoeff as KoeffR, ltl.OutUnit ");
            strASQL.Append ("FROM e6pr.PR_pticlist pl, e6pr.PR_vtilist vl, e6pr.PR_pticoutlist pol, e6pr.PR_dictunitstime dut, e6pr.pr_LinTIMList ltl ");
            strASQL.Append ("WHERE vl.idvti = pol.idvti and pol.idptic = pl.idptic and dut.idunittime = vl.idunittime and ltl.idptic = pl.idptic ");*/
            strASQL.Append ("SELECT pl.PTICuserNumber, sel_sec_15.idVTI as idVTI_SEC_15, vl.idVTI as idVTI_MIN_30, vl.idUnitGrp, dut.TimeIntCoeffDef as timeintcoeffdef, ltl.OutDivCoeff as KoeffI, ltl.OutMultCoeff as KoeffU, ltl.OutAddCoeff as KoeffR, ltl.OutUnit \
                            , usdl.USDUserNumber \
                            FROM \
                            (SELECT pl.PTICuserNumber, vl.idVTI \
                            , vl.idclassvti \
                            FROM e6pr.PR_pticlist pl, e6pr.PR_vtilist vl, e6pr.PR_pticoutlist pol, e6pr.PR_dictunitstime dut, e6pr.pr_LinTIMList ltl  \
                            WHERE vl.idvti = pol.idvti and pol.idptic = pl.idptic and dut.idunittime = vl.idunittime and ltl.idptic = pl.idptic");
            break;
        case 25: //�������������
            /*strASQL.Append ("SELECT pl.PTICuserNumber, vl.idVTI, pl.idUnitGrp, dut.TimeIntCoeffDef as timeintcoeffdef, etl.KoeffI, etl.KoeffU, etl.KoeffR, etl.OutUnit, etl.CounterDivCoeff ");
            strASQL.Append ("FROM e6pr.PR_PTIClist pl, e6pr.PR_vtilist vl, e6pr.PR_pticoutlist pol, e6pr.pr_DictUnitsTime dut, e6pr.pr_EngTIIList etl ");
            strASQL.Append ("WHERE vl.idvti = pol.idvti and pol.idptic = pl.idptic and dut.idUnitTime = vl.idUnitTime and etl.idptic = pl.idptic");*/
            strASQL.Append ("SELECT pl.PTICuserNumber, sel_sec_15.idVTI as idVTI_SEC_15, vl.idVTI as idVTI_MIN_30, pl.idUnitGrp, dut.TimeIntCoeffDef as timeintcoeffdef, etl.KoeffI, etl.KoeffU, etl.KoeffR, etl.OutUnit, etl.CounterDivCoeff \
                            , usdl.USDUserNumber \
                            FROM \
                            (SELECT pl.PTICuserNumber, vl.idVTI \
                            , vl.idclassvti \
                            FROM e6pr.PR_PTIClist pl, e6pr.PR_vtilist vl, e6pr.PR_pticoutlist pol, e6pr.pr_DictUnitsTime dut, e6pr.pr_EngTIIList etl \
                            WHERE vl.idvti = pol.idvti and pol.idptic = pl.idptic and dut.idUnitTime = vl.idUnitTime and etl.idptic = pl.idptic");
            break;
        default:
            ;
    }

    strASQL.Append (" and vl.idclassvti = 10"); //������

    //15 - �����, 25 - ������������� = 30 ���.
    //10 - �����, ???43??? - ������������� = 15 ���.
    strASQL.Append (" and vl.idclassval = ");

    switch (m_idClassVal) {
        case 15: //�����
            strATmp.Format ("%i", 10);
            break;
        case 25: //�������������
            strATmp.Format ("%i", 43);
            break;
        default: ;
    }
    strASQL.Append (strATmp);
    strATmp.Empty ();

    //10 = 15'', 14 = 3', 20 = 30'
    strASQL.Append (" and vl.idunittime = ");
    switch (m_idClassVal) {
        case 15: //�����
            strATmp.Format ("%i", 10);
            break;
        case 25: //�������������
            strATmp.Format ("%i", 0);
            break;
        default: ;
    }
    strASQL.Append (strATmp);
    strATmp.Empty ();

    strASQL.Append (" and pl.PTICuserNumber ");

    if (num > 0) {
        strATmp.Format ("= %i", num);
        strASQL.Append (strATmp);
    }
    else {
        strATmp.Format ("BETWEEN %i AND %i", 1, 512);
        strASQL.Append (strATmp);
    }
    strATmp.Empty ();

    switch (m_idClassVal) {
        case 15: //�����
            strASQL.Append (") sel_sec_15 \
                            , e6pr.PR_pticlist pl, e6pr.PR_vtilist vl, e6pr.PR_pticoutlist pol, e6pr.PR_dictunitstime dut, e6pr.pr_LinTIMList ltl \
                            , e6pr.pr_TIchnlList ticl, e6pr.pr_USDList usdl \
                            WHERE vl.idvti = pol.idvti and pol.idptic = pl.idptic and dut.idunittime = vl.idunittime and ltl.idptic = pl.idptic \
                            and vl.idclassvti = sel_sec_15.idclassvti");
            break;
        case 25: //�������������
            strASQL.Append (") sel_sec_15 \
                            , e6pr.PR_PTIClist pl, e6pr.PR_vtilist vl, e6pr.PR_pticoutlist pol, e6pr.pr_DictUnitsTime dut, e6pr.pr_EngTIIList etl \
                            , e6pr.pr_TIchnlList ticl, e6pr.pr_USDList usdl \
                            WHERE vl.idvti = pol.idvti and pol.idptic = pl.idptic and dut.idUnitTime = vl.idUnitTime and etl.idptic = pl.idptic \
                            and vl.idclassvti = sel_sec_15.idclassvti");
            break;
        default: ;
    }

    strASQL.Append (" and pl.idTIchnl = ticl.idTIchnl \
                    and usdl.idUSD = ticl.idUSD");

    strASQL.Append (" and vl.idclassval = ");

    strATmp.Format ("%i", m_idClassVal);
    strASQL.Append (strATmp);
    strATmp.Empty ();

    strASQL.Append (" and vl.idunittime = 20");

    strASQL.Append (" and pl.PTICuserNumber = sel_sec_15.PTICuserNumber");

    try { m_pRS->Open (CRecordset::dynaset,  CString (strASQL).GetBuffer (), CRecordset::useBookmarks | CRecordset::noDirtyFieldCheck); }
    catch (CDBException *err) {
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("�� �������� ������: %s.\n%s", MB_HSTRING (HString (strASQL.GetBuffer ())), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF

		err->Delete ();
    }

    /*if (m_pRS->CanScroll ()) {
        try { m_pRS->MoveFirst (); }
        catch (CDBException *err) {
            TRACE (err->m_strError);
        }
    }
    else
        ;*/

    RecordChannelInfoTotal *pRecCIT = 0x0;

    while (! m_pRS->IsEOF ()) {
        if (num > 0)
            pRecCIT = m_mapCIT [indxNum];
        else {
            strATmp = "PTICuserNumber";
            m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
            indxNum = dbData.m_iVal;

			pRecCIT = DEBUG_NEW RecordChannelInfoTotal;

            //������ ��-��� ������� �������� ���������� �������
            it = m_mapCIT.find (indxNum);
            if (! (it == m_mapCIT.end ()))
                delete (*it).second;
            else
                ;
            //����� ��������
            m_mapCIT [indxNum] = pRecCIT;
        }

        strATmp = "idVTI_SEC_15";
        m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
        //idVTI = dbData.m_iVal;
        pRecCIT->idVTI_SEC_15 = dbData.m_iVal;

        strATmp = "idVTI_MIN_30";
        m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
        //idVTI = dbData.m_iVal;
        pRecCIT->idVTI_MIN_30 = dbData.m_iVal;

        strATmp = "idUnitGrp";
        m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
        //idUnitGrp = dbData.m_iVal;
        pRecCIT->idUnitGrp = dbData.m_iVal;

		strATmp = "timeintcoeffdef";
        m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
        //iPeriodSeconds = dbData.m_iVal;
        pRecCIT->timeintcoeffdef = dbData.m_iVal;

        strATmp = "KoeffI";
        m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
        pRecCIT->iKI_6 = dbData.m_lVal;

        strATmp = "KoeffU";
        m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
        pRecCIT->iKU_6 = dbData.m_lVal;

        strATmp = "KoeffR";
        m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
		switch (m_idClassVal) {
			case 15: //�����
				pRecCIT->rci.sKR = dbData.m_dblVal;
				break;
			case 25: //�������������
				pRecCIT->rci.sKR = dbData.m_iVal;
				break;
			default:
				;
		}

        strATmp = "OutUnit";
        m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
        pRecCIT->Razm_6 = dbData.m_iVal;

        strATmp = "USDUserNumber";
        m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
        pRecCIT->iOwnerNumberUSD = dbData.m_iVal;

        switch (m_idClassVal) {
        case 15: //�����
            break;
        case 25: //�������������
            strATmp = "CounterDivCoeff";
            try { m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData); }
            catch (CDBException *err) {
                FILELOG_IF_WRITE (HERROR)
                strLog.format ("������ ������ �������� �� ����: %s.\n%s", MB_HSTRING (HString (strATmp.GetBuffer ())), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
                FILELOG_ENDIF

		        err->Delete ();
            }
            //iCounterDivCoef = dbData.m_iVal;
            pRecCIT->counterdivcoeff = dbData.m_iVal;
            break;
        default:
            ;
        }

		//TRACE ("�=%i, KI=%i KU=%i, KR=%f\n", indxNum, pRecCIT->iKI_6, pRecCIT->iKU_6, pRecCIT->rci.sKR);

        m_pRS->MoveNext ();
    }

    m_pRS->Close ();

    return 0;
}

int HEnerg6Data::GetInfoTotalOfGroup (int num) {
    FILELOG_VARIABLE

    int iRes = 0, indxNum = 0;
    CDBVariant dbData;
    CStringA strASQL, strATmp;

    strASQL.Append ("SELECT gl.GTICuserNumber, vl.idVTI, vl.idUnitGrp, dut.TimeIntCoeffDef ");
    strASQL.Append ("FROM e6pr.PR_vtilist vl, e6pr.PR_GTIClist gl, e6pr.PR_GTICSourcelist gsl, e6pr.PR_dictunitstime dut ");
    //strASQL.Append ("WHERE gl.idgtic = vl.idchnlowner and vl.idVTI = gsl.idVTIOut");
    strASQL.Append ("WHERE gl.idgtic = vl.idchnlowner and vl.idVTI = gsl.idVTIOut and dut.idunittime = vl.idunittime");
    strASQL.Append (" and vl.idclassvti = 15"); //������
    strASQL.Append (" and gl.GTICuserNumber = ");
    strATmp.Format ("%i", num);
    strASQL.Append (strATmp);
    strATmp.Empty ();
    strASQL.Append (" and vl.idclassval = 25");
	//strASQL.Append (" and dut.idunittime = 20");
    strASQL.Append (" and vl.idunittime = 20");
        
    try { m_pRS->Open (CRecordset::dynaset,  CString (strASQL).GetBuffer (), CRecordset::useBookmarks | CRecordset::noDirtyFieldCheck); }
    catch (CDBException *err) {
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("�� �������� ������: %s.\n%s", MB_HSTRING (HString (strASQL.GetBuffer ())), MB_HSTRING (HString (err->m_strError.GetBuffer ())));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF

        iRes = -1;

        err->Delete ();
    }
        
    if (iRes == 0) {
        if (m_pRS->CanScroll ()) {
            try { m_pRS->MoveFirst (); }
            catch (CDBException *err) {
                //TRACE (err->m_strError);

                iRes = -1;

			    err->Delete ();
            }
        }
        else
            ;
            
        if (iRes == 0) {
            while (! m_pRS->IsEOF ()) {
                strATmp = "idVTI";
                m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
                m_mapGIT [indxNum]->idVTI = dbData.m_iVal;
            
                strATmp = "idUnitGrp";
                m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
                m_mapGIT [indxNum]->idUnitGrp = dbData.m_iVal;

		        strATmp = "TimeIntCoeffDef";
                m_pRS->GetFieldValue (CString (strATmp).GetBuffer (), dbData);
                m_mapGIT [indxNum]->timeintcoeffdef = dbData.m_iVal;
        
                m_pRS->MoveNext ();
            }
        }
        else
            ;
    }
    else
        ;
        
    m_pRS->Close ();

    return iRes; //Success
}

double HEnerg6Data::GetValueOfChannel (char *pchNum, CTime tmBegin, int period) {
    int num = atoi (pchNum), indxNum = 0;

    //tmBegin -= 24 *60 * 60; //��� ������� �� ����� ���. �����
    double dblRes = 0;
    
    FILELOG_VARIABLE
    
    if (num > 0) {
        GetInfoTotalOfChannel (num);
        
        dblRes = GetValue (tmBegin, period,  m_mapCIT [indxNum]->timeintcoeffdef / 60,  m_mapCIT [indxNum]->idVTI_MIN_30, m_mapCIT [indxNum]->idUnitGrp);
    }
    else {
        FILELOG_IF_WRITE (HDATA_ENERG6)
        strLog.format ("������ � �������: %i ���!", num);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG6);
        FILELOG_ENDIF
    }
        
    return dblRes;
}

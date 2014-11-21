#include "StdAfx.h"
#include "HCheckStateTemplate.h"

//#include <vector>
//using namespace std;

EXT_DECLARE_HTHREAD_FILELOG

HCheckStateTemplate::HCheckStateTemplate (int id, HTREEITEM hItem, HString strWildcard, CTimeSpan tmSpanModify, CTimeSpan tmSpanTask, CTimeSpan tmSpanWork, int idInterval, int iMinExt, int iMaxExt, int iCountPassErrorCreate, BOOL bFullInfo) : HCheckStateKTS (id, hItem, strWildcard, tmSpanModify, tmSpanTask, tmSpanWork, idInterval, iMinExt, iMaxExt, bFullInfo) {
    //HObjectTemplate::HObjectTemplate (id, hItem, strWildcard, tmSpanModify, tmSpanTask, tmSpanWork,tmSpanInterval, iMinExt, iMaxExt,  bFullInfo);
    //CStringA strTemp;
    //������ �� ����������� �������� ��� ������ ������������ ???
	//HCheckStateTemplate ();
	
	FILELOG_VARIABLE
	FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
	strLog.format ("����������� 'HCheckStateTemplate' - %s", MB_HSTRING (strWildcard));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), CHECK_STATE_TEMPLATE);
    FILELOG_ENDIF

    if (SetInfo (bFullInfo))
		SetCheck ();
    else
        ;
}

HCheckStateTemplate::~HCheckStateTemplate (void) {
    FILELOG_VARIABLE

	HString strFullPathTemplate = GetFullPathTemplate ();

    FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
    strLog.format (_T ("'WaitFor...m_hEvtOperation' ��� '~HCheckStateTemplate' %S"), WC_HSTRING (strFullPathTemplate));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), CHECK_STATE_TEMPLATE);
    FILELOG_ENDIF
}

BOOL HCheckStateTemplate::SetInfo (BOOL bFullInfo) {
    FILELOG_VARIABLE

#if defined _DEBUG
    CMemoryState memStateBegin, memStateEnd, memStateDiff;
    memStateBegin.Checkpoint ();
#endif

    CFileFind ff;
    HFileInfo *pHFITmp = NULL, *pHFIPrev = NULL;
	BOOL bFindFile = false, bRes = true;
	CTimeSpan tmSpanIntervalDefinition = 0;
	HInfoTemplate it;
	
    HString strTemp; HString strATemp; // ???
    SYSTEMTIME stm;
	
	bFindFile = ff.FindFile (m_strWildcard.GetBuffer ());
	
	FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
	strLog.format ("���� � 'SetInfo' ��� %s", MB_HSTRING (m_strWildcard));
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CHECK_STATE_TEMPLATE);
    FILELOG_ENDIF
    strLog.free ();
    
#if defined _DEBUG
    memStateEnd.Checkpoint ();
    if (memStateDiff.Difference (memStateBegin, memStateEnd) && memStateDiff.m_lTotalCount > 4096) {
        TRACE (_T ("HCheckStateTemplate::SetInfo () - ff.FindFile () - MemoryLeaked!\n"));
        memStateDiff.DumpStatistics ();
    }
    else
        ;
#endif
	
    while (bFindFile) {
        bFindFile = ff.FindNextFile ();

        if (ff.IsDirectory ())
             continue;
        else
            ;

		m_iCountFileFind ++;
		           
		HFILEINFO (pHFITmp, ff.GetFilePath ().GetBuffer ()); //DEBUG_NEW HFileInfo (ff.GetFilePath ().GetBuffer ());
		if (pHFITmp == NULL) {
		    //�� ���������� ����� �� ������ �������� ����������
		    bRes = false;
		    break;
		}
		else
		    ;
	    
	    /*
	    bRes = ff.GetCreationTime (&fileTime);
	    if (bRes) it.tmCreation = CTime (fileTime);
	    else ; //�� ������ ����� ����/����� �������� ���������� ����� ???
		
	    bRes = ff.GetLastAccessTime (&fileTime);
	    if (bRes) it.tmLastAccess = CTime (fileTime);
	    else ; //�� ������ ����� ����/����� �������� ������� ���������� ����� ???
        */
        
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
        
		it.tmLastWrite = pHFITmp->tmLastWritten ();

        //if (it.tmLastWrite > CTime::GetCurrentTime ())
        if ((it.tmLastWrite - CTime::GetCurrentTime ()).GetTotalHours () > 1) {
            //������ �� ����/����� �����������, ������������ �������
			m_arInfoTemplate.RemoveAll ();
			m_arInfoSample.RemoveAll ();
			
			UpdateVariables ();
			
			//MessageBox
			
			return false;
		}
        else
            ;
		
//#if defined _DEBUG
//    memStateEnd.Checkpoint ();
//    if (memStateDiff.Difference (memStateBegin, memStateEnd) && memStateDiff.m_lTotalCount > 4096)
//    {
//        TRACE (_T ("HCheckStateTemplate::SetInfo () - ff.FindFile () - MemoryLeaked!\n"));
//        memStateDiff.DumpStatistics ();
//    }
//    else
//        ;
//#endif
		
        int iExtAsNumber = pHFITmp->FileExtAsNumber ();
		if (! bFullInfo) {
            if (iExtAsNumber > -1) {
		        //����. ����� ����������
		        if (m_iMaxNumberExt < iExtAsNumber)
		            m_iMaxNumberExt = iExtAsNumber;
                else
                    ;
    		
	            //���. ����� ���������� (�,�. '000')
		        if (m_iMinNumberExt > iExtAsNumber)
		            m_iMinNumberExt = iExtAsNumber;
                else
                    ;
            }
            else
                ;
		}
		else
            ; //������ ����������, � �.�. � �������� ������������

		//��������� ����/�����, ���������� �������� (������ ��������) ��������������� �����
	    if (m_pHFILast->tmLastWritten () < it.tmLastWrite) {
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
	    		
	    //��������� ����/�����, ���������� ������� (������ �������) ��������������� �����
	    if (m_pHFIFirst->tmLastWritten () > it.tmLastWrite) {
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
	    
	    it.tmLastWrite -= it.tmLastWrite.GetSecond ();
		//strTemp.init (it.tmLastWrite.Format (_T ("%d.%m.%y %H:%M")).GetBuffer ());
	   
	    //strTemp = ff.GetFileName ();
	    //strTemp = ff.GetFileName ().Right (ff.GetFileName ().GetLength () - ff.GetFileName ().Find ('.') - 1);
		
	    //it.iExt = ff.GetFileName ();
	    //it.iExt = _wtoi (((ff.GetFileName ().Right (ff.GetFileName ().GetLength () - ff.GetFileName ().Find ('.') - 1))).GetBuffer ());
	    it.iExt = iExtAsNumber;

        /*strLog.format ("����� ����������: 'Template' = %i", it.iExt);
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);*/
	    
	    m_arInfoTemplate.Add (it);
	    
//#if defined _DEBUG
//    memStateEnd.Checkpoint ();
//    if (memStateDiff.Difference (memStateBegin, memStateEnd) && memStateDiff.m_lTotalCount > 4096)
//    {
//        TRACE (_T ("HCheckStateTemplate::SetInfo () - ff.FindFile () - MemoryLeaked!\n"));
//        memStateDiff.DumpStatistics ();
//    }
//    else
//        ;
//#endif
		
		if (! bFullInfo) {
	        //���������� �������� ������������ ������
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
		        //�������� ����/����� ������������ 2-� ������ � ������������, ������������� �� 1 (����.: 'TEMPLATE.125' � 'TEMPLATE.126')
		        CTimeSpan tmSpanDiff = pHFITmp->tmLastWritten () - pHFIPrev->tmLastWritten ();
		        
		        if (tmSpanDiff > 0) {
		            //������� �� ���������
		            if (tmSpanDiff.GetSeconds () % 60) {
		                if (tmSpanDiff.GetSeconds () > 30)
		                    tmSpanDiff += (60 - tmSpanDiff.GetSeconds ());
		                else
		                    tmSpanDiff -= tmSpanDiff.GetSeconds ();
		            }
		            else ;

        		    //������ �� ���������, ���� 'tmSpanDiff' > 1 ���� � 6 ���.
        		    if (tmSpanDiff > ((60 * 60) + (6 * 60))) {
		                //if (tmSpanDiff.GetMinutes () % 60) //if (tmSpanDiff.GetMinutes () % ??? 30 ???)
		                //{
		                //    if (tmSpanDiff.GetMinutes () > 30)
		                //        tmSpanDiff -= (tmSpanDiff.GetMinutes () % 60) * 60; //tmSpanDiff -= (tmSpanDiff.GetMinutes () % ??? 30 ???) * 60;
		                //    else
		                //        tmSpanDiff += (30 - (tmSpanDiff.GetMinutes () % 60)) * 60; //tmSpanDiff += (30 - (tmSpanDiff.GetMinutes () % ??? 30 ???)) * 60;
		                //}
						//else
						//	;

		                if (tmSpanDiff.GetMinutes () % 60) {
		                    if (tmSpanDiff.GetMinutes () > 30)
		                        tmSpanDiff += ((60 - tmSpanDiff.GetMinutes ()) * 60);
		                    else
		                        tmSpanDiff -= (tmSpanDiff.GetMinutes () * 60);
		                }
		                else ;
                    }
                    else  ;

		            //m_iMinuteCurrentlyInterval = timeDiff.GetMinutes ();
		            if (tmSpanIntervalDefinition > 0) {
		                if (tmSpanIntervalDefinition == tmSpanDiff)
		                    tmSpanIntervalDefinition = tmSpanDiff;
						else {//�� ��������� �������� ???
							//=> �.�. �����???
						}
		            }
		            else
		                tmSpanIntervalDefinition = tmSpanDiff;
                }
                else ;

                pHFIPrev->CopyOf (pHFITmp);

//#if defined _DEBUG
//    memStateEnd.Checkpoint ();
//    if (memStateDiff.Difference (memStateBegin, memStateEnd) && memStateDiff.m_lTotalCount > 4096)
//    {
//        TRACE (_T ("HCheckStateTemplate::SetInfo () - ff.FindFile () - MemoryLeaked!\n"));
//        memStateDiff.DumpStatistics ();
//    }
//    else
//        ;
//#endif
	        }
		}
		else
            ; //������ ����������, � �.�. � �������� ������������
    }

    if (pHFITmp)
        delete pHFITmp;
    else ;

    if (pHFIPrev)
        delete pHFIPrev;
    else ;

    ff.Close ();

    FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE_ISUPDATE)
    strLog.format ("������� ���� ������ ������ �������. �������: %i", m_iCountFileFind);
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CHECK_STATE_TEMPLATE_ISUPDATE);
    FILELOG_ENDIF
    strLog.free ();

//#if defined _DEBUG
//    memStateEnd.Checkpoint ();
//    if (memStateDiff.Difference (memStateBegin, memStateEnd) && memStateDiff.m_lTotalCount > 4096)
//    {
//        TRACE (_T ("HCheckStateTemplate::SetInfo () - ff.Close () - MemoryLeaked!\n"));
//        memStateDiff.DumpStatistics ();
//    }
//    else
//        ;
//#endif

    if (m_iMaxNumberExt && m_iCountFileFind && bRes) {
        //���� � ��� �� ����: 1-�� � ������� ???
        if ((m_pHFIFirst->FileExtAsNumber () == m_pHFILast->FileExtAsNumber ()) && (m_pHFIFirst->tmLastWritten () == m_pHFILast->tmLastWritten ())) {
		    //MessageBox (AfxGetApp ()->m_pMainWnd->GetSafeHwnd (), _T (""), _T ("��������!"), MB_ICONINFORMATION);

		    //������� �1 ������ �� ������
		    return false;

		    //������� �2 ����������� �ٸ ���, ����������� �������� ???
		    /*if (! SetInfo (false))
		    {
		        m_arInfoTemplate.RemoveAll ();
		        m_arInfoSample.RemoveAll ();

		        UpdateVariables ();

		        return false;
            }*/
        }
        else {
            if ((! bFullInfo) && (tmSpanIntervalDefinition > 0)) {
                if (SetIdInterval (tmSpanIntervalDefinition) == 0)
                    ; //��������� �������� 'm_iIdInterval' - �����
                else
                    ; //???
            }
            else
                ;
        }

        bRes = false;

        /*
	    if (! bFullInfo)
	        if (m_iMaxNumberExt < m_iCountFileFind - 1)
		        m_iMaxNumberExt = m_iCountFileFind - 1;
        */

        HInfoSample is;
        //is.bIsSelected = false;

        SetTarget (&is, CTime::GetCurrentTime (), true);
        //SetTarget (&is, CTime::GetCurrentTime ());

        //�������
        is.tmLastWrite.GetAsSystemTime (stm);

        //26.10.2014
        SYSTEMTIME stmSeason; memset (&stmSeason, 0x0, sizeof (SYSTEMTIME));
        stmSeason.wYear = 2014; stmSeason.wMonth = 10; stmSeason.wDay = 26; stmSeason.wHour = 2; stmSeason.wMinute = 0; stmSeason.wSecond = 0; stmSeason.wMilliseconds = 0;
        CTime tmSeason (stmSeason);
        BOOL bSeason = FALSE;

        CTimeSpan tmSpanIntervalVariable
            , tmSpanDiff;
        //for (int i = 0; i < m_arInfoTemplate.GetSize (); i ++)
        for (int i = m_iMinNumberExt; i < m_iMaxNumberExt + 1; i ++) {
            m_arInfoSample.Add (is);
            /*
            if (m_arInfoSample.GetAt (m_arInfoSample.GetUpperBound ()).tmLastWrite.GetHour () == 0 && 
                m_arInfoSample.GetAt (m_arInfoSample.GetUpperBound ()).tmLastWrite.GetMinute () == 0 &&
                m_arInfoSample.GetAt (m_arInfoSample.GetUpperBound ()).tmLastWrite.GetSecond () == 0)
                m_arInfoSample.GetAt (m_arInfoSample.GetUpperBound ()).tmLastWrite += 60;
	        */    
            tmSpanIntervalVariable = GetIntervalVariable (m_arInfoSample.GetAt (m_arInfoSample.GetSize () - 1).tmLastWrite, DIRECTION_PAST); //??? ��� ������ �� ����� ���������� ��������

            //26.10.2014
            tmSpanDiff = is.tmLastWrite - tmSeason;
            if ((abs (tmSpanDiff.GetTotalSeconds ()) > 0) &&
                abs (tmSpanDiff.GetTotalSeconds ()) < 86400)
                ; //tmSpanIntervalVariable += 60 * 60;
            else
                ;                

            is.tmLastWrite -= tmSpanIntervalVariable;
            is.iExt --;

            if (is.iExt < m_iMinNumberExt)
                //is.iExt = m_arInfoTemplate.GetSize () - 1;
                is.iExt = m_iMaxNumberExt;
            else
                ;

            /*strLog.format ("����� ����������: 'Template' = %i", m_arInfoSample.GetAt (i).iExt);
            FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);*/
        }

        m_tmLastUpdate = CTime::GetCurrentTime ();

        //SetExtError ();

        bRes = true;
    }
    else {
        //�� ������� �� ������ ����� - �������� ��� ���� ��� �������� (�������������) ��������
		//�������� - �� ���������� 'PATH'
        bRes = false;
    }

    /*strLog.format ("������ 'm_arInfoTemplate': %i", m_arInfoTemplate.GetSize ());
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);*/
	
	FILELOG_IF_WRITE (CHECK_STATE_TEMPLATE)
    //FILELOG_IF_WRITE (HDEBUG)
	strLog.format ("����� �� 'SetInfo' ��� %s � �����: %i", MB_HSTRING (m_strWildcard), bRes);
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), CHECK_STATE_TEMPLATE);
    //FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
    FILELOG_ENDIF

    strTemp.free ();
    
//#if defined _DEBUG
//    memStateEnd.Checkpoint ();
//    if (memStateDiff.Difference (memStateBegin, memStateEnd) && memStateDiff.m_lTotalCount > 4096)
//    {
//        TRACE ("HCheckStateTemplate::SetInfo () - ����� - MemoryLeaked!\n");
//        memStateDiff.DumpStatistics ();
//    }
//    else
//        ;
//#endif
	
	return bRes;
}
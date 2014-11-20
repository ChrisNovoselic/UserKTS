#include "stdafx.h"

#include "HFileInfo.h"
#include "HEnerg5Data.h"

EXT_DECLARE_HTHREAD_FILELOG
EXT_DECLARE_HDATETIMEINTERVAL

HEnerg5Data::HEnerg5Data (CStringA con) : HEnergData () {
    FILELOG_VARIABLE
    FILELOG_IF_WRITE (HDATA_ENERG5)
    strLog.format ("Конструктор 'HEnerg5Data'");
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG5);
    FILELOG_ENDIF

    InitAccumulationId ();

	m_OverLapped.Offset     = 0;
    m_OverLapped.OffsetHigh = 0;
    m_OverLapped.hEvent     = NULL;

	m_hFileOfRead = 0x0, m_hTemplateFile = 0x0;

	m_sa.nLength = sizeof (LPSECURITY_ATTRIBUTES);
	m_sa.lpSecurityDescriptor = NULL;
	m_sa.bInheritHandle = false;
	
	if (SetConnectDB (con) == 0)
	    ; //ПРОВЕРКА рез-та установки дата/время обновления БД ??? в 'Connect'
    else
        return ;
}

HEnerg5Data::~HEnerg5Data (void) {
	CLOSE_HANDLE (m_hFileOfRead)
	CLOSE_HANDLE (m_hTemplateFile)
}

int HEnerg5Data::SetConnectDB (CStringA dir) {
    FILELOG_VARIABLE
    FILELOG_IF_WRITE (HDATA_ENERG5)
    strLog.format ("Вход в 'HEnerg5Data::Connect' с аргументом: %s", MB_HSTRING (HString (dir.GetBuffer ())));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG5);
    FILELOG_ENDIF

	int iRes = 0, iPos = -1;
	STR_CONNECTDB.Empty ();
	
	iPos = dir.Find ("DATABASE=", 0);
	if (iPos > -1)
	{
	    iPos += strlen ("DATABASE=");
	    dir = dir.Mid (iPos, dir.Find (";", iPos) - iPos);
	    
	    if (dir.GetLength ())
	    {
	        iPos = 0;
            while (iPos < dir.GetLength ())
	            STR_CONNECTDB.AppendChar (dir.GetAt (iPos ++));
        	    
	        if (! (STR_CONNECTDB.GetAt (STR_CONNECTDB.GetLength () - 1) == '\\'))
	            STR_CONNECTDB.AppendChar ('\\');
            else
                ;

	        if (! (SetDateDB () == 0))
	            iRes = -1; //Не найден ОДИН из файлов БД (adres.eng)
            else
                ;
        }
        else
            iRes = -1; //параметр для СОЕДИНЕЕНия имеет длину = 0
    }
    else
        iRes = -1; //Не НАЙДЕН параметр для СОЕДИНЕЕНия

    FILELOG_IF_WRITE (HDATA_ENERG5)
    strLog.format ("Выход из 'HEnerg5Data::Connect' с кодом: %i", iRes);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG5);
    FILELOG_ENDIF

    return iRes;
}

int HEnerg5Data::SetDateDB () {
    int iRes = 0; //Success
    FILELOG_VARIABLE
    HString strATmp;
    
    FILELOG_IF_WRITE (HDATA_ENERG5)
    strLog.format ("Вход в 'HEnerg5Data::SetDateDB' с аргументом: %s", "Аргументов нет!");
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG5);
    FILELOG_ENDIF

	strATmp = STR_CONNECTDB;
	strATmp.append ("adres.eng");
	HFileInfo *phfi = NULL;
	HFILEINFO (phfi, strATmp.GetBuffer ());
	strATmp.free ();

    //if (phfi->IsKindOf (phfi->GetRuntimeClass ()))
    if ((! (phfi == NULL)) && (phfi->IsInfo ()))
	    m_tmLastWriteDB = CTime (phfi->stmLastWritten ());
    else  {
        STR_CONNECTDB.Empty ();
        iRes = -1;
    }    
    delete phfi;

    FILELOG_IF_WRITE (HDATA_ENERG5)
    strLog.format ("Выход из 'HEnerg5Data::SetDateDB' с кодом: %i", iRes);
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG5);
    FILELOG_ENDIF
    
    strATmp.free ();
        
    return iRes;
}

double HEnerg5Data::GetValueOfGroup (char *pchNum, CTime tmBegin, int period)
{
    int num = atoi (pchNum);
    
	CStringA strAFileNameOfRead;
	FILELOG_VARIABLE
	
	double result = 0;
	
    int iBeforeDay = (m_tmLastWriteDB + (((tmBegin.GetHour () - m_tmLastWriteDB.GetHour ()) * 60 * 60) + ((tmBegin.GetMinute () - m_tmLastWriteDB.GetMinute ()) * 60) + (tmBegin.GetSecond () - m_tmLastWriteDB.GetSecond ())) - tmBegin).GetDays ();
    int num30Begin = tmBegin.GetHour () * 2 + tmBegin.GetMinute () / period, num30End = (tmBegin.GetHour () * 2 + tmBegin.GetMinute () / period) + ((double) period / 60) * 2,
        num30 = num30Begin, num30Count = 0;

    BOOL responce; //Для получения результата выполнения функций
	
    if (! ((num > 0) && (num < 513)))
    {
        FILELOG_IF_WRITE (HDATA_ENERG5)
        strLog.format ("Группы с номером: %i НЕТ!", num);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG5);
        FILELOG_ENDIF
    }
    else
    {
        //unsigned long szFileOfRead = 256 * 48 * sizeof (float) + 2; //Размер файла (и соответственно буфера) для чтения
        unsigned long szFileOfRead = sizeof (float); //Размер файла (и соответственно буфера) для чтения
        BYTE *valueOfRead = DEBUG_NEW BYTE [szFileOfRead]; //Буфер для чтения файла

        char strABeforeDay [256];
        
        strAFileNameOfRead.Empty ();
        //strAFileNameOfRead.Append (m_strAConnectDB);
    	
        if (iBeforeDay > 0)
        {   
            strAFileNameOfRead.Append (CStringA ("SutPro"));
            _itoa_s (iBeforeDay - 1, strABeforeDay, 256,  10);
            strAFileNameOfRead.Append (CStringA (strABeforeDay));
        }
        else
            strAFileNameOfRead.Append (CStringA ("SutTek"));

        //Открываем файл с данными
        if (! (OpenFileData (strAFileNameOfRead) == 0)) {
        }
        else
        {
	        while (num30Count < num30End - num30Begin)
	        {
	            m_OverLapped.Offset = ((num - 1) * 48 * sizeof (float)) + (num30 * sizeof (float));
	            responce = ReadFile (m_hFileOfRead, valueOfRead, szFileOfRead, &szFileOfRead, &m_OverLapped);
    		
		        if (!responce) {
		            FILELOG_IF_WRITE (HERROR)
			        strLog.format ("Ошибка чтения файла: %s", MB_HSTRING (HString (strAFileNameOfRead.GetBuffer ())));
			        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
			        FILELOG_ENDIF
			        break;
		        }
		        else
		        {
			        //curValue = * (WORD *)(bufferOfReadRetro + (512 * sizeof (WORD) * (num30)) + ((num - 1) * sizeof (WORD)));
		            result += * (float *)valueOfRead;
    			    
		            num30 ++;
                    num30Count ++;

		            if ((num30 > (48 - 1)) && (num30 % 48 == 0) && (num30 < num30End)) {
			            iBeforeDay --;

			            if (iBeforeDay < 0)
				            //Попали в будущее
				            break;
						else
							;
        					
			            num30 = 0;

			            CLOSE_HANDLE (m_hFileOfRead)
						CLOSE_HANDLE (m_hTemplateFile)

			            strAFileNameOfRead.Empty ();

			            if (iBeforeDay > 0)
                        {   
                            strAFileNameOfRead += CStringA ("SutPro");
                            _itoa_s (iBeforeDay - 1, strABeforeDay, 256,  10);
                            strAFileNameOfRead += CStringA (strABeforeDay);
                        }
                        else
                            strAFileNameOfRead += CStringA ("SutTek");
        			        
			            //Открываем файл с данными
			            if (! (OpenFileData (strAFileNameOfRead) == 0)) {
			            }
			            else {
			            }
		            }
                }
	        }
        }
        
        if (valueOfRead)
            delete valueOfRead;
        else
            ;

        CLOSE_HANDLE (m_hFileOfRead)
		CLOSE_HANDLE (m_hTemplateFile)
    }
    	
	return result;
}

double HEnerg5Data::GetValueOfChannel (char *pchNum, CTime tmBegin, int period) {
    FILELOG_VARIABLE

    double result = 0;
    std::map <int, RecordChannelInfoTotal *>::iterator it;
    
    result = (double) GetImpulses (pchNum, tmBegin, period);

    if (! (result < 0)) {
        it = m_mapCIT.find (0);
        if ((GetInfoTotalOfChannel (atoi (pchNum)) == 0) && (! (it == m_mapCIT.end ())))
            result = GetValueOfImpulses (atoi (pchNum), result, &((*it).second->rci), period * 60);
        else
        {
            FILELOG_IF_WRITE (HERROR)
	        strLog.format ("HEnerg5Data::GetValueOfChannel (); канал №%s", MB_HSTRING (HString (pchNum)));
	        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
	        FILELOG_ENDIF
        }
	}
    else
        ;

	return result;
}

int HEnerg5Data::OpenFileData (CStringA &name) {
	int iRes = 0; //Success

	FILELOG_VARIABLE

	CString strFullPath = CString (STR_CONNECTDB.GetBuffer ());
	strFullPath.Append (CString (name));
	
#ifndef _MBCS
    strFullPath.Append (_T (".eng"));
#else
    strFullPath.Append (".eng");
#endif

	TRY {
		m_hFileOfRead = CreateFile (
            strFullPath.GetBuffer (),	// (LPCTSTR) pointer to name of the file
			GENERIC_READ,	// (DWORD) access (read-write) mode
			FILE_SHARE_READ,	// (DWORD) share mode
			&m_sa,	// (LPSECURITY_ATTRIBUTES) pointer to security attributes
			OPEN_EXISTING,	// (DWORD) how to create
			FILE_ATTRIBUTE_NORMAL && FILE_FLAG_OVERLAPPED,	// (DWORD) file attributes
			m_hTemplateFile 	// (HANDLE) handle to file with attributes to copy
			);
	}
	CATCH_ALL (err) {
		iRes = -1;
		THROW (err);
	}
	END_CATCH_ALL

	if (INVALID_HANDLE_VALUE == m_hFileOfRead) {
		iRes = -1;

        FILELOG_IF_WRITE (HERROR)
	    strLog.format ("Ошибка открытия файла: %s", MB_HSTRING (HString (name.GetBuffer ())));
	    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
	    FILELOG_ENDIF
    }
    else  {
	}

	return iRes;
}

int HEnerg5Data::GetInfoTotalOfChannel (int num) {
    FILELOG_VARIABLE

    int iRes = 0, //Success
        responce = FALSE,
        indxCIT = -1;
    std::map <int, RecordChannelInfoTotal *>::iterator it;
    DWORD szFileOfRead = -1;
    CStringA strAFileNameOfRead;
    RecordChannelInfo rci;

    if (num > 0)
        indxCIT = 0;
    else
        ;

    strAFileNameOfRead.Append ("Datatek");

	//Открываем файл с данными
	if (! (OpenFileData (strAFileNameOfRead) == 0)) {
	}
	else {
		szFileOfRead = sizeof (RecordChannelInfo);
		m_OverLapped.Offset = (num - 1) * szFileOfRead;
		responce = ReadFile (m_hFileOfRead, &rci, szFileOfRead, &szFileOfRead, &m_OverLapped);

	    if (responce == FALSE) {
	        FILELOG_IF_WRITE (HERROR)
		    strLog.format ("Ошибка чтения файла: %s", MB_HSTRING (HString (strAFileNameOfRead.GetBuffer ())));
			FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
			FILELOG_ENDIF

            iRes = -1;
	    }
	    else {
	        /*strATmp.Format ("Информация по каналу: %i, KI=%f, KU=%f, KR=%f, Размерность-%i", num, rchi.sKI, rchi.sKU, rchi.sKR, rchi.Razm);
			FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), LEVEL_INFO_2);*/

            it = m_mapCIT.find (indxCIT);
            if (!(it == m_mapCIT.end ()))
                (*it).second->rci = rci;
            else
                iRes = -1;
	    }

		CLOSE_HANDLE (m_hFileOfRead)
		CLOSE_HANDLE (m_hTemplateFile)
	}

    return iRes;
}

int HEnerg5Data::GetInfoTotalOfGroup (int ) {
    return 0;
}

BYTE HEnerg5Data::GetAdresData (INDEX_TIME indxTime) {
    FILELOG_VARIABLE

    BYTE iRes = 0; //Success
    int responce = FALSE,
        indxCIT = -1;
    WORD adres;
    DWORD szFileOfRead = -1;
    CStringA strAFileNameOfRead;

    switch (indxTime) {
        case MINUTE_05:
            strAFileNameOfRead.Append ("adres");

	        //Открываем файл с данными
            responce = OpenFileData (strAFileNameOfRead);
	        if (! (responce == 0)) {
                iRes = -1;
	        }
	        else {
		        szFileOfRead = sizeof (WORD);
		        m_OverLapped.Offset = 0;
		        responce = ReadFile (m_hFileOfRead, &adres, szFileOfRead, &szFileOfRead, &m_OverLapped);

	            if (responce == FALSE) {
	                FILELOG_IF_WRITE (HERROR)
		            strLog.format ("Ошибка чтения файла: %s", MB_HSTRING (HString (strAFileNameOfRead.GetBuffer ())));
			        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
			        FILELOG_ENDIF

                    iRes = -1;
	            }
	            else {
	            }

		        CLOSE_HANDLE (m_hFileOfRead)
		        CLOSE_HANDLE (m_hTemplateFile)
	        }

            if (iRes == 0) {
                adres --;
                if (adres == 0)
                    adres = 6;
                else ;

                iRes = adres;
            }
            else
                iRes = -1;
            break;
        case MINUTE_30:
            break;
        default: ;
    }

    return iRes;
}

WORD HEnerg5Data::GetImpulses (char *pch_num, INDEX_TIME indxTime) {
    FILELOG_VARIABLE

    int iRes = 0, //Success
        i, responce = FALSE;
    WORD wValueOfRead = -1, impulses = 0;
    WORD *adreses = 0x0;
    SYSTEMTIME stm;
    DWORD szFileOfRead = -1;
    CStringA strAFileNameOfRead;

    int num = atoi (pch_num);

    if (num > 0) {
        switch (indxTime) {
            case MINUTE_05:
                strAFileNameOfRead.Append ("On30T");
                adreses = DEBUG_NEW WORD [2]; memset (adreses, 0x0, 2 * sizeof (WORD));
                adreses [0] = GetAdresData (indxTime);
                adreses [1] = 0;
                break;
            case MINUTE_30:
                strAFileNameOfRead.Append ("KanTek");
                GetSystemTime (&stm);
                stm.wHour -= PTR_DATETIMEINTERVAL->tm64tBias () / 60 / 60;
                adreses = DEBUG_NEW WORD [2]; memset (adreses, 0x0, 2 * sizeof (WORD));
                ((stm.wHour == 0x0) && (stm.wMinute == 0x0)) ? adreses [0] = 48 - 1 : adreses [0] = (WORD) (stm.wHour * 2 + stm.wMinute / 30);
                adreses [1] = 0;
                break;
            case HOUR_01:
                strAFileNameOfRead.Append ("KanTek");
                GetSystemTime (&stm);
                stm.wHour -= PTR_DATETIMEINTERVAL->tm64tBias () / 60 / 60;
                adreses = DEBUG_NEW WORD [3]; memset (adreses, 0x0, 3 * sizeof (WORD));
                (stm.wHour == 0x0) ? adreses [0] = 48 - 2 : adreses [0] = (WORD) ((stm.wHour - 1) * 2 + 1);
                adreses [1] = adreses [0] + 1;
                adreses [2] = 0;
                break;
            default: ;
        }

        //Открываем файл с данными
        iRes = OpenFileData (strAFileNameOfRead);
	    if (! (iRes == 0))
            ;
	    else {
		    szFileOfRead = sizeof (WORD);

            i = 0;
            //while (! (adreses [i] < 0)) {
            while (adreses [i] > 0) {
		        m_OverLapped.Offset = (512 * sizeof (WORD) * (adreses [i] - 1)) + ((num - 1) * sizeof (WORD));
		        responce = ReadFile (m_hFileOfRead, &wValueOfRead, szFileOfRead, &szFileOfRead, &m_OverLapped);

	            if (! (responce == TRUE)) {
	                FILELOG_IF_WRITE (HERROR)
		            strLog.format ("Ошибка чтения файла: %s", MB_HSTRING (HString (strAFileNameOfRead.GetBuffer ())));
			        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
			        FILELOG_ENDIF

                    iRes = -1;

                    break;
	            }
	            else {
                    impulses += wValueOfRead;
	            }

                i ++;
            }

		    CLOSE_HANDLE (m_hFileOfRead)
		    CLOSE_HANDLE (m_hTemplateFile)
	    }

        delete adreses;
    }
    else
        iRes = -1; //Не адекватный номер канала

    if (iRes == 0)
        return impulses;
    else
        return -1;
}

DWORD HEnerg5Data::GetImpulses (char *pch_num, CTime tmBegin, int period) {
    DWORD dwRes = 0;

    int num = atoi (pch_num);
    
	CStringA strAFileNameOfRead;
	FILELOG_VARIABLE

	int iBeforeDay = (m_tmLastWriteDB + (((tmBegin.GetHour () - m_tmLastWriteDB.GetHour ()) * 60 * 60) + ((tmBegin.GetMinute () - m_tmLastWriteDB.GetMinute ()) * 60) + (tmBegin.GetSecond () - m_tmLastWriteDB.GetSecond ())) - tmBegin).GetDays ();
	int num30Begin = tmBegin.GetHour () * 2 + tmBegin.GetMinute () / period, num30End = (tmBegin.GetHour () * 2 + tmBegin.GetMinute () / period) + ((double) period / 60) * 2,
	    num30 = num30Begin, num30Count = 0;

	BOOL responce; //Для получения результата выполнения функций
	
	if (! ((num > 0) && (num < 513))) {
	    FILELOG_IF_WRITE (HDATA_ENERG5)
	    strLog.format ("Канала с номером: %i НЕТ!", num);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_ENERG5);
        FILELOG_ENDIF
	}
	else {
	    //unsigned long szFileOfRead = 512 * 48 * sizeof (WORD) + 2; //Размер файла (и соответственно буфера) для чтения
	    unsigned long szFileOfRead = sizeof (WORD); //Размер файла (и соответственно буфера) для чтения
        BYTE *valueOfRead = DEBUG_NEW BYTE [szFileOfRead]; //Буфер для чтения файла

        char strABeforeDay [256];

	    strAFileNameOfRead.Empty ();
	    //strAFileNameOfRead.Append (m_strAConnectDB);

	    if (iBeforeDay > 0) {   
            strAFileNameOfRead. Append ("KanPro");
            _itoa_s (iBeforeDay - 1, strABeforeDay, 256,  10);
            strAFileNameOfRead.Append (CStringA (strABeforeDay));
        }
        else
            strAFileNameOfRead.Append ("KanTek");

	    //Открываем файл с данными
	    if (! (OpenFileData (strAFileNameOfRead) == 0)) {
	    }
	    else {
		    while ((num30Count < (num30End - num30Begin)) && (! (INVALID_HANDLE_VALUE == m_hFileOfRead)) && (! (0x0 == m_hFileOfRead)))
		    {
			    m_OverLapped.Offset = (512 * sizeof (WORD) * (num30)) + ((num - 1) * sizeof (WORD));
			    responce = ReadFile (m_hFileOfRead, valueOfRead, szFileOfRead, &szFileOfRead, &m_OverLapped);

			    if (! responce) {
			        FILELOG_IF_WRITE (HERROR)
				    strLog.format ("Ошибка чтения файла: %s", MB_HSTRING (HString (strAFileNameOfRead.GetBuffer ())));
				    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
				    FILELOG_ENDIF
				    break;
			    }
			    else {
				    //curValue = * (WORD *)(bufferOfReadRetro + (512 * sizeof (WORD) * (num30)) + ((num - 1) * sizeof (WORD)));
				    dwRes += * (WORD *)(valueOfRead);

				    num30 ++;
			        num30Count ++;

					//Проверка перехода через сутки
			        if ((num30 > (48 - 1)) && (num30 % 48 == 0) && (num30 < num30End)) {
				        iBeforeDay --;

				        if (iBeforeDay < 0)
					        //Попали в будущее ???
					        break;
						else
							;

			            num30 = 0;

						CLOSE_HANDLE (m_hFileOfRead)
						CLOSE_HANDLE (m_hTemplateFile)

				        strAFileNameOfRead.Empty ();

				        if (iBeforeDay > 0) {   
					        strAFileNameOfRead += "KanPro";
					        _itoa_s (iBeforeDay - 1, strABeforeDay, 256,  10);
					        strAFileNameOfRead += CStringA (strABeforeDay);
				        }
				        else
					        strAFileNameOfRead += "KanTek";

				        //Открываем НОВый файл с данными
				        if (! (OpenFileData (strAFileNameOfRead) == 0)) {
				        }
				        else
							;
			        }
					else
						; //Нет перехода через сутки
			    }
		    } //Цикл по 30-ти минутным интервалам
	    }

	    CLOSE_HANDLE (m_hFileOfRead)
		CLOSE_HANDLE (m_hTemplateFile)

	    delete valueOfRead;

	    strAFileNameOfRead.Empty ();
    }
    
    return dwRes;
}
#include "stdafx.h"

//Для theApp ('GetPtrThread')
#include "PseudoKTS.h"

#include "HThreadWriteData.h"
#include "HThreadGetData.h"
#include "HCheckStateTemplate.h"

IMPLEMENT_DYNAMIC(HThreadWriteData, HWinThread)

HThreadWriteData::HThreadWriteData (void) {
}

HThreadWriteData::~HThreadWriteData (void) {
}

BEGIN_MESSAGE_MAP (HThreadWriteData, HWinThread)
	ON_THREAD_MESSAGE (WM_USER_THREAD_RESUME, &OnResumeRun)
	ON_THREAD_MESSAGE (ID_OPERATION_CREATE, &OnOperationCreate)
	ON_THREAD_MESSAGE (ID_OPERATION_CHANGE_DATETIME, &OnOperationModifyDT)
END_MESSAGE_MAP ()

int HThreadWriteData::InitInstance (void) {
	int iRes = -1;
	//ПЕРВый вызов метода БАЗового класса
	iRes = HWinThread::InitInstance ();

    FILELOG_VARIABLE

    if (iRes == 1) {
	}
	else ; //Инициализация - ОШИБКА

	return iRes;
}

int HThreadWriteData::ExitInstance (void) {
	//КРАЙний вызов метода БАЗового класса
	return HWinThread::ExitInstance ();
}

afx_msg void HThreadWriteData::OnResumeRun (WPARAM, LPARAM ) {
}

#define READ_FILEDATA hTemplate = CreateFile (WC_HSTRING (strFullPath), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); \
						if ((! (hTemplate == INVALID_HANDLE_VALUE)) && (! (hTemplate == 0x0))) { \
							szContent = GetFileSize (hTemplate, &szContent); \
							content = DEBUG_NEW BYTE [szContent]; \
							if (ReadFile (hTemplate, content, szContent, &iResBytes, 0x0)) ; else ; \
						} else hTemplate = 0x0;

afx_msg void HThreadWriteData::OnOperationCreate (WPARAM wp, LPARAM lp) {
	HCheckStateTemplate *pCST = (HCheckStateTemplate *) wp;
	delete [] (int *) lp;

	HThreadGetData *ptrThreadGetData = 0x0;
	//boolean bGetData = false;

	HString strFullPath (pCST->GetFullPathTemplate ()), strItemName, strIntervalName;
	void *content = 0x0; DWORD iResBytes = -1, szContent = -1;

	SYSTEMTIME stmWrite; pCST->GetDTModifyInfoSample (0).GetAsSystemTime (stmWrite);
	__time64_t tm64tWrite = pCST->GetDTModifyInfoSample (0).GetTime (), *ptm64tQuery [COUNT_QUERY_TIME];
	memset (ptm64tQuery, 0x0, COUNT_QUERY_TIME * sizeof (__time64_t *));

#ifdef _DEBUG
	SYSTEMTIME stmQuery;
#else
#endif

	WORD *pWordImpulses [COUNT_QUERY_TIME];
	memset (pWordImpulses, 0x0, COUNT_QUERY_TIME * sizeof (WORD *));
	
	strItemName = strFullPath.right (strFullPath.length () - (strFullPath.reverseFind ("\\") + 1));
	strItemName = strItemName.left (strItemName.length () - 4);

	HANDLE hTemplate = 0x0;

	/*FILE *fTemplate;
	fTemplate = fopen (MB_HSTRING (strFullPath), "+w");*/

	strIntervalName = PTR_DATETIMEINTERVAL->GetNameInterval (pCST->GetIdInterval ());

	//if (! (iResBytes == 0x0)) {
		ptrThreadGetData = dynamic_cast <HThreadGetData *> ((dynamic_cast <CPseudoKTSApp *> (AfxGetApp ()))->GetPtrThread (CPseudoKTSApp::THREAD_GETDATA));

		if (strIntervalName.compareNoCase ("MINUTE_05") == 0) {
			ptm64tQuery [MINUTE_05] = (__time64_t *) ptrThreadGetData->GetDataOfId (HThreadGetData::TIME64T_QUERY_MIN_05);
			pWordImpulses [MINUTE_05] = (WORD *) ptrThreadGetData->GetDataOfId (HThreadGetData::PTR_IMP_MIN_05);

			if ((! (ptm64tQuery [MINUTE_05] == 0x0)) &&
				(! (pWordImpulses [MINUTE_05] == 0x0))) {
#if defined _DEBUG
				CTime (*ptm64tQuery [MINUTE_05]).GetAsSystemTime (stmQuery);
#else
#endif
				if (tm64tWrite == *ptm64tQuery [MINUTE_05]) {
					READ_FILEDATA
				}
				else ;
			}
			else ;

			if (/*(WaitForSingleObject (APP_HANDLE_EVENT (HWinAppKTS::ID_HANDLE_EVENT::GETDATA_05_MIN), 0) == WAIT_OBJECT_0) && */hTemplate) {
				//Вариант №1
				if ((strItemName.compareNoCase (_T ("DtTm")) == 0)) {
					*(((WORD *) content) + 0) = stmWrite.wYear;
					*(((WORD *) content) + 1) = stmWrite.wMonth;
					*(((WORD *) content) + 2) = stmWrite.wDay;
					*(((WORD *) content) + 3) = stmWrite.wHour;
					*(((WORD *) content) + 4) = stmWrite.wMinute;
					*(((WORD *) content) + 5) = stmWrite.wSecond;
					*(((WORD *) content) + 6) = 0xff; //Контрольная сумма
				}
				else
					if (strItemName.compareNoCase ("Adres") == 0) {
						*(((WORD *) content) + 0) = (WORD) ((int) stmWrite.wMinute % 30 / 5) + 1;
					}
					else
						if (strItemName.compareNoCase ("On30T") == 0) {
							WORD adres = (WORD) ((int) stmWrite.wMinute % 30 / 5) + 1;

                            adres --;
                            if (adres == 0)
                                adres = 6;
                            else ;

							memcpy ((WORD *) content + (adres - 1) * 512, pWordImpulses [MINUTE_05], 512 * sizeof (WORD));
						}
						else
							;

				//Вариант №2
				/*switch (pstID->item_id) {
					case 1:
						break;
					case 2:
						break;
					default:
						;
				}*/
			} else ; //bGetData == false (Данные НЕ ГОТовЫ) - MINUTE_05
		} else {
			if (strIntervalName.compareNoCase ("MINUTE_30") == 0) {
				ptm64tQuery [MINUTE_30] = (__time64_t *) ptrThreadGetData->GetDataOfId (HThreadGetData::TIME64T_QUERY_MIN_30);
				pWordImpulses [MINUTE_30] = (WORD *) ptrThreadGetData->GetDataOfId (HThreadGetData::PTR_IMP_MIN_30);

				if ((! (ptm64tQuery [MINUTE_30] == 0x0)) &&
					(! (pWordImpulses [MINUTE_30] == 0x0)))
					if (tm64tWrite == *ptm64tQuery [MINUTE_30]) {
						READ_FILEDATA;
					}
					else ;
				else ;

				if (hTemplate) {
					if (strItemName.compareNoCase ("KanTek") == 0) {
						WORD adres = -1;
						((stmWrite.wHour == 0x0) && (stmWrite.wMinute == 0x0)) ?
							adres = 48 - 1 :
							adres = (WORD) (stmWrite.wHour * 2 + stmWrite.wMinute / 30);

						memcpy ((WORD *) content + (adres - 1) * 512, pWordImpulses [MINUTE_30], 512 * sizeof (WORD));
					}
					else
						;
				} else ; //bGetData == false (Данные НЕ ГОТовЫ) - MINUTE_30
			}
			else ; //MINUTE_30
		}

		if ((! (content == 0x0)) && (szContent > 0) && (! (hTemplate == 0x0)) && (! (hTemplate == INVALID_HANDLE_VALUE))) {
			CLOSE_HANDLE (hTemplate)

			hTemplate = CreateFile (WC_HSTRING (strFullPath), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

			//DWORD dwPos = SetFilePointer(hAppend, 0, NULL, FILE_END);
			//LockFile (hTemplate

			WriteFile (hTemplate, content, szContent, &iResBytes, 0x0); //return BOOL
			//fwrite (content, szContent, &iResWriteBytes, fTemplate);

			//fclose (fTemplate);
			
			FILETIME FileTimeCreate, FileTimeAccess, FileTimeWrite;
			GetFileTime (hTemplate, &FileTimeCreate, &FileTimeAccess, &FileTimeWrite);

			//CTime dtWrite (stmWrite);
			//dtWrite = CTime (dtWrite.GetTime () - datetimeZoneInfo.Bias * 60 * 60);  //* pow (10.0, 7);
			//dtWrite.GetAsSystemTime (stmWrite);
			SystemTimeToFileTime (&stmWrite, &FileTimeWrite);
			__int64 *pi64ft = (__int64 *) &FileTimeWrite;
			*pi64ft += PTR_DATETIMEINTERVAL->FileTimeBias ();
			SetFileTime (hTemplate, &FileTimeCreate, &FileTimeAccess, &FileTimeWrite);

            if (strItemName.compareNoCase ("On30T") == 0) {
                ptrThreadGetData->PostThreadMessage (WM_USER_WRITEDATA, MINUTE_05, 0);
            }
            else
                if (strItemName.compareNoCase ("KanTek") == 0) {
                    ptrThreadGetData->PostThreadMessage (WM_USER_WRITEDATA, MINUTE_30, 0);

                    if (stmWrite.wMinute == 0) {
                        ptrThreadGetData->PostThreadMessage (WM_USER_WRITEDATA, HOUR_01, 0);

                        if (stmWrite.wHour == 0) {
                            //ptrThreadGetData->PostThreadMessage (WM_USER_WRITEDATA, DAY_01, 0);
                        }
                        else
                            ;
                    }
                    else
                        ;
                }
                else
                    ;
		}
		else ;
	/*}
	else
		;*/

	delete content;

	SetEvent (pCST->m_hEvtOperation);

	if ((! (hTemplate == 0x0)) && (! (hTemplate == INVALID_HANDLE_VALUE)))
        CLOSE_HANDLE (hTemplate)
    else
        ;
			
	//MODIFY_DATETIME_FILE (pCST, strFullPath, 0)
}

afx_msg void HThreadWriteData::OnOperationModifyDT (WPARAM, LPARAM ) {
}
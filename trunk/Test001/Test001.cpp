// Test001.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

#if defined _DEBUG
    extern CMemoryState g_memStateBegin, g_memStateEnd, g_memStateDiff;
#endif

extern BOOL CALLBACK fWndEnumProcToLog (HWND , LPARAM );
extern BOOL CALLBACK fWndEnumProcCloseErr (HWND , LPARAM );

class HTestMemory : public CObject
{
public:
    HTestMemory () : CObject (), m_pwchValue (NULL), m_iValue (0) {}
    HTestMemory (int iValue) : CObject (), m_pwchValue (NULL), m_iValue (iValue) {}
    HTestMemory (TCHAR *pwchValue) : CObject () { InitializeMembers (); m_pwchValue = DEBUG_NEW TCHAR [wcslen (pwchValue) + 1]; std::cout << wcslen (pwchValue) << std::endl; m_pwchValue = wcscpy (m_pwchValue, pwchValue); }
    ~HTestMemory () { if (m_pwchValue) delete m_pwchValue; else ; }

    void InitializeMembers () { m_pwchValue = NULL; m_iValue = 0; }
    
    /*HString getReverse (HString src)
    {
        HString strRes;
        
        strRes = src;
        strRes.reverse ();
        
        return strRes;
    }*/
    
    void getReverse (HString &src)
    {
        src.reverse ();
    }
    
    CString GetStringIntValue () {
        CString strRes;
        strRes.Format (_T ("%i"), m_iValue);
        
        return strRes;
    }
    
    CString GetStringTcharValue () {
        CString strRes;
        int i = 0;
        while (* ((TCHAR *) (m_pwchValue + i)))
        {
            strRes.AppendChar (* ((TCHAR *) (m_pwchValue + i ++)));
        }
        strRes.AppendChar (0x0);
        
        return strRes;
    }
private:
    int m_iValue;
    TCHAR *m_pwchValue;
};

EXT_DECLARE_HTHREAD_FILELOG

EXT_DECLARE_WSA
EXT_DECLARE_HA2KW

int setI (int indx, int &val) {
	int iRes = false;

	if (indx > -1) {
		val = indx;
		iRes = true;
	}
	else
		val = -1;

	return iRes;
}

int _tmain(int argc, _TCHAR *argv [])
{
#if defined _DEBUG
    g_memStateBegin.Checkpoint ();
#endif    

	FILELOG_VARIABLE

    //CString strTemp;
    int iTemp = -666, iRes = -1;

	if ((setI (1, iTemp)) && (iTemp > 0) && (iTemp == 1)) {
		iTemp = 666;
	}
	else
		;

    wchar_t wcstrTemp [MAX_PATH] = _T ("KanPro12.eng"), wcstrSearch [MAX_PATH] = _T ("Pro");
    wchar_t *wcstrRes = 0x0;
    //LPWSTR wcstrRes = 0x0, wcstrTemp = _T ("KanPro12.eng"), wcstrSearch = _T ("Pro");
    wcsupr (wcstrTemp); wcsupr (wcstrSearch);
    wcstrRes = wcsstr (wcstrTemp, wcstrSearch);
    

	HTestMemory *objTestMemory = 0x0;
	HString strHostName, strAppDirectory, strAppName, strTemp;

	WSA_IMPLEMENT
	/*if (WSAStartup (MAKEWORD(2, 2), &g_wsaData) == 0) {
		g_pHostEnt = gethostbyname ("localhost");

		if (WSACleanup () != 0) {
			delete g_pHostEnt;
			g_pHostEnt = 0x0;
		}
		else WSA_PATHAPP (g_strPathApp);
	}
	else ;*/

	if (WSA_IS_INIT) {
		WSA_HOSTNAME (strHostName)
		//strHostName = HString (g_pHostEnt->h_name); strHostName = strHostName.left (strHostName.find (0, '.'));
		WSA_APPDIRECTORY (strAppDirectory)
		WSA_APPNAME (strAppName)

		//Старт потока записи ЛОГ-файла...
		IMPLEMENT_HTHREAD_FILELOG

		if (IS_INIT_HTHREAD_FILELOG) {
			//int iCOUNT_ATTEMPT = 6;
			//iRes = 0;
			//do {
			//    iCOUNT_ATTEMPT --;
			//}
			////while ((iCOUNT_ATTEMPT > 0) && (! (iRes == 1)));
			//while ((iCOUNT_ATTEMPT > 0) && (iRes == 0));

			WNDENUMPROC fWndProc = NULL;
			LPARAM lpWndProc;
			HString strAppName;
			WSA_APPNAME (strAppName)
			//::GetModuleFileName ();


			//::GetWindowThreadProcessId (

			//fWndProc = fWndEnumProcCloseErr;
			//lpWndProc = (LPARAM) GetAppName ().GetBuffer ();
			//EnumThreadWindows (GetCurrentThreadId (), fWndProc, lpWndProc);

			fWndProc = fWndEnumProcCloseErr; //fWndEnumProcToLog;
			lpWndProc = (LPARAM) strAppName.GetBuffer ();
			EnumWindows (fWndProc, lpWndProc);

			iRes = -1;

			objTestMemory = DEBUG_NEW HTestMemory (666);
			/*strTemp = objTestMemory->GetStringIntValue ();
			std::cout << ((TCHAR *) objTestMemory->GetStringIntValue ().GetBuffer ()) << std::endl;*/
		    
			//HTestMemory *objTestMemory = DEBUG_NEW HTestMemory (_T ("666"));
			//strTemp = objTestMemory->GetStringTcharValue ();
			//std::cout << ((TCHAR *) objTestMemory->GetStringTcharValue ().GetBuffer ()) << std::endl;
		        
			strLog = ("uyfug");
			strTemp  = (_T ("\tiksdgerkg"));
			
			strLog.append (strTemp);
			strLog.append (_T ("hghoiujpoip["), HString::WCSTR);
			strLog.append ("dfgiufeh");

			strTemp.free ();

			strLog = "1234567,12345678,123456789";
			int i = 44, iPos = 0;
			//while ((iPos > -1) && (i > -1)) {
			while (i > -1) {
				strTemp = strLog.tokenize (",", iPos);
				i --;
			}
			
			strLog.format ("%i%s%c12/12/1966 34:56:78%i", 666, MB_HSTRING (strTemp), '=', 666);
			strLog.format ("%i%s%c12/12/1966 34:56:78%i", 666, "666", '=', 666);
			strLog.format ("Пройден цикл ПОИСКа файлов ШАБЛОНа. Найдено: %i", iTemp);
			strLog.format ("%i%s%c12/12/1966 34:56:78%i", 666, "666", '=', 666);
			strLog.format ("%i%S%c12/12/1966 34:56:78%i", iTemp, _T ("666"), '=', 666);
			
			iRes = strLog.insert (0, strTemp, HString::MBSTR);
			
			iRes = strLog.insert (iRes, strTemp, HString::WCSTR);
			iRes = strLog.insert (iRes / 2, strTemp, HString::MBSTR);
			
			strTemp = HString (_T ("Пройден цикл ПОИСКа файлов ШАБЛОНа. Найдено"));
			//strTemp = strLog;
			
			char ch = MB_AT (strTemp, 6);
			TCHAR wch = WC_AT (strTemp, 7);
			
			//wch = (unsigned char) ch; //НЕ КОРРЕКТНО
			
			cout << MB_HSTRING (strTemp) << endl;
			
			strTemp.appendChar ('S');
			strTemp.appendChar (wch);
			
			iRes = strTemp.find (0, wch);
			iRes = strTemp.find (0, (TCHAR)'П');
			iRes = strTemp.find (0, 'Z');
			iRes = strTemp.find (0, (TCHAR)'я');
			iRes = strTemp.find (0, '\t');
			iRes = strTemp.find (0, '\\');
			iRes = strTemp.find (0, ' ');
			iRes = strTemp.find (0, '\32');
			iRes = strTemp.find (0, '\x67');
			iRes = strTemp.find (0, '\u1234');
			iRes = strTemp.find (0, (TCHAR)'й');
			
			iRes = strTemp.reverseFind ((TCHAR)'Ш');

			strTemp.append (strLog);
			
			strTemp.cut (6, 6);
			strTemp.replace ("ПОИСК", "НАЙДЕН");
			
			strTemp = strTemp + strLog;
			
			//strTemp = objTestMemory->getReverse (strTemp);
			objTestMemory->getReverse (strTemp);
			
			strLog.free ();
			
			strLog = HString (strTemp);
			strTemp = strLog.mid  (2, 4);

			CTime tmEntry = CTime::GetCurrentTime (), tmCur;
			__time64_t tm64tEntry  = tmEntry.GetTime (), tm64tCur, tm64tPrev;
			ATL::CTime atltmEntry = tmEntry.GetTickCount (), atltmCur;
			SYSTEMTIME stmEntry, stmCur;
			CTime::GetCurrentTime ().GetAsSystemTime (stmEntry);
			clock_t clktEntry =  clock (), clktCur, clktPrev = clktEntry;

			//unsigned short arValueSleep [] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 0};
			unsigned short arValueSleep [] = {10, 50, 100, 200, 400, 666, 500, 800, 1000, 0};
			unsigned short arValueSleepSum = 0;

			i = 0;
			while (arValueSleep [i]) {
				Sleep (arValueSleep [i]);
				arValueSleepSum += arValueSleep [i];

				tmCur = CTime::GetCurrentTime ();
				tm64tCur = tmCur.GetTime ();
				atltmCur = CTime::GetTickCount ();
				CTime::GetCurrentTime ().GetAsSystemTime (stmCur);
				clktCur = clock ();

				strTemp.format ("Милисекунды (%i); ms = %i; Тики (%i)", arValueSleepSum, clktCur - clktPrev, tm64tCur);
				//strTemp.format ("Милисекунды (%i) = Тики (%i); ms = %l", arValueSleepSum, clktCur - clktEntry, tm64tCur);

				clktPrev = clktCur;

				cout << MB_HSTRING (strTemp) << endl;

				i ++;
			}

			int iCountPass = 66666;
			clktEntry =  clock ();
			for (i = 0; i < iCountPass; i ++) {
				strTemp = _T ("Формат шаблона");
				strTemp.appendChar (':');
				strTemp.appendChar (' ');
				strTemp.append ("ПОЛНЫЙ_ПУТЬ\\имя файла.рсш");
				strTemp.cut (0, 7);
				strTemp.insert (0, "шаблона");
				strTemp = strTemp.left (strTemp.length () - 6);
			}
			clktCur = clock ();

			strTemp.free ();

			cout << "HString ms = " << clktCur - clktEntry << endl;

			CString strCTemp;
			clktEntry =  clock ();
			for (i = 0; i < iCountPass; i ++) {
				strCTemp = _T ("Формат шаблона");
				strCTemp.AppendChar (':');
				strCTemp.AppendChar (' ');
				strCTemp.Append (_T ("ПОЛНЫЙ_ПУТЬ\\имя файла.рсш"));
				strCTemp = strCTemp.Mid (0, 7);
				strCTemp.Insert (0, _T ("шаблона"));
				strCTemp = strCTemp.Left (strCTemp.GetLength () - 6);
			}
			clktCur = clock ();

			strCTemp.ReleaseBuffer ();

			cout << "CString ms = " << clktCur - clktEntry << endl;

			LPCTSTR lpstrFile = _T ("ПОЛНЫЙ_ПУТЬ\\имя файла.рсш");
			strTemp = HString (_T ("Формат шаблона '"));
			strTemp = (HString (lpstrFile)).mid ((HString (lpstrFile)).reverseFind ('\\') + 1, (HString (lpstrFile)).reverseFind ('.') - (HString (lpstrFile)).reverseFind ('\\') - 1);
			strTemp = HString (_T ("Формат шаблона '")) + (HString (lpstrFile)).mid ((HString (lpstrFile)).reverseFind ('\\') + 1, (HString (lpstrFile)).reverseFind ('.') - (HString (lpstrFile)).reverseFind ('\\') - 1) + HString (_T ("' известен?"));
			
			strCTemp = CString (_T ("Формат шаблона '")) + (CString (lpstrFile)).Mid ((CString (lpstrFile)).ReverseFind ('\\') + 1, (CString (lpstrFile)).ReverseFind ('.') - (CString (lpstrFile)).ReverseFind ('\\') - 1) + CString (_T ("' известен?"));
			strCTemp = "666";
			strCTemp.Empty ();	
			
			cout << WC_HSTRING (strTemp) << endl;
			i = AfxIsValidString (strTemp.GetBuffer ());
			
			CString strError ("Eroor.. Ошибка.. Eroor.. Ошибка.. Eroor.. Ошибка.. Eroor.. Ошибка..");
			strTemp.format (_T ("%S"), WC_HSTRING (HString (strError.GetBuffer ())));
			strError.Empty ();
			
			strTemp += strLog;
			
			unsigned char delim [] = "yatqdfq iuewrgfiuerh shdgoegh", seps [] = " ", *intermediate = 0x0, *ret = 0x0;
			//ret = _mbstok_s ((unsigned char *) delim, (unsigned char *) seps, &intermediate);
			//
			//iRes = intermediate - delim;
			
			strTemp = HString ((char *) delim);
			iRes = 0;
			strLog = strTemp.tokenize (HString (" "), iRes);
			
			while (strLog.isEmpty () == 0)
			{
				strLog = strTemp.tokenize (HString (" "), iRes);
			}
			
			strTemp = "006";
			strTemp.format (_T ("%i"), strTemp.number () + 666);
			
			//strTemp = "\17\18\19 \t 006\t \16\14\15";
			strTemp = _T (" \t  яЯБб \t 006\t \16\14\15");
			strTemp.trim ();
		}
		else
			; //Не инициализирован HTHREAD_LOGFILE

		DELETE_HTHREAD_FILELOG

		strTemp.free ();
		strLog.free ();
	    
		delete objTestMemory;
#ifdef _DEBUG
		g_memStateEnd.Checkpoint ();
		if (g_memStateDiff.Difference (g_memStateBegin, g_memStateEnd))
		{
			g_memStateDiff.DumpAllObjectsSince ();
			g_memStateDiff.DumpStatistics ();
		}
		else
			;
	        
		g_memStateBegin.DumpAllObjectsSince ();
		g_memStateBegin.DumpStatistics ();
#endif
	}
	else
		;

	getch ();

	return 0;
}


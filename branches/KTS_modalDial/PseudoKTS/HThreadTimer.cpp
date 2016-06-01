#include "stdafx.h"

#include "HThreadTimer.h"

EXT_DECLARE_HTHREAD_FILELOG

EXT_DECLARE_HDATETIMEINTERVAL

unsigned int g_arTimeCatchValue [COUNT_INDEX_TIME + 2];
HString g_arTimeCatchName [] = {_T ("SECOND_05"), _T ("SECOND_10"), _T ("SECOND_15"), _T ("MINUTE_01"), _T ("MINUTE_03"), _T ("MINUTE_05"), _T ("MINUTE_30"), _T ("HOUR_01"), _T ("DAY_01"), _T ("MONTH_01"), _T ("YEAR_01")};
time_t g_arTimeFixed [COUNT_INDEX_TIME + 2];

//BOOL g_arTimeCatchIsSend [COUNT_TIME_CATCH] = {0, 0, 0, 0};

VOID CALLBACK callbackTimer (HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    int indx;
    //SYSTEMTIME stmNow; GetSystemTime (&stmNow);
    //TRACE ("Системное время: %i\n", dwTime);
    time_t tmtNow; time (&tmtNow);

    //Вариант №1
    /*unsigned short arTimeCurrent [COUNT_TIME_CATCH];
    for (indx = SECOND_15; indx < COUNT_TIME_CATCH; indx ++) {
        arTimeCurrent [indx] = (stmNow.wMinute * 60 + stmNow.wSecond) % g_arTimeCatchValue [indx];
    }*/
    
    for (indx = 0; indx < COUNT_INDEX_TIME + 2; indx ++) {
        //if ((arTimeCurrent [indx] == 0) && (! g_arTimeCatchIsSend [indx])) {
        //if ((arTimeCurrent [indx] == 0) && (stmNow.wMilliseconds < 110)) {
        //if ((CTime (stmNow) - CTime (g_arTimeFixed [indx])).GetTotalSeconds () == g_arTimeCatchValue [indx]) {
        if (! ((g_arTimeFixed [indx] + g_arTimeCatchValue [indx]) > tmtNow)) {
            //g_arTimeCatchIsSend [indx] = 1;

           g_arTimeFixed [indx] = tmtNow - tmtNow % g_arTimeCatchValue [indx];

            //TRACE ("Системное время: %i ч %i мин %i сек %i мс = (%i)\n", stmNow.wHour, stmNow.wMinute, stmNow.wSecond, stmNow.wMilliseconds, indx);
            ::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_USER_THREAD_TIMER, (WPARAM) g_arTimeCatchName [indx].GetBuffer (), g_arTimeCatchValue [indx]);
        }
        else
            /*if (g_arTimeCatchIsSend [indx] == 0) {
            }
            else
                g_arTimeCatchIsSend [indx] = 0*/;
    }
}

HThreadTimer::HThreadTimer (void) : m_idTimer (0) {
}

HThreadTimer::~HThreadTimer (void) {
}

BEGIN_MESSAGE_MAP (HThreadTimer, HWinThread)
    ON_THREAD_MESSAGE (WM_USER_THREAD_RESUME, &OnResumeRun)
END_MESSAGE_MAP()

int HThreadTimer::InitInstance (void) {
    int iRes = HWinThread::InitInstance ();
    
    //SYSTEMTIME stmNow; GetSystemTime (&stmNow);
    //FILETIME ftmNow; SystemTimeToFileTime (&stmNow, &ftmNow);
    //CTime tmCurrent = CTime::GetCurrentTime ();
    time_t tmtNow; time (&tmtNow);

    if (! (g_arTimeCatchName == 0x0))
        if (sizeof (g_arTimeCatchName)) {
            ASSERT ((sizeof (g_arTimeCatchName) / sizeof (g_arTimeCatchName [0])) == COUNT_INDEX_TIME + 2); //SECOND_10 == -1

            for (int indx = 0; indx < COUNT_INDEX_TIME + 2; indx ++) {
                g_arTimeCatchValue [indx] = PTR_DATETIMEINTERVAL->GetInterval (g_arTimeCatchName [indx]);
                //g_arTimeFixed [indx] = stmNow;
                g_arTimeFixed [indx] = tmtNow - tmtNow % g_arTimeCatchValue [indx];
            }
        }
        else
            iRes = FALSE; //Нет ни одного элемента в 'g_arTimeCatchName'
    else
        iRes = FALSE; //Массив 'пустой'

    return iRes;
}

int HThreadTimer::ExitInstance (void) {
    if (m_idTimer > 0)
        KillTimer (0x0, m_idTimer);
    else
        ;

    return HWinThread::ExitInstance ();
}

afx_msg void HThreadTimer::OnResumeRun (WPARAM, LPARAM ) {
    m_idTimer = SetTimer (0x0, ID_TIMER, 100, callbackTimer);
}

#include "stdafx.h"
#include "HThreadUndoClear.h"

#if defined PTR_DB
#include "UserKTS.h"
extern CUserKTSApp
#else
extern CWinApp
#endif
theApp;

EXT_DECLARE_HTHREAD_FILELOG

IMPLEMENT_DYNCREATE(HThreadUndoClear, HWinThread)

HThreadUndoClear::HThreadUndoClear (void) {
    FILELOG_VARIABLE
    FILELOG_IF_WRITE (THREAD_UNDO_CLEAR)
    strLog.format (_T ("%S"), _T (" онструктор потока 'HThreadUndoClear'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), THREAD_UNDO_CLEAR);
    FILELOG_ENDIF
}

HThreadUndoClear::~HThreadUndoClear (void) {
	FILELOG_VARIABLE
    FILELOG_IF_WRITE (HDEBUG)
    strLog.format (_T ("%S"), _T ("ƒ≈структор потока 'HThreadUndoClear'"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);
    FILELOG_ENDIF
}

BOOL HThreadUndoClear::InitInstance () {
	// TODO:  perform and per-thread initialization here	
	return HWinThread::InitInstance ();
}

int HThreadUndoClear::ExitInstance () {
	// TODO:  perform any per-thread cleanup here	
	return HWinThread::ExitInstance ();
}

BEGIN_MESSAGE_MAP(HThreadUndoClear, HWinThread)
	ON_THREAD_MESSAGE (WM_USER_THREAD_RESUME, &HThreadUndoClear::OnResumeRun)
END_MESSAGE_MAP()

//int HThreadUndoClear::Run ()
afx_msg void HThreadUndoClear::OnResumeRun (WPARAM wp, LPARAM lp) {
    SYSTEMTIME stmRun = *(SYSTEMTIME *) GetDataOfId (TIME_RUN);
    CTime tmRun = CTime (stmRun);
    tmRun += 24 * 60 * 60;
    tmRun.GetAsSystemTime (stmRun);
    AddData (TIME_RUN, &stmRun, sizeof (SYSTEMTIME));

    AddData (NUMBER_IDPROGRESSBAR, &lp, sizeof (int));

    DWORD idExitCodeThread = m_idExitCodeThread;
    //CWinApp *theApp = ::AfxGetApp ();

    HWND hWndProgressCtrl = (HWND) wp;

    //«десь обман 'link.exe', который возвращает error - ::Run () must return a value
    //Ќо Ќ≈ќЅ’одимо завершить ѕќ“ќ  вызовом 'AfxEndThread'
    //с этим  ќƒом имеем лишь: warning - not all paths return a value
    if (false) {
        idExitCodeThread = -1;
        //return idExitCodeThread;
    }
    else
        ;

    int iCountFindFiles = 0;
    BOOL bFileFind = false;
    CFileFind ff;
    CTime tmLastWrite, tmModify, tmNow = CTime::GetCurrentTime ();
    HFileInfo *phfi = NULL;
    
    HString strPathUndo = stringData (GetDataOfId (STRING_DIRECTORYUNDO));
    CTimeSpan tmSpanHistory = CTimeSpan (*((long *) GetDataOfId (TIMESPAN_HISTORY))), tmSpanCurrent;
    
    strPathUndo.append (_T ("\\*.*"));
    
    //¬»«”јЋизаци€ “–≈Ѕует жертв
    //1- ый ѕќ»— ќвџй проход по файлам (по ƒ≈Ћу м. удал€ть —–ј«у, т.е се…„јс)
    bFileFind = ff.FindFile (strPathUndo.GetBuffer ());
    
    while (bFileFind) {
        bFileFind = ff.FindNextFile ();

        if (ff.IsDirectory ())
             continue;
        else
            ;
             
        iCountFindFiles ++;
    }

    //0 - здесь говорит, что сообщение отправл€етс€ от ѕќ“ќ а задачи очистки директории с резервными копиями
    PostMessage (hWndProgressCtrl, WM_USER_PROGRESS_RULES, 0, iCountFindFiles);
    
    //¬»«”јЋизаци€ “–≈Ѕует жертв
    //2- ой Ќј—“оящий… проход по файлам 
    bFileFind = ff.FindFile (strPathUndo.GetBuffer ());
    
    while ((iCountFindFiles > 0) && (bFileFind == TRUE)) {
        bFileFind = ff.FindNextFile ();

        if (ff.IsDirectory ())
             continue;
        else
            ;
             
        iCountFindFiles --;
        PostMessage (hWndProgressCtrl, WM_USER_PROGRESS_RULES, 0, 0);
        
        if (bFileFind) {
            ff.GetLastWriteTime (tmLastWrite);
            
            SYSTEMTIME stmLastWrite;
            tmLastWrite.GetAsSystemTime (stmLastWrite);
            
            //Sleep (100); //»митј÷и€
            
            tmSpanCurrent = tmNow - tmLastWrite;
            if (tmSpanCurrent > tmSpanHistory)
            {//”даление у—“ј–евш≈й резервнќ…  ќѕии
                CFile::Remove (ff.GetFilePath ());
            }
            else
                ;
        }
        else
            ;
    }
    
    strPathUndo.free ();
    
    ::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_USER_EXIT_UNDO_CLEAR, (WPARAM) hWndProgressCtrl, 0);
    
    idExitCodeThread = 0x0;

	//¬ариант є1
	//return idExitCodeThread;
    //¬ариант є2
	//AfxEndThread (idExitCodeThread, m_bAutoDelete);
}

#pragma once
//#include "stdafx.h"
#include "hwinthread.h"

class HThreadUndoClear : public HWinThread {
    DECLARE_DYNCREATE(HThreadUndoClear)

protected:
    // protected constructor used by dynamic creation
	
public:
    HThreadUndoClear (void);
    virtual ~HThreadUndoClear (void);
    
    enum ID_DATAITEM {STRING_DIRECTORYUNDO = 1977, TIMESPAN_HISTORY, TIME_RUN, NUMBER_IDPROGRESSBAR};
    
    BOOL InitInstance();
	int ExitInstance();

    afx_msg void OnResumeRun (WPARAM , LPARAM );
	//int Run();
	void Stop (void) {}

protected:
    //HString strDirectory; //Для размещения ВРЕМЕННо удаляемых файлов (для отмены действия в будущем)
    //CTimeSpan tmSpanHistory;
    //CTime tmRun;
    ////BOOL bRun;

	DECLARE_MESSAGE_MAP()
};

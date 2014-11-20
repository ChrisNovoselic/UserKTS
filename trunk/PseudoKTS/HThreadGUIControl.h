#pragma once

#if ! defined _HTHREAD_GUI_CONTROL_
#define _HTHREAD_GUI_CONTROL_

#include "HDlgGUIControl.h"

class HThreadGUIControl : public HWinThread {
public:
    HThreadGUIControl (void);
    virtual ~HThreadGUIControl (void);

    enum ID_DATAITEM {COUNT_ID_DATAITEM};

    int InitInstance (void);
    int ExitInstance (void);

	virtual void Stop (void) {}

    afx_msg void OnResumeRun (WPARAM, LPARAM );
    PTR_HANDLER_ONTHREADMESSAGE OnConnectToServer;
    PTR_HANDLER_ONTHREADMESSAGE OnMessageTimer;
    PTR_HANDLER_ONTHREADMESSAGE OnSec15ThreadGetData;
    PTR_HANDLER_ONTHREADMESSAGE OnEngPseudoThreadGetData;
    PTR_HANDLER_ONTHREADMESSAGE OnEngOriginThreadGetData;
    PTR_HANDLER_ONTHREADMESSAGE OnQueryUserNumberPTICControl;

protected:

private:
    void setItemTextOfGetData (int , int , WPARAM , LPARAM );

    DECLARE_MESSAGE_MAP()
};

#else
#endif //_HTHREAD_GUI_CONTROL_
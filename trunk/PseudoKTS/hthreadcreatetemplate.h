#pragma once

#if ! defined _HTHREAD_CREATE_TEMPLATE_
#define _HTHREAD_CREATE_TEMPLATE_

class HThreadCreateTemplate : public HWinThread {
public:
    HThreadCreateTemplate (void);
    virtual ~HThreadCreateTemplate (void);

    enum ID_DATAITEM {COUNT_ID_DATAITEM};

    int InitInstance (void);
    int ExitInstance (void);

	virtual void Stop (void) {}

    afx_msg void OnResumeRun (WPARAM, LPARAM );

private:
    DECLARE_MESSAGE_MAP()
};

#else
#endif //_HTHREAD_CREATE_TEMPLATE_
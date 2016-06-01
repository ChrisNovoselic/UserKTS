#pragma once
#include "hwinthread.h"

class HThreadTimer : public HWinThread {
public:
    enum {ID_TIMER = 666};

    HThreadTimer(void);
    virtual ~HThreadTimer(void);

    int InitInstance (void);
    int ExitInstance (void);

    afx_msg void OnResumeRun (WPARAM, LPARAM );

private:
    UINT_PTR m_idTimer;

	DECLARE_MESSAGE_MAP()
};


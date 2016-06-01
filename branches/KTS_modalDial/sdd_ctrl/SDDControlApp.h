#pragma once
//#include <hwinappkts.h>
class CSDDControlApp : public HWinApp {
public:
    CSDDControlApp(void);
    ~CSDDControlApp(void);

    virtual BOOL InitInstance ();
	virtual int ExitInstance ();

    DECLARE_MESSAGE_MAP ()
};


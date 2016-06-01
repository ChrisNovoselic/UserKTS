#if ! defined _HTHREAD_WRITEDATA_H_
#define _HTHREAD_WRITEDATA_H_

#include <hwinthread.h>

class HThreadWriteData : public HWinThread {
	DECLARE_DYNCREATE (HThreadWriteData)

public:
	HThreadWriteData (void);
	virtual ~HThreadWriteData (void);

	int InitInstance (void);
	int ExitInstance (void);

	afx_msg void OnResumeRun (WPARAM, LPARAM );
	afx_msg void OnOperationCreate (WPARAM, LPARAM );
	afx_msg void OnOperationModifyDT (WPARAM, LPARAM );

	enum ID_DATAITEM {};

private:
	DECLARE_MESSAGE_MAP ()
};

#else
#endif //_HTHREAD_WRITEDATA_H_